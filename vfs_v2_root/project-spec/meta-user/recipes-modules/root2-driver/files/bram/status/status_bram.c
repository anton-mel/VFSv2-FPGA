/*
 * The Status BRAM is used for:
 * - Monitoring system status
 * - Handling input/output operations
 * - Managing communication between nodes
 */

#include "status_bram.h"

#define DRIVER_NAME "statusdriver"

// Global variables
void __iomem *bram1_addr;
unsigned long bram1_start;
unsigned long bram1_end;

// BRAM local structure
struct status_local {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

// BRAM probe function
static int status_probe(struct platform_device *pdev)
{
    struct resource *r_mem;
    struct device *dev = &pdev->dev;
    struct status_local *lp = NULL;
    int rc = 0;

    dev_info(dev, "Device Tree Probing\n");

    // Allocate memory for local data structure
    lp = (struct status_local *) kmalloc(sizeof(struct status_local), GFP_KERNEL);
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
    bram1_start = lp->mem_start;
    bram1_end = lp->mem_end;

    // Lock the BRAM memory region
    if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER_NAME)) {
        dev_err(dev, "Couldn't lock memory region at %p\n", (void *)lp->mem_start);
        rc = -EBUSY;
        goto error1;
    }

    // Map the BRAM memory into kernel space
    lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);

    bram1_addr = lp->base_addr;

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
static int status_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct status_local *lp = dev_get_drvdata(dev);

    // Free resources
    iounmap(lp->base_addr);
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    bram1_addr = NULL;
    return 0;
}

// Device tree match table
#ifdef CONFIG_OF
struct of_device_id status_of_match[] = {
    { .compatible = "xlnx,axi-bram-ctrl-status", },
    { /* end of table */ },
};
MODULE_DEVICE_TABLE(of, status_of_match);
#else
# define statusmodule_of_match
#endif

// Platform driver structure
struct platform_driver status_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .owner = THIS_MODULE,
        .of_match_table = status_of_match,
    },
    .probe = status_probe,
    .remove = status_remove,
};
