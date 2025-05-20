/*  testingmodule.c - The simplest kernel module.

* Copyright (C) 2013-2022 Xilinx, Inc
* Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program. If not, see <http://www.gnu.org/licenses/>.

*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h> // for platform_driver_register


#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/kthread.h>

#include <linux/proc_fs.h>
#include <linux/uaccess.h>  // for copy_to_user function
#include <linux/seq_file.h>  // For seq_read, seq_lseek
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Namitha Liyanage and Louka Verstraete: Yale Paragon");
MODULE_DESCRIPTION
    ("loadable module for root fpga - includes VFS");

#define BRAM_BASE_ADDR 0x20100000000  // The base address of axi_bram_ctrl_0
#define BRAM_SIZE 0x2000            // The size of the memory region you want to write to
#define BUF_SIZE 256
#define DRIVER_NAME "statusdriver"
#define DRIVER2_NAME "sbramdriver"
#define DRIVER3_NAME "rbramdriver"

void __iomem  *bram1_addr; // Global variable for base_addr of BRAM 1 linked to root
void __iomem  *sbram_addr; // Global variable for base_addr of BRAM 2 linked to FPGA1
void __iomem  *rbram_addr; // Global variable for base_addr of BRAM 2 linked to FPGA1

static struct proc_dir_entry *paragon_dir;
static struct proc_dir_entry *network_file;
static struct proc_dir_entry *status_file;
static struct proc_dir_entry *log_file;

struct paragon_tree_node {
    int node_id;
    int status;
    struct paragon_tree_node *children[4];
	struct proc_dir_entry *proc_entry;
	struct proc_dir_entry *parameter_entries[10];
};

// Global pointer to the root of the tree
static struct paragon_tree_node *root = NULL;

/*static int proc_show(struct seq_file *m, void *v);
static int proc_open(struct inode *inode, struct file *file);
static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
static int proc_release(struct inode *inode, struct file *file);
static ssize_t dummy_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
*/
static ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

//static struct task_struct *polling_thread;

static ssize_t input_write(struct file *file, const char __user *usr_buf, 
                          size_t count, loff_t *pos);
static ssize_t output_read(struct file *file, char __user *usr_buf,
                         size_t count, loff_t *pos);

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------IP ASSIGNMENT FUNCTIONS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

static uint8_t free_ip[64];

// Set a bit
static void set_ip(int n) {
    free_ip[n / 8] |= (1 << (n % 8));
}

// Clear a bit
static void clear_ip(int n) {
    free_ip[n / 8] &= ~(1 << (n % 8));
}

// Check if a bit is set
static bool is_ip_set(int n) {
    return free_ip[n / 8] & (1 << (n % 8));
}

// Function to find the first 0 bit
static int find_a_free_ip(void) {
    for (int i = 0; i < 64; i++) {
        if (free_ip[i] != 0xFF) { // Check if the byte is not all 1s
            for (int j = 0; j < 8; j++) {
                if (!(free_ip[i] & (1 << j))) {
                    return i * 8 + j; // Return the bit index
                }
            }
        }
    }
    return -1; // Return -1 if all bits are 1
}

