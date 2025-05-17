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
#include <linux/kthread.h>

#include <linux/proc_fs.h>
#include <linux/uaccess.h>  // for copy_to_user function
#include <linux/seq_file.h>  // For seq_read, seq_lseek
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Louka Verstraete: Yale Paragon");
MODULE_DESCRIPTION
    ("loadable module for a leaf node");


#define DRIVER_NAME "statusdriver"
#define DRIVER2_NAME "sbramdriver"
#define DRIVER3_NAME "rbramdriver"
#define BRAM_SIZE 0x2000            // The size of the memory region you want to write to
#define USER_APP_PATH "/usr/bin/script_input"

void __iomem *bram1_addr = NULL; // Global variable for base_addr of BRAM 1 linked to root
void __iomem  *sbram_addr = NULL; // Global variable for base_addr of BRAM 2 linked to FPGA1
void __iomem  *rbram_addr = NULL; // Global variable for base_addr of BRAM 2 linked to FPGA1

// Path to the data file (modify this as needed)
static const char *filename = "/tmp/input_data_5_1.txt";

// Declare the new thread pointer
static struct task_struct *status_bram_thread;
static struct task_struct *polling_thread;
//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------STATUS BRAM 1 DRIVER--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

/*Memory mapping variables:
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
/*static ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
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
    
    printk(KERN_ALERT "Read back 0x%x from BRAM\n", bram_data[0]);
    //format output
    len = snprintf(output, sizeof(output), "0x%08x 0x%08x 0x%08x\n", bram_data[0], bram_data[1], bram_data[2]);
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
*/

/* WRITE FUNCTION */
/*
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

    // Calculate the word offset (each word is 4 bytes)
    if (addr > (bram1_end - bram1_start) / 4) {
        printk(KERN_ALERT "Address out of range.\n");
        return -EINVAL;
    }

    // Write the value to the specified BRAM address
    iowrite64(val, bram1_addr + (addr * 4)-4);

    printk(KERN_ALERT "Assigned 0x%x to status register %u\n", val, addr);

    return count;
}
*/
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
// -----------------------------------------------------USER-APPLICATION--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------
static bool reset_offset = false; //flag to track if reset is needed

static int start_generate(void) {
    static char *argv[] = {USER_APP_PATH, NULL};
    static char *envp[] = { "HOME=/", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL }; // Environment variables
    pr_info("Executing user-space application...\n");

    reset_offset = true;
    int ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC); // Synchronous execution
    if (ret < 0) {
        pr_err("Failed to execute user-space script: %d\n", ret);
    } else {
        pr_info("User-space script executed successfully.\n");
    }

    return ret;
}

static int read_data_file(const char *filename)
{
    struct file *file;
    char *buffer;
    static loff_t offset = 0;
    size_t buf_size = 9; // each data value is 8 bytes + \n + \0
    ssize_t read_size;
    char *line, *ptr;
    uint32_t value = 0;
    static bool in_sequence = false;
    uint32_t bram_offset = 64 + 4; //first 64 bytes are reserved and first word is valid bit

    if (!bram1_addr) {
        pr_err("BRAM address is not initialized\n");
        return -EINVAL;
    }
    printk("BRAM address is ok\n");

    // Reset offset if the flag is set
    if (reset_offset) {
        offset = 0;
        reset_offset = false;  // Clear flag after resetting
        pr_info("Offset reset to 0\n");
    }

    // Print the current offset value
    printk("Current file offset: %lld\n", offset);

    // Check if the valid bit is cleared by PL
    if (ioread32(bram1_addr + 64) != 0) {
        pr_info("Valid bit not cleared by PL, waiting for PL to process data.\n");
        return -EBUSY; // Return a busy error code
    }

    // Allocate memory for the buffer
    buffer = kzalloc(buf_size, GFP_KERNEL);
    if (!buffer)
        return -ENOMEM;
    printk("Memory for buffer allocated\n");

    file = filp_open(filename, O_RDONLY, 0);
    
    if (IS_ERR(file)) {
        pr_err("Failed to open file: %s\n", filename);
        kfree(buffer);
        return PTR_ERR(file);
    }
    printk("File opened\n");

    // Read the file line by line
    while ((read_size = kernel_read(file, buffer, buf_size , &offset)) > 0) {
        // Print the offset after each read
        printk("Offset after read: %lld\n", offset);

        //debugging
        if (read_size < 0) {
             pr_err("Error reading file: %s\n", filename);
             goto end;
        }
        printk("File read\n");

        
        line = buffer;
  
        line = strsep(&buffer, "\n"); //splits a buffer into lines by replacing \n with \0 charavters

        // Parse the hexadecimal value, converts the string line into an unsigned integer, interpreting it as a hexadecimal value
        value = simple_strtoul(line, &ptr, 16);
        
        if (!in_sequence) {
                    // Start of a new sequence, skip the ID
                    pr_info("Encountered new sequence ID: 0x%08X\n", value);
                    in_sequence = true;
                    continue;
                }
        if (value == 0xFFFFFFFF) {
            // Sequence terminator
            iowrite32(value, bram1_addr + bram_offset);
            pr_info("Encountered termination value 0xFFFFFFFF\n");

            // Set the valid bit
            iowrite32(0x1, bram1_addr + 64);
            pr_info("Valid bit set to 1 at BRAM offset 64\n");

            in_sequence = false; // Mark the end of the current sequence

            goto end;
        }

        // Write to BRAM
        if (bram_offset < BRAM_SIZE) {
            iowrite32(value, bram1_addr + bram_offset);
            pr_info("Wrote 0x%08X to BRAM offset 0x%08X\n", value, bram_offset);
            bram_offset += 4; // Increment by 4 bytes for each 32-bit value
        } else {
            pr_err("BRAM overflow detected\n");
            filp_close(file, NULL);
            kfree(buffer);
            return -ENOMEM;
        }
    }
    

end:

    // Check if the file read returned EOF
    if (read_size == 0) {
        pr_info("End of file reached.\n");
    } else if (read_size < 0) {
        pr_err("Error reading file: %s\n", filename);
    }

    filp_close(file, NULL);
    kfree(buffer);

    // Remember the offset for the next call
    printk("Final offset after this call: %lld\n", offset);
    return 0;
}

