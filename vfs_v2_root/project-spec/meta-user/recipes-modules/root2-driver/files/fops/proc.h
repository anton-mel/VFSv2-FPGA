#ifndef PROC_H
#define PROC_H

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/delay.h>

// Global variables
extern struct proc_dir_entry *paragon_dir;
extern struct proc_dir_entry *network_file;
extern struct proc_dir_entry *status_file;
extern struct proc_dir_entry *log_file;

// File operations structures
extern const struct proc_ops input_fops;
extern const struct proc_ops output_fops;
extern const struct proc_ops status_fops;
extern const struct proc_ops bram_fops;
extern const struct proc_ops generate_fops;
extern const struct proc_ops decode_fops;

#endif /* PROC_H */ 