static int ip_init(void) {
    
    memset(free_ip, 0, 64); // Initialize all bits to 0

    // Example usage
    set_ip(0);
    set_ip(1);
    printk(KERN_INFO "IP array Initialized. Host has IP 192.168.1.1\n");

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------TREE & NODE FUNCTIONS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

// Function to search for a node by ID
static struct paragon_tree_node* find_node_by_id(struct paragon_tree_node *node, int id) {
    struct paragon_tree_node *found_node;
	if (!node) 
        return NULL;

    if (node->node_id == id)
        return node;

    
    for (int i = 0; i < 4; i++) {
        found_node = find_node_by_id(node->children[i], id);
        if (found_node)
            return found_node;
    }

    return NULL;
}

// Helper function to find the parent of a node with a given ID
static struct paragon_tree_node* find_parent(struct paragon_tree_node *node, int id) {
    struct paragon_tree_node *parent;
	if (!node)
        return NULL;

    for (int i = 0; i < 4; i++) {
        if (node->children[i] && node->children[i]->node_id == id)
            return node;

        parent = find_parent(node->children[i], id);
        if (parent)
            return parent;
    }

    return NULL;
}


// Helper function to print branches and indentations
/*
static void print_branches(struct seq_file *m, int level) {
    for (int i = 0; i < level - 1; i++)
        seq_printf(m, "|   ");
    if (level > 0)
        seq_printf(m, "|-- ");
}

// Function to display the tree using seq_printf
static void display_tree(struct seq_file *m, struct paragon_tree_node *node, int level) {
    if (!node)
        return;

    print_branches(m, level);
    seq_printf(m, "Node IP: 192.168.1.%d, Status: Running\n", node->node_id);

    for (int i = 0; i < 4; ++i) {
        display_tree(m, node->children[i], level + 1);
    }
}

// New function to display the tree starting from root
static int display_tree_from_root(struct seq_file *m, void *v) {
    if (!root) {
        printk(KERN_INFO "The tree is empty.\n");
        return 0;
    }
    display_tree(m, root, 0);

	return 0;
}
*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------CREATING & DELETING TREE OF NODES--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------


// Modified create_node function
static struct paragon_tree_node* create_node(struct paragon_tree_node *parent, int child_index) {
    
	if (!parent->children[child_index]) {
		// Slot is available, create a new node
		struct paragon_tree_node *new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
		if (!new_node) {
			printk(KERN_ALERT "Failed to allocate memory for new node\n");
			return NULL;
		}
		new_node->node_id = find_a_free_ip();
		set_ip(new_node->node_id);
		new_node->status = 0;
		for (int j = 0; j < 4; j++)
			new_node->children[j] = NULL;

		// Add the new node to the parent's children array
		parent->children[child_index] = new_node;
		return new_node;
	}

    // No available slot in the parent's children
    printk(KERN_INFO "There is already a node at the given index\n");
    return NULL;
}

// Function to recursively delete a node and all its children
static void delete_tree(struct paragon_tree_node *node) {
    if (!node)
        return;
    printk(KERN_INFO "Started deleting tree\n");
    for (int i = 0; i < 4; i++) {
        if (node->children[i]){
            //delete_tree(node->children[i]); commented bcs children currently don't have status file
            clear_ip(node->node_id);
            proc_remove(node->proc_entry);
            kfree(node);
            printk(KERN_INFO "Child deleted\n");
            node->children[i] = NULL; 
        }
    }
    
	clear_ip(node->node_id);
	proc_remove(node->proc_entry);
    printk(KERN_INFO "Proc entry deleted\n");
	//proc_remove(node->parameter_entries[0]);
	//proc_remove(node->parameter_entries[1]);
	proc_remove(node->parameter_entries[2]);
	//proc_remove(node->parameter_entries[3]);
	//proc_remove(node->parameter_entries[4]);
	//proc_remove(node->parameter_entries[5]);
    
    kfree(node);
}

// Modified delete_node function to search and delete by ID
static void delete_node(int id) {
    struct paragon_tree_node *parent;
	if (!root)
        return;

    if (root->node_id == id) {
        // Special case if we're deleting the root
        // Assuming deep deletion: delete the root and all its descendants
        delete_tree(root);
        root = NULL;
        return;
    }

    parent = find_parent(root, id);
    if (!parent) {
        printk(KERN_ALERT "Node with ID %d not found\n", id);
        return;
    }

    for (int i = 0; i < 4; i++) {
        if (parent->children[i] && parent->children[i]->node_id == id) {
            // Found the node to delete
            delete_tree(parent->children[i]); // Assuming deep deletion
            parent->children[i] = NULL;
            return;
        }
    }
}
static int paragon_tree_init(void) {
    printk(KERN_INFO "Paragon Tree Module Initialized\n");

	ip_init();
    // Initialize the tree by creating the root node
    root = kmalloc(sizeof(*root), GFP_KERNEL);

	if (!root) {
        printk(KERN_ALERT "Failed to allocate memory for root node\n");
        return -ENOMEM;
    }

    //Set root to IP 2
    root->node_id = find_a_free_ip();
	set_ip(root->node_id);

    root->status = 0;  // Assuming a default status, modify as needed
    for (int i = 0; i < 4; i++)
        root->children[i] = NULL;

    return 0;
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------STATUS BRAM 1 DRIVER--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

/*Memory mapping variables
   - a pointer to the VF
   - a pointer to virtual memory address of device
*/
unsigned long bram1_start;
unsigned long bram1_end;
struct status_local {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

/*PROBE FUNCTION*/
static int status_probe(struct platform_device *pdev)
{
    struct resource *r_mem;
    struct device *dev = &pdev->dev;
    struct status_local *lp = NULL;
    int rc = 0;

    dev_info(dev, "Device Tree Probing\n");

    // Allocate memory for local data structure
    lp = (struct status_local *) kmalloc(sizeof(struct status_local), GFP_KERNEL);
    if (!lp) {
        dev_err(dev, "Failed to allocate memory for bram\n");
        return -ENOMEM;
    }

    dev_set_drvdata(dev, lp);

    // Get memory resource for BRAM from device tree
    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
        dev_err(dev, "No valid memory resource found\n");
        rc = -ENODEV;
        goto error1;
    }

    lp->mem_start = r_mem->start;
    lp->mem_end = r_mem->end;
    
    //two variables to use in write function (watch out with multiple threads)
    bram1_start = lp->mem_start;
    bram1_end = lp->mem_end;

    // Lock the BRAM memory region
    if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER_NAME)) {
        dev_err(dev, "Couldn't lock memory region at %p\n", (void *)lp->mem_start);
        rc = -EBUSY;
        goto error1;
    }

    // Map the BRAM memory into kernel space
    lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);

    bram1_addr = lp->base_addr;

    if (!lp->base_addr) {
        dev_err(dev, "Failed to map BRAM memory to kernel space\n");
        rc = -EIO;
        goto error2;
    }

    dev_info(dev, "status-BRAM mapped to address %p\n", lp->base_addr);
    return 0;

error2:
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    return rc;
}


/* REMOVE AND CLEAN UP */
static int status_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct status_local *lp = dev_get_drvdata(dev);

    // Free resources
    iounmap(lp->base_addr);
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    bram1_addr = NULL;
    return 0;
}

/*MATCH TABLE FOR THE DT*/
#ifdef CONFIG_OF
static struct of_device_id status_of_match[] = {
    { .compatible = "xlnx,axi-bram-ctrl-status", },
    { /* end of table */ },
};
MODULE_DEVICE_TABLE(of, status_of_match);