// Function for the new polling thread
static int status_bram_monitor_thread(void *data)
{
    while (!kthread_should_stop()) {
        uint32_t valid_bit;
        uint32_t value;
        uint32_t bram_offset = 64 + 4; //first 64 bytes are reserverd, valid bit is the next 4 bytes
        bool terminate = false;

        if (!bram1_addr) {
            pr_err("BRAM address is not initialized\n");
            return -EINVAL;
        }

        // Read the valid bit from the status BRAM
        valid_bit = ioread32(bram1_addr + 64); // Offset 64 contains the valid bit
        if (valid_bit == 1) {
            printk(KERN_INFO "Valid bit detected in status BRAM\n");

            // Read data from the BRAM until encountering 0xFFFFFFFF
            while (!terminate && bram_offset < BRAM_SIZE) {
                value = ioread32(bram1_addr + bram_offset);
                printk(KERN_INFO "Read value 0x%08X from BRAM offset 0x%08X\n", value, bram_offset);

                if (value == 0xFFFFFFFF) {
                    printk(KERN_INFO "Encountered termination value 0xFFFFFFFF in BRAM\n");
                    terminate = true;
                }

                bram_offset += 4; // Increment by 4 bytes for each 32-bit word

                if (bram_offset >= BRAM_SIZE) {
                    pr_err("Exceeded BRAM size limit, terminating early.\n");
                    break;
                }
            }


            // Clear the valid bit after processing
            iowrite32(0, bram1_addr + 64);
            printk(KERN_INFO "Valid bit cleared in status BRAM\n");

            //Reset
            bram_offset = 64+4;
            terminate = false;

            //send next batch of measurements
            printk("Next data batch is being read\n");
            read_data_file(filename);
            printk("Next measurements have been sent, decoding can start\n");
        }

        // Sleep for a short duration before polling again
        msleep(50); // 100 ms polling interval
    }

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------READ/WRITE OPERATIONS--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------


// Function to respond to ping
static void respond_to_ping(void) {
    uint64_t hardcoded_msg = 0x0020000000000000; // destination: 00, request type: 0010_0000, payload: 0000...0000
    iowrite64(hardcoded_msg, sbram_addr + 8);
    iowrite64(1, sbram_addr); // Set valid bit to 1
}
// Function to respond to unidentified request
static void send_hardcoded_message(void) {
    uint64_t hardcoded_msg = 0xFF000000000000; // send all 1's as an error response
    iowrite64(hardcoded_msg, sbram_addr + 8);
    iowrite64(1, sbram_addr); // Set valid bit to 1
}

// Function to read from status BRAM and then send it
static void send_status_bram_data(void) {
    printk(KERN_ERR "Processing status request\n"); //error log
    uint64_t sbram_valid;
    uint64_t status_data = ioread64(bram1_addr); // Read from status BRAM
	uint64_t payload = status_data & 0x0000FFFFFFFFFFFF; //isolate the last 48 bits of the message, aka the payload
	uint64_t message = payload | 0x0021000000000000; //concatenate payload with request type and destination
    iowrite64(message, sbram_addr + 8);
    iowrite64(1, sbram_addr); // Set valid bit to 1
    printk(KERN_ERR "Message sent\n"); //error log 

    //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
    do {
        sbram_valid = ioread64(sbram_addr);
        msleep(10);  // Small delay to prevent busy waiting
    } while (sbram_valid == 1);
    printk(KERN_ERR "Message acknowledged\n"); //error log 
}
// Function to write a value to 
static void write_status_bram(uint64_t received_message) {
    printk(KERN_ERR "Processing write request\n"); //error log
    uint64_t sbram_valid;
    uint64_t status_data = ioread64(bram1_addr); // Read from status BRAM

    uint64_t new_value = received_message & 0x0000FFFFFFFFFFFF; //read the value that needs to be placed in the status bram
    iowrite64(new_value, bram1_addr); //store new value in status bram

	uint64_t payload = status_data & 0x0000FFFFFFFFFFFF; //isolate the last 48 bits from the data
	uint64_t message = payload | 0x0022000000000000; //concatenate payload with request type and destination, include the old value
    iowrite64(message, sbram_addr + 8);
    iowrite64(1, sbram_addr); // Set valid bit to 1

    //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
    do {
        sbram_valid = ioread64(sbram_addr);
        msleep(10);  // Small delay to prevent busy waiting
    } while (sbram_valid == 1);
    printk(KERN_ERR "Message acknowledged\n"); //error log 
}


// Thread function to continuously monitor and process messages
static int bram_monitor_thread(void *data) {

    printk(KERN_INFO "Started bram_monitor_thread\n");
    while (!kthread_should_stop()) {
        if (!rbram_addr || !sbram_addr || !bram1_addr) {
            printk(KERN_ERR "BRAM addresses are not initialized yet!\n");
            msleep(100); // Wait longer before retrying
            continue;
        }

        // Check the valid bit in the receiving BRAM
        uint64_t rbram_valid = ioread64(rbram_addr) & 0x1;
        if (rbram_valid) {
            printk(KERN_ERR "Received a request\n"); //error log - message has arrived

            // Read the message from the next address line
            uint64_t received_message = ioread64(rbram_addr + 8);
            // Clear the valid bit after reading the message
            iowrite64(0, rbram_addr);

            printk(KERN_ERR "Request has been read\n"); //error log - valid bit is set to 0
            printk(KERN_ALERT " Message: %016llx\n", received_message); 
            // Check bits 56 to 48 aka the header, this is the type of request
			uint64_t request_type = received_message & 0xFF000000000000;
            printk(KERN_ALERT " Message: %016llx\n", request_type); 
            if (request_type == 0x0020000000000000) {
                printk(KERN_ERR "ping\n"); //error log 
                respond_to_ping(); // ping, 0010_0000
            } else if (request_type == 0x0021000000000000) {
                printk(KERN_ERR "send status\n"); //error log 
                send_status_bram_data();  // send status, 0010_0001
            } else if (request_type == 0x0022000000000000) {
                printk(KERN_ERR "write status\n"); //error log 
				write_status_bram(received_message); //set status, 0010_0010
			} else if (request_type == 0x0023000000000000) {
                    printk(KERN_ERR "generate measurements\n"); //error log 
                    start_generate();
                    printk("Generation was successful\n");
                    printk("First data batch is being read\n");
                    read_data_file(filename);
                    printk("First measurements have been sent\n");
                    //printk("Second data batch is being read\n");
                    //read_data_file(filename);
                    //printk("Second measurements have been sent, decoding can\n");
			}else {
                printk(KERN_ERR "unknown request type\n"); //error log 
				send_hardcoded_message(); //send that request was undefined
			}
        
        }
        msleep(10); // Sleep for a short period to avoid busy-waiting
        
    }
    return 0;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------LOADING MODULE--------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------

/* LOADING THE MODULE*/
int __init leaf_init(void)
{
    printk(KERN_ALERT "Setting up leaf\n");

	printk("Setting up drivers for Status-BRAM, Send-BRAM and Receive-BRAM\n");
    platform_driver_register(&status_driver);
    platform_driver_register(&sbram_driver);
    platform_driver_register(&rbram_driver);
	printk("All drivers have been set up, leaf initialized succesfully\n");

    	// Start the monitoring thread
    polling_thread = kthread_run(bram_monitor_thread, NULL, "bram_monitor_thread");
    if (IS_ERR(polling_thread)) {
        pr_err("Failed to create kernel thread\n");
        return PTR_ERR(polling_thread);
    }
    printk(KERN_INFO "Started the sending BRAM monitoring thread\n");

    // Start the status BRAM monitoring thread
    status_bram_thread = kthread_run(status_bram_monitor_thread, NULL, "status_bram_thread");
    if (IS_ERR(status_bram_thread)) {
        pr_err("Failed to create status BRAM kernel thread\n");
        return PTR_ERR(status_bram_thread);
    }
    printk(KERN_INFO "Started the status BRAM monitoring thread\n");

    return 0;

}

/*UNLOADING THE MODULE */
void __exit leaf_exit(void)
{
	//stop thread
	if (polling_thread) {
        kthread_stop(polling_thread);
    }
	printk(KERN_INFO "Thread stopped\n");

    if (status_bram_thread) {
        kthread_stop(status_bram_thread);
        printk(KERN_INFO "Stopped the status BRAM monitoring thread\n");
    }   
	//remove all drivers
    platform_driver_unregister(&status_driver);
    platform_driver_unregister(&sbram_driver);
    platform_driver_unregister(&rbram_driver);
    printk(KERN_ALERT "Status-BRAM, Send-BRAM and Receive-BRAM have been removed\n");
}

module_init(leaf_init);
module_exit(leaf_exit);