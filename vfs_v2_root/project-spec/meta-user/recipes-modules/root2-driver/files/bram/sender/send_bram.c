/*
 * send_bram.c - The Send BRAM driver for Paragon system
 * The Send BRAM is used for sending data to other nodes in the system.
 */

#include "send_bram.h"

#define DRIVER2_NAME "sbramdriver"

// Global variables
void __iomem *sbram_addr;
unsigned long sbram_start;
unsigned long sbram_end;

// BRAM local structure
struct sbram_local {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

// BRAM probe function
static int sbram_probe(struct platform_device *pdev)
{
    struct resource *r_mem;
    struct device *dev = &pdev->dev;
    struct sbram_local *lp = NULL;
    int rc = 0;

    dev_info(dev, "Device Tree Probing\n");

    // Allocate memory for local data structure
    lp = (struct sbram_local *) kmalloc(sizeof(struct sbram_local), GFP_KERNEL);
    if (!lp) {
        dev_err(dev, "Failed to allocate memory for bram\n");
        return -ENOMEM;
    }

    dev_set_drvdata(dev, lp);

    // Get memory resource for BRAM from device tree
    r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!r_mem) {
        dev_err(dev, "No valid memory resource found\n");
        rc = -ENODEV;
        goto error1;
    }

    lp->mem_start = r_mem->start;
    lp->mem_end = r_mem->end;
    
    //two variables to use in write function (watch out with multiple threads)
    sbram_start = lp->mem_start;
    sbram_end = lp->mem_end;

    // Lock the BRAM memory region
    if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER2_NAME)) {
        dev_err(dev, "Couldn't lock memory region at %p\n", (void *)lp->mem_start);
        rc = -EBUSY;
        goto error1;
    }

    // Map the BRAM memory into kernel space
    lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);

    sbram_addr = lp->base_addr;

    if (!lp->base_addr) {
        dev_err(dev, "Failed to map BRAM memory to kernel space\n");
        rc = -EIO;
        goto error2;
    }

    dev_info(dev, "status-BRAM mapped to address %p\n", lp->base_addr);
    return 0;

error2:
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    return rc;
}

// BRAM remove function
static int sbram_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct sbram_local *lp = dev_get_drvdata(dev);

    // Free resources
    iounmap(lp->base_addr);
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    sbram_addr = NULL;
    return 0;
}

// Device tree match table
#ifdef CONFIG_OF
struct of_device_id sbram_of_match[] = {
    { .compatible = "xlnx,axi-bram-ctrl-send", },
    { /* end of table */ },
};
MODULE_DEVICE_TABLE(of, sbram_of_match);
#else
# define sbram_of_match
#endif

// Platform driver structure
struct platform_driver sbram_driver = {
    .driver = {
        .name = DRIVER2_NAME,
        .owner = THIS_MODULE,
        .of_match_table = sbram_of_match,
    },
    .probe = sbram_probe,
    .remove = sbram_remove,
};
