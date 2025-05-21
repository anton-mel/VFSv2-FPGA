#ifndef MULT_FOPS_H
#define MULT_FOPS_H

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

// Function declarations
ssize_t input_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
ssize_t output_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

// File operations structures
extern const struct proc_ops input_fops;
extern const struct proc_ops output_fops;

#endif /* MULT_FOPS_H */ 