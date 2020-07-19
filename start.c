/*
 * start.c - Illustrates the multifile module
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* We're doing kernel work */

int init_module(void) 
{
	printk(KERN_INFO "Hello, world - this is kernel speaking\n");
	return 0;
}
