#ifndef RECEIVE_BRAM_H
#define RECEIVE_BRAM_H

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of.h>

// Global variables
extern void __iomem *rbram_addr;
extern unsigned long rbram_start;
extern unsigned long rbram_end;

// Platform driver structure
extern struct platform_driver rbram_driver;

#endif /* RECEIVE_BRAM_H */ 
