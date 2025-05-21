#ifndef STATUS_FOPS_H
#define STATUS_FOPS_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/io.h>

// External declarations
extern void __iomem *bram1_addr;
extern unsigned long bram1_start;
extern unsigned long bram1_end;

// Function declarations
ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
ssize_t status_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

// File operations structure
extern const struct proc_ops status_fops;

#endif /* STATUS_FOPS_H */ 