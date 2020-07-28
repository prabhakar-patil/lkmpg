/*
 * print_string.c - send output to tty we're running on, regardless of
 * it's running through X11, telnet etc. We do this by printing the string
 * to the tty associated with current task.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/tty.h>

MODULE_LICENSE("GPL");

static void print_string(char *str)
{
	struct tty_struct *tty;
	tty = current->signal->tty;
}

int init_module(void)
{
	printk(KERN_INFO "tty print module inserted\n");
	print_string(KERN_INFO "tty print module inserted\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "tty print module removed\n");
}
