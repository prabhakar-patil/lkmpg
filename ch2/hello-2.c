/*
 * hello-2.c - Demonstrating module_init() and module_exit() macros
 * This is preferred over init_module() and cleanup_module() 
 */
#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed by KERN_INFO */
#include <linux/init.h>   /* Needed for init and exit macro */

static int __init hello_2_init(void)
{
	printk(KERN_INFO "Hello, world 2\n");
	return 0;
}

static void __exit hello_2_exit(void)
{
	printk(KERN_INFO "Goodbye, world 2\n");
}

module_init(hello_2_init);
module_exit(hello_2_exit);
