#ifndef BRAM_FOPS_H
#define BRAM_FOPS_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/io.h>

// External declarations
extern void __iomem *sbram_addr;
extern void __iomem *rbram_addr;

// Function declarations
ssize_t bram_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
ssize_t bram_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

// File operations structure
extern const struct proc_ops bram_fops;

#endif /* BRAM_FOPS_H */ 