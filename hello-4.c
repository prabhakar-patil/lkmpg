/*
 * hello-4.c - Demonstrate module documentation
 */
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed by KERN_INFO */
#include <linux/init.h>	  /* Need by by init,exit macros */

#define DRIVER_AUTHOR "Prabhakar Patil <prpatil12@gmail.com>"
#define DRIVER_DESC "A sample driver"

static int __init hello_4_init(void)
{
	printk(KERN_INFO "Hello, world 4\n");
	return 0;
}

static void __exit hello_4_exit(void)
{
	printk(KERN_INFO "Goodbye world 4\n");
}

module_init(hello_4_init);
module_exit(hello_4_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

/* This module uses /dev/testdevice */
MODULE_SUPPORTED_DEVICE("testdevice");

