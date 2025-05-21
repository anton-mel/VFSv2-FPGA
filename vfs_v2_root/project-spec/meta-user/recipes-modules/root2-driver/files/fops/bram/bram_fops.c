#include <linux/jiffies.h>
#include "bram_fops.h"

// Global variables
extern void __iomem *sbram_addr;
extern void __iomem *rbram_addr;

ssize_t bram_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int len;
    uint64_t message;
    uint64_t sbram_valid;
    uint64_t response;
    uint64_t payload;
    char output[128];
    unsigned long timeout;

    // Get node id from filename
    const char *filename = file->f_path.dentry->d_name.name;
    int node_id;
    sscanf(filename, "status_%d", &node_id);

    if (*pos > 0) {
        return 0;
    }

    printk(KERN_ALERT "Started requesting status of FPGA\n");
    if (!sbram_addr) {
        pr_err("BRAM address is NULL\n");
        return -EFAULT;
    }

    // Construct message based on node_id
    if (node_id == 3) {
        message = 0x0121000000000000;
    } else if (node_id == 4) {
        message = 0x0221000000000000;
    } else if (node_id == 5) {
        message = 0x0321000000000000;
    } else if (node_id == 6) {
        message = 0x0421000000000000;
    }

    printk(KERN_ALERT "sbram_addr is %p\n", sbram_addr);

    iowrite64(message, sbram_addr + 8);
    iowrite64(1, sbram_addr);

    printk(KERN_ALERT "Request sent to FPGA %d\n", node_id);

    do {
        sbram_valid = ioread64(sbram_addr);
        msleep(10);
    } while (sbram_valid == 1);
    
    printk(KERN_ALERT "Request acknowledged\n");

    timeout = jiffies + msecs_to_jiffies(60000);

    while (ioread64(rbram_addr) == 0) {
        if (time_after(jiffies, timeout)) {
            pr_err("Timeout waiting for rbram_valid\n");
            return -ETIMEDOUT;
        }
        msleep(10);
    }

    printk(KERN_ALERT "Response received\n");
    response = ioread64(rbram_addr + 8);

    payload = response & 0x0000FFFFFFFFFFFF;
    len = snprintf(output, sizeof(output), "Status: 0x%016llx\n", payload);
    printk(KERN_ALERT "Response read\n");

    iowrite64(0, rbram_addr);
    printk(KERN_ALERT "Response acknowledged\n");

    if (!access_ok(usr_buf, len)) {
        printk(KERN_ALERT "User buffer is not accessible\n");
        return -EFAULT;
    }

    if (count < len) {
        pr_err("User buffer too small\n");
        return -EINVAL;
    }

    int ret = copy_to_user((void __user *)usr_buf, output, len);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }
    
    *pos += len;
    return len;
}

ssize_t bram_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char input[64];
    char output[200];
    unsigned int val;
    uint64_t message, payload, response, sbram_valid;
    int len;
    unsigned long timeout;

    const char *filename = file->f_path.dentry->d_name.name;
    int node_id;
    sscanf(filename, "status_%d", &node_id);

    if (count > sizeof(input) - 1)
        return -EINVAL;

    if (copy_from_user(input, usr_buf, count))
        return -EFAULT;

    input[count] = '\0';
    printk(KERN_ALERT "Reading data\n");

    if (sscanf(input, "0x%x", &val) != 1) {
        printk(KERN_ALERT "Invalid input format. Expected: <value>\n");
        return -EINVAL;
    }
    printk(KERN_ALERT "Data collected\n");

    if (node_id == 3) {
        message = 0x0122000000000000 | val;
        printk(KERN_ALERT "Sent message to node 3\n");
    } else if (node_id == 4) {
        message = 0x0222000000000000 | val;
        printk(KERN_ALERT "Sent message to node 4\n");
    } else if (node_id == 5) {
        message = 0x0322000000000000 | val;
        printk(KERN_ALERT "Sent message to node 5\n");
    } else if (node_id == 6) {
        message = 0x0422000000000000;
    }
    
    iowrite64(message, sbram_addr + 8);
    iowrite64(1, sbram_addr);
    printk(KERN_ALERT "Data sent to FPGA %d\n", node_id);

    do {
        sbram_valid = ioread64(sbram_addr);
        msleep(10);
    } while (sbram_valid == 1);
    
    printk(KERN_ALERT "Message acknowledged\n");

    timeout = jiffies + msecs_to_jiffies(60000);
    
    while (ioread64(rbram_addr) == 0) {
        if (time_after(jiffies, timeout)) {
            pr_err("Timeout waiting for rbram_valid\n");
            return -ETIMEDOUT;
        }
        msleep(10);
    }

    printk(KERN_ALERT "Response received\n");
    response = ioread64(rbram_addr + 8);

    payload = response & 0x0000FFFFFFFFFFFF;
    len = snprintf(output, sizeof(output), "0x%llx\n", payload);

    iowrite64(0, rbram_addr);

    printk(KERN_ALERT "Response acknowledged\n");
    printk(KERN_ALERT "Collected data\n");

    if (!access_ok(usr_buf, len)) {
        printk(KERN_ALERT "User buffer is not accessible\n");
        return -EFAULT;
    }

    if (count < len) {
        pr_err("User buffer too small\n");
        return -EINVAL;
    }

    printk(KERN_INFO "Data stored. Old data:\n");
    int ret = copy_to_user((void __user *)usr_buf, output, len);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }
    
    return count;
}

const struct proc_ops bram_fops = {
    .proc_read = bram_read,
    .proc_write = bram_write,
};