#else
# define statusmodule_of_match
#endif

/*PLATFORM DRIVER STRUCTURE*/
static struct platform_driver status_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = status_of_match,
    },
    .probe = status_probe,
    .remove = status_remove,
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------STATUS BRAM 1 OPERATIONS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------
static ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int len;
    uint64_t bram_data[3]; //buffer for the words read from the bram
    char output[64];	   //buffer for what you want to print as output


    // If *pos > 0, it means the data has already been read once; return 0 (EOF) necessary because cat automatically reads twice
    if (*pos > 0) {
        return 0;
    }
    printk(KERN_ALERT "Reading from BRAM\n");
    if (!bram1_addr) {
        pr_err("BRAM address is NULL\n");
        return -EFAULT; // Return error if bram1_addr is NULL
    }

    printk(KERN_ALERT "bram1_addr is %p\n", bram1_addr);
    //read first 3 words from bram
    bram_data[0] = ioread64(bram1_addr);
    bram_data[1] = ioread64(bram1_addr + 8);
    bram_data[2] = ioread64(bram1_addr + 16);
    
    printk(KERN_ALERT "Read back 0x%llx from BRAM\n", bram_data[0]);
    //format output
    len = snprintf(output, sizeof(output), "0x%08llx 0x%08llx 0x%08llx\n", bram_data[0], bram_data[1], bram_data[2]);
    //len = snprintf(output, sizeof(output), "Hello, world!\n");

    printk(KERN_ALERT "Collected data\n");

    //check if usr_buf is accessible
    if (!access_ok(usr_buf, len)) {
        printk(KERN_ALERT "User buffer is not accessible\n");
        return -EFAULT;
    } 

    if (count < len) {
        pr_err("User buffer too small\n");
        return -EINVAL;  // Return an error if the user buffer is too small
    }

    //check return value of copy_to_user, 0= works 
    int ret = copy_to_user(usr_buf, output, len);
    printk(KERN_ALERT "copy_to_user returns %d\n", ret);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }   
    
    *pos += len;
    return len; //return number of bytes read
}

/* WRITE FUNCTION */
static ssize_t status_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char input[64];          // Buffer for input string
    unsigned int addr, val;  // Variables to hold the parsed address and value

    // Check if the input fits in the buffer
    if (count > sizeof(input) - 1)
        return -EINVAL;

    // Copy the input from user space
    if (copy_from_user(input, usr_buf, count))
        return -EFAULT;

    // Null-terminate the input string
    input[count] = '\0';

    // Parse input to extract the address and value
    if (sscanf(input, "%u 0x%x", &addr, &val) != 2) {
        printk(KERN_ALERT "Invalid input format. Expected: <address> <value>\n");
        return -EINVAL;
    }

    // Calculate the word offset (each word is 4 bytes)-> 2x32 bits per address line
    if (addr > (bram1_end - bram1_start) / 8) {
        printk(KERN_ALERT "Address out of range.\n");
        return -EINVAL;
    }

    // Write the value to the specified BRAM address
    iowrite64(val, bram1_addr + (addr * 8)-8);

    printk(KERN_ALERT "Assigned 0x%x to status register %u\n", val, addr);

    return count;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------SEND BRAM DRIVER (communication)--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

/*Memory mapping variables:
   - a pointer to the VF
   - a pointer to virtual memory address of device
*/
unsigned long sbram_start;
unsigned long sbram_end;
struct sbram_local {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

/*PROBE FUNCTION*/
static int sbram_probe(struct platform_device *pdev)
{
    struct resource *r_mem;
    struct device *dev = &pdev->dev;
    struct sbram_local *lp = NULL;
    int rc = 0;

    dev_info(dev, "Device Tree Probing\n");

    // Allocate memory for local data structure
    lp = (struct sbram_local *) kmalloc(sizeof(struct sbram_local), GFP_KERNEL);
    if (!lp) {
        dev_err(dev, "Failed to allocate memory for bram\n");
        return -ENOMEM;
    }

    dev_set_drvdata(dev, lp);

    // Get memory resource for BRAM from device tree
    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
        dev_err(dev, "No valid memory resource found\n");
        rc = -ENODEV;
        goto error1;
    }

    lp->mem_start = r_mem->start;
    lp->mem_end = r_mem->end;
    
    //two variables to use in write function (watch out with multiple threads)
     sbram_start = lp->mem_start;
     sbram_end = lp->mem_end;

    // Lock the BRAM memory region
    if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER2_NAME)) {
        dev_err(dev, "Couldn't lock memory region at %p\n", (void *)lp->mem_start);
        rc = -EBUSY;
        goto error1;
    }

    // Map the BRAM memory into kernel space
    lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);

    sbram_addr = lp->base_addr;

    if (!lp->base_addr) {
        dev_err(dev, "Failed to map BRAM memory to kernel space\n");
        rc = -EIO;
        goto error2;
    }

    dev_info(dev, "status-BRAM mapped to address %p\n", lp->base_addr);
    return 0;

error2:
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    return rc;
}


/* REMOVE AND CLEAN UP */
static int sbram_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct sbram_local *lp = dev_get_drvdata(dev);

    // Free resources
    iounmap(lp->base_addr);
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    kfree(lp);
    dev_set_drvdata(dev, NULL);
     sbram_addr = NULL;
    return 0;
}

