#include "../bram/status/status_bram.h"
#include "../bram/sender/send_bram.h"
#include "../bram/receiver/receive_bram.h"

#include "proc.h"

#include <linux/kthread.h>
#include <linux/delay.h>

#define BUF_SIZE 256

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
    int ret = copy_to_user((void __user *)usr_buf, (const void *)output, len);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }   
    
    *pos += len;
    return len; //return number of bytes read
}

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
    int ret = copy_to_user((void __user *)usr_buf, (const void *)output, len);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }   
    
    return count;
}

// BRAM read/write operations
ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
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
    if (copy_to_user((void __user *)usr_buf, output, len)) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", len);
        return -EFAULT;
    }   
    
    *pos += len;
    return len; //return number of bytes read
}

ssize_t status_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
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

/// These functions are used for training purposes

ssize_t input_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    char buf[64];
    uint64_t val;

    if (count >= sizeof(buf))
        return -EINVAL;

    if (!bram1_addr)
        return -ENODEV;
    
    if (copy_from_user(buf, usr_buf, count))
        return -EFAULT;
    buf[count] = '\0';

    if (kstrtou64(buf, 0, &val))
        return -EINVAL;

    iowrite64(val, bram1_addr);
    // printk(KERN_INFO "Wrote 0x%016llx to input BRAM\n", val);

    return count;
}

ssize_t output_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
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

    len = snprintf(buf, sizeof(buf), "%llu\n", val);
    if (len < 0 || count < len)
        return -EINVAL;

    if (copy_to_user(usr_buf, buf, len))
        return -EFAULT;

    *pos += len;
    return len;
}


// // Decoding thread variables
// static struct task_struct *decoding_thread;
// static bool decoding_active = false;

// // Function to continuously send the message every few seconds
// static int decoding_thread_fn(void *data)
// {
//     while (!kthread_should_stop()) {
//         if (decoding_active && ioread64(sbram_addr) == 0) {
//             iowrite64(0x000200000000000f, sbram_addr+8); //header_initialize_decoding = 8'h02 and added 0000f bcs peel_and_finish should be 1
//             iowrite64(0x1, sbram_addr); //setting valid bit
//             pr_info("Sent message to start decoding\n");
//             msleep(500); // Sleep for 2 seconds
//         } else {
//             msleep(1000); // Sleep for 1 second if not active
//         }
//     }
//     return 0;
// }

// // File operation to start/stop the decoding process
// static ssize_t start_decoding(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
// {
//     char *buffer;
//     char message[BUF_SIZE];
    
//     if (count >= BUF_SIZE)
//         return -EINVAL;

//     buffer = kzalloc(BUF_SIZE, GFP_KERNEL);
//     if (!buffer)
//         return -ENOMEM;

//     if (copy_from_user(buffer, user_buffer, count)) {
//         kfree(buffer);
//         return -EFAULT;
//     }

//     buffer[count] = '\0'; // Null-terminate the string

//     sscanf(buffer, "%s", message); // Read the command (start or stop)

//     if (strcmp(message, "start") == 0) {
//         if (!decoding_active) {
//             decoding_active = true;
//             // Start the decoding thread
//             decoding_thread = kthread_run(decoding_thread_fn, NULL, "decoding_thread");
//             if (IS_ERR(decoding_thread)) {
//                 pr_err("Failed to create decoding thread\n");
//                 decoding_active = false;
//                 kfree(buffer);
//                 return PTR_ERR(decoding_thread);
//             }
//             pr_info("Started decoding thread\n");
//         }
//     } else if (strcmp(message, "stop") == 0) {
//         if (decoding_active) {
//             decoding_active = false;
//             kthread_stop(decoding_thread); // Stop the decoding thread
//             pr_info("Stopped decoding thread\n");
//         }
//     } else {
//         pr_err("Invalid command. Use 'start' or 'stop'.\n");
//     }

//     kfree(buffer);
//     return count;
// }

// static ssize_t generate_measurements(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
// {
//     char *input; //buffer for input string
//     char *line;
//     uint64_t node_id;
//     uint64_t message, destination;
//     uint64_t sbram_valid;

//     if (count >= BUF_SIZE)
//         return -EINVAL;

//     input = kzalloc(BUF_SIZE, GFP_KERNEL);
//     if (!input)
//         return -ENOMEM;

//     if (copy_from_user(input, user_buffer, count)) {
//         kfree(input);
//         return -EFAULT;
//     }

//     input[count] = '\0'; // Null-terminate the string
//     printk(KERN_ALERT "Reading data\n"); //error log

//     line = input;

//     while (sscanf(line, "%llu", &node_id) == 1) {
//         pr_info("Processing node ID: %lu\n", node_id);

//         // Generate the message
//         destination = ((node_id - 2) & 0xFF) << 56;
//         printk ("destination: 0x%016llx\n" , destination);
//         message = destination | 0x0023000000000000; // destination = node_id - 2 and header
//         // Remaining bits are 0

//         //error log - Wait for sbram valid bit to be set to 0 to check that the PL has processed the request
//         do {
//             sbram_valid = ioread64(sbram_addr);
//             msleep(10);  // Small delay to prevent busy waiting
//         } while (sbram_valid == 1);

//         // Write the message to the message address
//         iowrite64(message, sbram_addr + 8); 

//         // Set the valid bit to 1
//         iowrite64(1, sbram_addr);

//         pr_info("Sent message: 0x%016llX to node ID: %lu\n", message, node_id);

//         // Move to the next node ID in the input
//         line = strchr(line, ' ');
//         if (!line)
//             break;
//         line++; // Skip the space
//     }

//     kfree(input);
//     return count;
// }

// const struct proc_ops generate_fops = {
//     .proc_write = generate_measurements,
// };

// const struct proc_ops decode_fops = {
//     .proc_write = start_decoding,
// };

// File operations structures
const struct proc_ops input_fops = {
    .proc_write = input_write,
};

const struct proc_ops output_fops = {
    .proc_read = output_read,
};

const struct proc_ops status_fops = {
    .proc_read = status_read,
    .proc_write = status_write,
};

const struct proc_ops bram_fops = {
    .proc_read = bram_read,
    .proc_write = bram_write,
};
