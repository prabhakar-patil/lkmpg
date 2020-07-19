/*
 * stop.c - Illustrates multifile module
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed by KERN_INFO */

void cleanup_module(void)
{
	printk("Short is the life of a kernel module\n");
}