/*MATCH TABLE FOR THE DT*/
#ifdef CONFIG_OF
static struct of_device_id sbram_of_match[] = {
    { .compatible = "xlnx,axi-bram-ctrl-send", },
    { /* end of table */ },
};
MODULE_DEVICE_TABLE(of, sbram_of_match);

#else
# define sbram_of_match
#endif

/*PLATFORM DRIVER STRUCTURE*/
static struct platform_driver sbram_driver = {
    .driver = {
        .name = DRIVER2_NAME,
        .owner = THIS_MODULE,
        .of_match_table = sbram_of_match,
    },
    .probe = sbram_probe,
    .remove = sbram_remove,
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------RECEIVE BRAM DRIVER (communication)--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

/*Memory mapping variables:
   - a pointer to the VF
   - a pointer to virtual memory address of device
*/
unsigned long rbram_start;
unsigned long rbram_end;
struct rbram_local {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

/*PROBE FUNCTION*/
static int rbram_probe(struct platform_device *pdev)
{
    struct resource *r_mem;
    struct device *dev = &pdev->dev;
    struct rbram_local *lp = NULL;
    int rc = 0;

    dev_info(dev, "Device Tree Probing\n");

    // Allocate memory for local data structure
    lp = (struct rbram_local *) kmalloc(sizeof(struct rbram_local), GFP_KERNEL);
    if (!lp) {
        dev_err(dev, "Failed to allocate memory for bram\n");
        return -ENOMEM;
    }

    dev_set_drvdata(dev, lp);

    // Get memory resource for BRAM from device tree
    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
        dev_err(dev, "No valid memory resource found\n");
        rc = -ENODEV;
        goto error1;
    }

    lp->mem_start = r_mem->start;
    lp->mem_end = r_mem->end;
    
    //two variables to use in write function (watch out with multiple threads)
     rbram_start = lp->mem_start;
     rbram_end = lp->mem_end;

    // Lock the BRAM memory region
    if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER2_NAME)) {
        dev_err(dev, "Couldn't lock memory region at %p\n", (void *)lp->mem_start);
        rc = -EBUSY;
        goto error1;
    }

    // Map the BRAM memory into kernel space
    lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);

    rbram_addr = lp->base_addr;

    if (!lp->base_addr) {
        dev_err(dev, "Failed to map BRAM memory to kernel space\n");
        rc = -EIO;
        goto error2;
    }

    dev_info(dev, "status-BRAM mapped to address %p\n", lp->base_addr);
    return 0;

error2:
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    return rc;
}


/* REMOVE AND CLEAN UP */
static int rbram_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct rbram_local *lp = dev_get_drvdata(dev);

    // Free resources
    iounmap(lp->base_addr);
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    rbram_addr = NULL;
    return 0;
}

/*MATCH TABLE FOR THE DT*/
#ifdef CONFIG_OF
static struct of_device_id rbram_of_match[] = {
    { .compatible = "xlnx,axi-bram-ctrl-receive", },
    { /* end of table */ },
};
MODULE_DEVICE_TABLE(of, rbram_of_match);

#else
# define rbram_of_match
#endif

/*PLATFORM DRIVER STRUCTURE*/
static struct platform_driver rbram_driver = {
    .driver = {
        .name = DRIVER3_NAME,
        .owner = THIS_MODULE,
        .of_match_table = rbram_of_match,
    },
    .probe = rbram_probe,
    .remove = rbram_remove,
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------LEAF COMMUNICATION OPERATIONS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------
static ssize_t bram_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int len;
    uint64_t message;
    uint64_t  sbram_valid;
    uint64_t response; //buffer for the words read from the bram
    uint64_t payload;
    char output[128];	   //buffer for what you want to print as output
    unsigned long timeout; // For timeout calculation

    //to access node id
    const char *filename = file->f_path.dentry->d_name.name;
    int node_id;
    sscanf(filename, "status_%d", &node_id);

    // If *pos > 0, it means the data has already been read once; return 0 (EOF) necessary because cat automatically reads twice
    if (*pos > 0) {
        return 0;
    }

    printk(KERN_ALERT "Started requesting status of FPGA\n");
    if (!sbram_addr) {
        pr_err("BRAM address is NULL\n");
        return -EFAULT; // Return error if bram1_addr is NULL
    }

    //consructing message to request status, node id determines destination
    if (node_id == 3) {
        message = 0x0121000000000000; //destination: 0000_0001
    } else if (node_id == 4) {
        message = 0x0221000000000000; //destination: 0000_0010
    } else if (node_id == 5) {
        message = 0x0321000000000000; //destination: 0000_0011
    } else if (node_id == 6) {
        message = 0x0421000000000000; //destination: 0000_0100
    }

    printk(KERN_ALERT " sbram_addr is %p\n",  sbram_addr); //error log - check correct bram

    //write message to sending bram to request an update on the status, then set valid bit
    iowrite64(message, sbram_addr + 8);
    iowrite64(1, sbram_addr);

    printk(KERN_ALERT "Request sent to FPGA %d\n", node_id);//error log

    //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
    do {
        sbram_valid = ioread64(sbram_addr);
        msleep(10);  // Small delay to prevent busy waiting
    } while (sbram_valid == 1);
    
    printk(KERN_ALERT "Request acknowledged\n");//error log

    // Set timeout for 1 minute (60 seconds) - temporary for debugging
    timeout = jiffies + msecs_to_jiffies(60000);

