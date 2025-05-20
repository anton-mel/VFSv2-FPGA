#ifndef SEND_BRAM_H
#define SEND_BRAM_H

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/of.h>

// Global variables
extern void __iomem *sbram_addr;
extern unsigned long sbram_start;
extern unsigned long sbram_end;

// Platform driver structure
extern struct platform_driver sbram_driver;

#endif /* SEND_BRAM_H */
