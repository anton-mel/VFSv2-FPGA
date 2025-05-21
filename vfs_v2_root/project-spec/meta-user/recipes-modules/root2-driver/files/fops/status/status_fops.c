#include "status_fops.h"

// Global variables
extern void __iomem *bram1_addr;
extern unsigned long bram1_start;
extern unsigned long bram1_end;

ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int len;
    uint64_t bram_data[3];
    char output[64];

    if (*pos > 0) {
        return 0;
    }
    printk(KERN_ALERT "Reading from BRAM\n");
    if (!bram1_addr) {
        pr_err("BRAM address is NULL\n");
        return -EFAULT;
    }

    printk(KERN_ALERT "bram1_addr is %p\n", bram1_addr);
    bram_data[0] = ioread64(bram1_addr);
    bram_data[1] = ioread64(bram1_addr + 8);
    bram_data[2] = ioread64(bram1_addr + 16);
    
    printk(KERN_ALERT "Read back 0x%llx from BRAM\n", bram_data[0]);
    len = snprintf(output, sizeof(output), "0x%08llx 0x%08llx 0x%08llx\n", bram_data[0], bram_data[1], bram_data[2]);

    printk(KERN_ALERT "Collected data\n");

    if (!access_ok(usr_buf, len)) {
        printk(KERN_ALERT "User buffer is not accessible\n");
        return -EFAULT;
    }

    if (count < len) {
        pr_err("User buffer too small\n");
        return -EINVAL;
    }

    int ret = copy_to_user((void __user *)usr_buf, output, len);
    printk(KERN_ALERT "copy_to_user returns %d\n", ret);
    if (ret != 0) {
        printk(KERN_ALERT "Failed to copy %d bytes to userspace\n", ret);
        return -EFAULT;
    }
    
    *pos += len;
    return len;
}

ssize_t status_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char input[64];
    unsigned int addr, val;

    if (count > sizeof(input) - 1)
        return -EINVAL;

    if (copy_from_user(input, usr_buf, count))
        return -EFAULT;

    input[count] = '\0';

    if (sscanf(input, "%u 0x%x", &addr, &val) != 2) {
        printk(KERN_ALERT "Invalid input format. Expected: <address> <value>\n");
        return -EINVAL;
    }

    if (addr > (bram1_end - bram1_start) / 8) {
        printk(KERN_ALERT "Address out of range.\n");
        return -EINVAL;
    }

    iowrite64(val, bram1_addr + (addr * 8)-8);

    printk(KERN_ALERT "Assigned 0x%x to status register %u\n", val, addr);

    return count;
}

const struct proc_ops status_fops = {
    .proc_read = status_read,
    .proc_write = status_write,
};