    // Wait for rbram valid bit to be set to 1
    while (ioread64(rbram_addr) == 0) {
        if (time_after(jiffies, timeout)) {
            pr_err("Timeout waiting for rbram_valid\n");
            return -ETIMEDOUT; // Return timeout error
        }
        msleep(10); // Small delay to prevent busy waiting
    }

    // Wait for rbram valid bit to be set to 1
    /*do {
        rbram_valid = ioread64(rbram_addr);
        msleep(10);  // Small delay to prevent busy waiting
    } while (rbram_valid == 0);
    */
    printk(KERN_ALERT "Response received\n"); //error log
    response = ioread64(rbram_addr + 8);

    //Mask to isolate last 48 bits aka the payload (=response to the request)
    payload = response & 0x0000FFFFFFFFFFFF;
    len = snprintf(output, sizeof(output), "Status: 0x%016llx\n", payload); 
    printk(KERN_ALERT "Response read\n"); //error log

    //deassert valid bit in sending BRAM
    iowrite64(0, rbram_addr);
    printk(KERN_ALERT "Response acknowledged\n"); //error log

    printk(KERN_ALERT "Collected data\n");

    //check if usr_buf is accessible
    if (!access_ok(usr_buf, len)) {
        printk(KERN_ALERT "User buffer is not accessible\n");
        return -EFAULT;
    } 

    if (count < len) {
        pr_err("User buffer too small\n");
        return -EINVAL;  // Return an error if the user buffer is too small
    }

    //check return value of copy_to_user, 0= works 
    int ret = copy_to_user(usr_buf, output, len);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }   
    
    *pos += len;
    return len; //return number of bytes read
}

/* WRITE FUNCTION */

static ssize_t bram_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char input[64];          // Buffer for input string
    char output[200];
    unsigned int val;  // new value to be stored
    uint64_t message, payload, response, sbram_valid; 
    int len;
    unsigned long timeout; // For timeout calculation

    //to access node id
    const char *filename = file->f_path.dentry->d_name.name;
    int node_id;
    sscanf(filename, "status_%d", &node_id);

    // Check if the input fits in the buffer
    if (count > sizeof(input) - 1)
        return -EINVAL;

    // Copy the input from user space
    if (copy_from_user(input, usr_buf, count))
        return -EFAULT;

    // Null-terminate the input string
    input[count] = '\0';
    printk(KERN_ALERT "Reading data\n"); //error log

    // Parse input to extract the address and value
    if (sscanf(input, "0x%x", &val) != 1) {
        printk(KERN_ALERT "Invalid input format. Expected: <value>\n");
        return -EINVAL;
    }
    printk(KERN_ALERT "Data collected\n"); //error log

    //compose message
    if (node_id == 3) {
        message = 0x0122000000000000 | val;  //destination 01, request 0010_0010, payload = val
        printk(KERN_ALERT "Sent message to node 3\n"); //error log
    } else if (node_id == 4) {
        message = 0x0222000000000000 | val; //destination: 0000_0010
        printk(KERN_ALERT "Sent message to node 4\n"); //error log
    } else if (node_id == 5) {
        message = 0x0322000000000000 | val; //destination: 0000_0011
        printk(KERN_ALERT "Sent message to node 5\n"); //error log
    } else if (node_id == 6) {
        message = 0x0422000000000000; //destination: 0000_0100
    }
    
    // Write the value to the specified BRAM address
    iowrite64(message, sbram_addr + 8);
    iowrite64(1, sbram_addr); //set valid bit
    printk(KERN_ALERT "Data sent to FPGA %d\n", node_id); //error log
    //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
    do {
        sbram_valid = ioread64(sbram_addr);
        msleep(10);  // Small delay to prevent busy waiting
    } while (sbram_valid == 1);
    
    printk(KERN_ALERT "Message acknowledged\n");//error log

    // Set timeout for 1 minute (60 seconds) - temporary for debugging
    timeout = jiffies + msecs_to_jiffies(60000);
    
    // Wait for rbram valid bit to be set to 1
    while (ioread64(rbram_addr) == 0) {
        if (time_after(jiffies, timeout)) {
            pr_err("Timeout waiting for rbram_valid\n");
            return -ETIMEDOUT; // Return timeout error
        }
        msleep(10); // Small delay to prevent busy waiting
    }

    // Wait for rbram valid bit to be set to 1
    /*
    do {
        rbram_valid = ioread64(rbram_addr);
        msleep(10);  // Small delay to prevent busy waiting
    } while (rbram_valid == 0);
    */
    printk(KERN_ALERT "Response received\n");
    response = ioread64(rbram_addr + 8);


    //Mask to isolate last 48 bits (=response to the request)
    payload = response & 0x0000FFFFFFFFFFFF;
    len = snprintf(output, sizeof(output), "0x%llx\n", payload); 

    //deassert valid bit in sending BRAM
    iowrite64(0, rbram_addr);

    printk(KERN_ALERT "Response acknowledged\n"); //error log
    printk(KERN_ALERT "Collected data\n");

    //check if usr_buf is accessible
    if (!access_ok(usr_buf, len)) {
        printk(KERN_ALERT "User buffer is not accessible\n");
        return -EFAULT;
    } 

    if (count < len) {
        pr_err("User buffer too small\n");
        return -EINVAL;  // Return an error if the user buffer is too small
    }

    printk(KERN_INFO "Data stored. Old data:\n");
    //check return value of copy_to_user, 0= works 
    int ret = copy_to_user(usr_buf, output, len);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }   
    
    return count;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------GENERATE & DECODE OPERATIONS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

