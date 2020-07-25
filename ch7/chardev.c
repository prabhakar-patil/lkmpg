/*
 * chardev.c - create input/output character device
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>		/* file_operations, char dev registration */
#include <linux/uaccess.h>	/* put_user, get_user macros */

#include "chardev.h"

#define SUCCESS 0
#define BUF_LEN 80

MODULE_AUTHOR("Prabhakar Patil <prpatil12@gmail.com>");
MODULE_DESCRIPTION("A sample IOCTL driver");
MODULE_LICENSE("GPL");

/* stores a message to give to user when asked */
static char msg[BUF_LEN];

/* points to Message, conveys how far reading process is reached */
static char *msg_ptr = NULL;

/* 1: decices is being used by a process. do not allow concurrent access to device
 * by multiple processes
 * 0: dovice is free to use
 */
static int device_open = 0;

int chardev_open(struct inode *inodep, struct file *filep)
{
	/* check if device is already in use by another process */
	if (device_open)
		return -EBUSY;

	/* increament ref count and device_open count as well */
	device_open++;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

int chardev_release(struct inode *inodep, struct file *filep)
{
	/* process no longer need device driver file */
	device_open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}

ssize_t chardev_read(struct file *filep,
		     char __user *buffer,
		     size_t length,
		     loff_t *offsetp)
{
	/* number bytes actually written to buffer*/
	int read_bytes = 0;

#ifdef DEBUG
	printk(KERN_INFO "%s(%p, %p, %ld)\n", __func__, filep, buffer, length);
#endif

	if (msg_ptr == NULL || *msg_ptr == 0)
		return 0;

	while (length && *msg_ptr != '\0') {
		put_user(*(msg_ptr++), buffer++);
		length--;
		read_bytes++;
	}
	return read_bytes;
}

ssize_t chardev_write(struct file *filep,
		      const char __user *buffer,
		      size_t length,
		      loff_t *offsetp)
{
	int i;
#ifdef DEBUG
	printk(KERN_INFO "%s(%p, %p, %ld)\n", __func__, filep, buffer, length);
#endif

	for (i = 0; i < length && i < BUF_LEN; i++)
		get_user(msg[i], buffer + i);
	msg_ptr = msg;

	/* return number of bytes received from userspace */
	return i;
}

/* This function is called whenver a process tries to do ioctl on our device file */
long chardev_ioctl(struct file *filep,
		   unsigned int ioctl_num,
		   unsigned long ioctl_param)
{
	int i;
	char *tmp = NULL;
	char ch;

	printk(KERN_INFO "IOCTL\n");
	/* switch according to ioctl */
	switch (ioctl_num) {
		case IOCTL_GET_MSG:
			tmp = (char *)ioctl_param;
			i = chardev_read(filep, tmp, 99, 0); /* why 99, no reason :)*/
			/* terminate user buffer with null */
			put_user('\0', tmp+i);
			break;
		case IOCTL_SET_MSG:
			tmp = (char *)ioctl_param;
			/* calculate user message buf size */
			get_user(ch, tmp);
			for (i=0; ch && i < BUF_LEN; i++, tmp++)
				get_user(ch, tmp);

			/* now set driver message by using driver call */
			chardev_write(filep, (char *)ioctl_param, i, 0);
			break;
		case IOCTL_GET_NTH_BYTE:
			return msg[ioctl_param];
		default:
			printk(KERN_ALERT "%s: %u invalid IOCTL\n", __func__, ioctl_num);
			break;

	}
	return SUCCESS;
}

/* device driver fops */
struct file_operations chardev_fops = {
	.read = chardev_read,
	.write = chardev_write,
	.open = chardev_open,
	.release = chardev_release,
	.unlocked_ioctl = chardev_ioctl
};

int init_module(void)
{
	int ret;
	ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &chardev_fops);
	if (ret < 0) {
		printk(KERN_ALERT "Device '%s' registration failed: %d\n", DEVICE_NAME, ret);
		return ret;
	}
	printk(KERN_INFO "Device '%s' registration successful. Major numver: %d\n", DEVICE_NAME, MAJOR_NUM);
	printk(KERN_INFO "If you wan to talk to device driver,\n");
	printk(KERN_INFO "you'll have to create a device file\n");
	printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, MAJOR_NUM);
	printk(KERN_INFO "The device file name is important\n");
	printk(KERN_INFO "ioctl will required the device file to talk to driver\n");
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk(KERN_INFO "Good bye %s driver\n", DEVICE_NAME);
}
