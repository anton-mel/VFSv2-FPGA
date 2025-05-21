#ifndef TEST_FOPS_H
#define TEST_FOPS_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/io.h>

#define BUF_SIZE 256

int decoding_thread_fn(void *data);
ssize_t start_decoding(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos);
ssize_t generate_measurements(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos);

// External declarations
extern void __iomem *sbram_addr;
extern void __iomem *bram1_addr;
extern unsigned long bram1_start;
extern unsigned long bram1_end;

// File operations structures
extern const struct proc_ops generate_fops;
extern const struct proc_ops decode_fops;

#endif /* TEST_FOPS_H */ 