static ssize_t generate_measurements(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
{
    char *input; //buffer for input string
    char *line;
    uint64_t node_id;
    uint64_t message, destination;
    uint64_t sbram_valid;

    if (count >= BUF_SIZE)
        return -EINVAL;

    input = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (!input)
        return -ENOMEM;

    if (copy_from_user(input, user_buffer, count)) {
        kfree(input);
        return -EFAULT;
    }

    input[count] = '\0'; // Null-terminate the string
    printk(KERN_ALERT "Reading data\n"); //error log

    line = input;

    while (sscanf(line, "%llu", &node_id) == 1) {
        pr_info("Processing node ID: %lu\n", node_id);

        // Generate the message
        destination = ((node_id - 2) & 0xFF) << 56;
        printk ("destination: 0x%016llx\n" , destination);
        message = destination | 0x0023000000000000; // destination = node_id - 2 and header
        // Remaining bits are 0

         //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
        do {
            sbram_valid = ioread64(sbram_addr);
            msleep(10);  // Small delay to prevent busy waiting
        } while (sbram_valid == 1);

        // Write the message to the message address
        iowrite64(message, sbram_addr + 8); 

        // Set the valid bit to 1
        iowrite64(1, sbram_addr);

        pr_info("Sent message: 0x%016llX to node ID: %lu\n", message, node_id);

        // Move to the next node ID in the input
        line = strchr(line, ' ');
        if (!line)
            break;
        line++; // Skip the space
    }

    kfree(input);
    return count;
}

static struct task_struct *decoding_thread;
static bool decoding_active = false;

// Function to continuously send the message every few seconds
static int decoding_thread_fn(void *data)
{
    while (!kthread_should_stop()) {
        if (decoding_active && ioread64(sbram_addr) == 0) {
            iowrite64(0x000200000000000f, sbram_addr+8); //header_initialize_decoding = 8'h02 and added 0000f bcs peel_and_finish should be 1
            iowrite64(0x1, sbram_addr); //setting valid bit
            pr_info("Sent message to start decoding\n");
            msleep(500); // Sleep for 2 seconds
        } else {
            msleep(1000); // Sleep for 1 second if not active
        }
    }
    return 0;
}

// File operation to start/stop the decoding process
static ssize_t start_decoding(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
{
    char *buffer;
    char message[BUF_SIZE];
    
    if (count >= BUF_SIZE)
        return -EINVAL;

    buffer = kzalloc(BUF_SIZE, GFP_KERNEL);
    if (!buffer)
        return -ENOMEM;

    if (copy_from_user(buffer, user_buffer, count)) {
        kfree(buffer);
        return -EFAULT;
    }

    buffer[count] = '\0'; // Null-terminate the string

    sscanf(buffer, "%s", message); // Read the command (start or stop)

    if (strcmp(message, "start") == 0) {
        if (!decoding_active) {
            decoding_active = true;
            // Start the decoding thread
            decoding_thread = kthread_run(decoding_thread_fn, NULL, "decoding_thread");
            if (IS_ERR(decoding_thread)) {
                pr_err("Failed to create decoding thread\n");
                decoding_active = false;
                kfree(buffer);
                return PTR_ERR(decoding_thread);
            }
            pr_info("Started decoding thread\n");
        }
    } else if (strcmp(message, "stop") == 0) {
        if (decoding_active) {
            decoding_active = false;
            kthread_stop(decoding_thread); // Stop the decoding thread
            pr_info("Stopped decoding thread\n");
        }
    } else {
        pr_err("Invalid command. Use 'start' or 'stop'.\n");
    }

    kfree(buffer);
    return count;
}
//-----Hub specific functions-----//

static const struct proc_ops input_fops = {
    .proc_write = input_write,
};

static const struct proc_ops output_fops = {
    .proc_read = output_read,
};

static const struct proc_ops status_fops = {
    //.proc_open = proc_open,
    .proc_read = status_read,
    .proc_write = status_write,
    //.proc_lseek = seq_lseek,
    //.proc_release = proc_release,
};

//file operations for sending and receiving bram when communicating with the leaves from root
static const struct proc_ops bram_fops = {
    //.proc_open = proc_open,
    .proc_read = bram_read,
    .proc_write = bram_write,
    //.proc_lseek = seq_lseek,
    //.proc_release = proc_release,
};

//file operations to command the leaves to start generating measurement data
static const struct proc_ops generate_fops = {
    //.proc_open = proc_open,
    //.proc_read = bram_read,
    .proc_write = generate_measurements,
    //.proc_lseek = seq_lseek,
    //.proc_release = proc_release,
};

static const struct proc_ops decode_fops = {
    //.proc_open = proc_open,
    //.proc_read = bram_read,
    .proc_write = start_decoding,
    //.proc_lseek = seq_lseek,
    //.proc_release = proc_release,
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------CREATING VIRTUAL FILES & FOLDERS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

static int add_hub_process(int id, int child_index) {
	struct paragon_tree_node *parent_node;
	struct paragon_tree_node *new_node;
    char hub_name[20]; // Buffer to hold the hub name

    char status_name[16];

	parent_node = find_node_by_id(root, id);
	if (!parent_node) {
		printk(KERN_ALERT "Parent node with ID %d not found\n", id);
		return -1;
	}

	new_node = create_node(parent_node, child_index);
	if (!new_node) {
		printk(KERN_ALERT "Failed to create new node\n");
		return -1;
	}

    //to give each hub a different folder
    snprintf(hub_name, sizeof(hub_name), "fpga%d", child_index+1);

	new_node->proc_entry = proc_mkdir(hub_name, parent_node->proc_entry);
	if (!new_node->proc_entry) {
		printk(KERN_ALERT "Failed to create proc entry for new node\n");
		return -1;
	}

    
    // Create files within /proc/root
    // Assuming file operations (e.g., ip_fops) are defined elsewhere

	//new_node->parameter_entries[0] = proc_create("ip", 0666, new_node->proc_entry, &ip_fops);
    snprintf(status_name, sizeof(status_name), "status_%d", new_node->node_id);

	new_node->parameter_entries[2] = proc_create(status_name, 0666, new_node->proc_entry, &bram_fops);
	//new_node->parameter_entries[3] = proc_create("", 0666, new_node->proc_entry, &generate_fops);
	//new_node->parameter_entries[4] = proc_create("log", 0666, new_node->proc_entry, &log_hub_fops);
	//new_node->parameter_entries[5] = proc_create("reboot", 0666, new_node->proc_entry, &reboot_fops);
    //new_node->parameter_entries[1] = proc_create("bitstream", 0666, new_node->proc_entry, &bitstream_fops);
    printk(KERN_INFO "fpga %d created\n", new_node->node_id);

    return 0;
}

static int add_root_hub(void){
	root->proc_entry = proc_mkdir("root", paragon_dir);
	if (!root->proc_entry) {
		printk(KERN_ALERT "Failed to create proc entry for new node\n");
		return -1;
	}

    // Create files within /proc/hub
    // Assuming file operations (e.g., ip_fops) are defined elsewhere

	//root->parameter_entries[0] = proc_create("ip", 0666, root->proc_entry, &ip_fops);
	root->parameter_entries[2] = proc_create("status", 0666, root->proc_entry, &status_fops);
	root->parameter_entries[3] = proc_create("decode", 0666, root->proc_entry, &decode_fops);
	root->parameter_entries[4] = proc_create("generate", 0666, root->proc_entry, &generate_fops);
	//root->parameter_entries[5] = proc_create("reboot", 0666, root->proc_entry, &reboot_fops);
    //root->parameter_entries[1] = proc_create("bitstream", 0666, root->proc_entry, &bitstream_fops);
    printk(KERN_INFO "hub %d created\n", root->node_id);

    // Add new input/output files
    root->parameter_entries[5] = proc_create("input", 0666, root->proc_entry, &input_fops);
    root->parameter_entries[6] = proc_create("output", 0444, root->proc_entry, &output_fops);
    printk(KERN_INFO "I/O created\n");

	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------REGISTER PARAGON DEVICE--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

/*
static int proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello from the Petalinux kernel world!\n");
    return 0;
}


static int proc_open(struct inode *inode, struct file *file) {
    //bram1_addr = ioremap(BRAM_BASE_ADDR, BRAM_SIZE); -> bram1_addr 
    if (!bram1_addr) {
        printk(KERN_ERR "Mapping memory failed\n");
        return -ENOMEM;
    }
    // printk(KERN_INFO "Memory mapped\n");
    return single_open(file, proc_show, NULL);
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int len;
    char buffer[50];
    uint64_t value;

    if (*pos > 0 || count < sizeof(buffer)) {
        return 0;
    }

    if (!bram1_addr) {
        len = sprintf(buffer, "Memory not mapped\n");
    } else {
        value = ioread64(bram1_addr);
        len = snprintf(buffer, sizeof(buffer), "Value: 0x%x\n", value);
    }

    if (copy_to_user(usr_buf, buffer, len)) {
        return -EFAULT;
    }

    *pos += len;
    return len;
}

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char buffer[50];
    uint64_t value;

    if (count > sizeof(buffer) - 1) {
        return -EINVAL;
    }

    if (copy_from_user(buffer, usr_buf, count)) {
        return -EFAULT;
    }

    buffer[count] = '\0';  // Null-terminate the string

    if (kstrtouint(buffer, 0, &value)) {
        return -EINVAL;
    }

    if (bram1_addr) {
        iowrite64(value, bram1_addr);
    } else {
        printk(KERN_INFO "Memory not mapped\n");
        return -EIO;
    }

    return count;
}

static int proc_release(struct inode *inode, struct file *file) {
    if (bram1_addr) {
        iounmap(bram1_addr);
        bram1_addr = NULL;
        // printk(KERN_INFO "Memory unmapped\n");
    }
    return single_release(inode, file);
}

static ssize_t dummy_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    return 0;
}
*/

// Network view functions----------------------------------------------------------------------------------------------------------------------------------

// static int tree_display(struct seq_file *m, void *v) {
//     seq_printf(m, "  192.168.1.2 : Hub : Running\n");
//     seq_printf(m, "  |-192.168.1.3 : Hub : Running\n");
//     return 0;
// }

/*
static int network_open(struct inode *inode, struct file *file) {
    return single_open(file, display_tree_from_root, NULL);
}

static const struct proc_ops network_fops = {
    .proc_open = network_open,
    .proc_read = seq_read,
    .proc_write = dummy_write,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};
*/



// log view functions-----------------------------------------------------------------------------------------------------------------------------------------
/*
//static ssize_t log_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int len;
    char buffer[50];
    uint64_t value;

    if (*pos > 0 || count < sizeof(buffer)) {
        return 0;
    }

    if (!bram1_addr) {
        len = sprintf(buffer, "Memory not mapped\n");
    } else {
        value = ioread64(bram1_addr);
        if(value == 0){
            len = sprintf(buffer, "Root_hub/hub0 missing children\n");
        }
        else{
             len = sprintf(buffer, "Root_hub missing children\n");
        }
    }

    if (copy_to_user(usr_buf, buffer, len)) {
        return -EFAULT;
    }

    *pos = len;
    return len;
}

static const struct proc_ops log_fops = {
    .proc_open = proc_open,
    .proc_read = log_read,
    .proc_write = proc_write,
    .proc_lseek = seq_lseek,
    .proc_release = proc_release,
};
*/

// Polling thread function--------------------------------------------------------------------------------------------------------------------------------

/*static int polling_thread_func(void *data) {
    while (!kthread_should_stop()) {
        if (*+ == 1) {
            // Handle the case where the memory is 1
            printk(KERN_INFO "Memory is 1\n");
        } else if (*polling_addr == 0) {
            // Handle the case where the memory is 0
            // printk(KERN_INFO "Memory is 0\n");
        }
        ssleep(1);  // Sleep for a second
    }
    return 0;
}*/

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------FILE WRITE & READ--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------


static ssize_t input_write(struct file *file,
                           const char __user *usr_buf,
                           size_t count, loff_t *pos)
{
    char buf[64];
    uint64_t val;
    int ret;

    if (count >= sizeof(buf))
        return -EINVAL;

    if (!bram1_addr)
        return -ENODEV;
    
    if (copy_from_user(buf, usr_buf, count))
        return -EFAULT;
    buf[count] = '\0';

    if (kstrtou64(buf, 0, &val))
        return -EINVAL;

    /* BRAM write */
    iowrite64(val, bram1_addr); // no offset
    // printk(KERN_INFO "Wrote 0x%016llx to input BRAM\n", val);

    return count;
}

static ssize_t output_read(struct file *file,
                           char __user *usr_buf,
                           size_t count, loff_t *pos)
{
    char buf[64];
    uint64_t val;
    int len;

    if (*pos > 0)
        return 0;

    if (!bram1_addr)
        return -ENODEV;

    val = ioread64(bram1_addr + 8);
    // printk(KERN_INFO "Read 0x%016llx from output BRAM\n", val);

    /* format as decimal + newline */
    len = snprintf(buf, sizeof(buf), "%llu\n", val);
    if (len < 0 || count < len)
        return -EINVAL;

    if (copy_to_user(usr_buf, buf, len))
        return -EFAULT;

    *pos += len;
    return len;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------LOADING & UNLOADING--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

static int __init paragon_module_init(void) {

    int i;
    // Create the /proc/Paragon directory
    paragon_dir = proc_mkdir("Paragon", NULL);
    if (!paragon_dir) {
        printk(KERN_ALERT "Paragon not found\n");
        return -ENOMEM;
    }

    // Create files and subdirectory inside /proc/Paragon
    //network_file = proc_create("network", 0666, paragon_dir, &network_fops); // &network_fops for file operations
    //status_file = proc_create("status", 0666, paragon_dir, &status_fops);   // &status_fops
    //log_file = proc_create("log", 0666, paragon_dir, &log_fops);   // &issues_fops
    
	paragon_tree_init();
	add_root_hub();
	//add_hub_process(root->node_id, 0); //Arguments are parent and index of the child
    for (i = 0; i < 2; i++) { // Create 4 children for the root
        add_hub_process(root->node_id, i); // root->node_id is assumed to be the ID of the root node
    }
	// Create and start the kernel thread
    /*polling_thread = kthread_run(polling_thread_func, NULL, "my_polling_thread");
    if (IS_ERR(polling_thread)) {
        printk(KERN_ALERT "Failed to create the polling thread\n");
        return PTR_ERR(polling_thread);
    }*/

	printk(KERN_INFO "Paragon Module Initialized\n");
    
    //registering platform driver for status bram of root
    printk("Setting up drivers for Status-BRAM, Send-BRAM and Receive-BRAM\n");
    platform_driver_register(&status_driver);
    platform_driver_register(&sbram_driver);
    platform_driver_register(&rbram_driver);


    return 0;
}

static void __exit paragon_module_exit(void) {
    // Remove files and subdirectory
    //proc_remove(network_file);
    //proc_remove(status_file);
    //proc_remove(log_file);

    delete_tree(root);
    printk(KERN_ALERT "Tree removed\n");
    // Remove the /proc/Paragon directory
    proc_remove(paragon_dir);
    printk(KERN_ALERT "Paragon removed\n");
    platform_driver_unregister(&status_driver);
    platform_driver_unregister(&sbram_driver);
    platform_driver_unregister(&rbram_driver);
    printk(KERN_ALERT "Status-BRAM, Send-BRAM and Receive-BRAM have been removed\n");
}



module_init(paragon_module_init);
module_exit(paragon_module_exit);



