#ifndef STATUS_BRAM_H
#define STATUS_BRAM_H

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of.h>

// Global variables
extern void __iomem *bram1_addr;
extern unsigned long bram1_start;
extern unsigned long bram1_end;

// Platform driver structure
extern struct platform_driver status_driver;

// Function declarations
ssize_t status_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
ssize_t status_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

// These are functions from the training
ssize_t input_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
ssize_t output_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

#endif /* STATUS_BRAM_H */ 
