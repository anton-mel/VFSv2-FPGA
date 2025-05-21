#include "mult_fops.h"

// Global variables
extern void __iomem *bram1_addr;

ssize_t input_write(struct file *file,
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

    iowrite64(val, bram1_addr);

    return count;
}

ssize_t output_read(struct file *file,
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

    len = snprintf(buf, sizeof(buf), "%llu\n", val);
    if (len < 0 || count < len)
        return -EINVAL;

    if (copy_to_user((void __user *)usr_buf, buf, len))
        return -EFAULT;

    *pos += len;
    return len;
}

const struct proc_ops input_fops = {
    .proc_write = input_write,
};

const struct proc_ops output_fops = {
    .proc_read = output_read,
};
