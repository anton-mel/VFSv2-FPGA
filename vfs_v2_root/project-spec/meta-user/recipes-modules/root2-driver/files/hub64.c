/*  hub64.c - The Paragon Root FPGA kernel module.

* Copyright (C) 2013-2022 Xilinx, Inc
* Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program. If not, see <http://www.gnu.org/licenses/>.

*/
#include <linux/kernel.h>           // for printk
#include <linux/init.h>             // for __init, __exit
#include <linux/module.h>           // for module_init, module_exit
#include <linux/platform_device.h>  // for platform_driver_register
#include <linux/proc_fs.h>          // for proc_mkdir, proc_remove

#include "bram/status/status_bram.h"
#include "bram/sender/send_bram.h"
#include "bram/receiver/receive_bram.h"

#include "vfs/tree.h"

MODULE_LICENSE("GPL");
MODULE_VERSION("1.1");
MODULE_AUTHOR("Namitha Liyanage, Louka Verstraete, Anton Melnychuk: Yale Paragon");
MODULE_DESCRIPTION("Loadable module for root fpga - includes VFS");

static int __init paragon_module_init(void) {
    // Create the /proc/Paragon directory
    paragon_dir = proc_mkdir("Paragon", NULL);
    if (!paragon_dir) {
        printk(KERN_ALERT "Paragon not found\n");
        return -ENOMEM;
    }

    paragon_tree_init();
    add_root_hub();

    for (int i = 0; i < 2; i++) { 
        // Statically create 2 children for the root
        add_hub_process(root->node_id, i);
    }

    printk(KERN_INFO "Paragon Module Initialized\n");

    //registering platform driver for status bram of root
    printk("Setting up drivers for Status-BRAM, Send-BRAM and Receive-BRAM\n");
    platform_driver_register(&status_driver);
    platform_driver_register(&sbram_driver);
    platform_driver_register(&rbram_driver);

    return 0;
}

static void __exit paragon_module_exit(void) {
    delete_tree(root);
    printk(KERN_ALERT "Tree removed\n");
    // Remove the /proc/Paragon directory
    proc_remove(paragon_dir);
    printk(KERN_ALERT "Paragon removed\n");
    platform_driver_unregister(&status_driver);
    platform_driver_unregister(&sbram_driver);
    platform_driver_unregister(&rbram_driver);
    printk(KERN_ALERT "Status-BRAM, Send-BRAM and Receive-BRAM have been removed\n");
}

module_init(paragon_module_init);
module_exit(paragon_module_exit);
