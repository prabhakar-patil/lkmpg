/*
 * chardev.c - Creates read only charcter device that shows how many times you've read from devfile
 */
#include <linux/module.h>	/* Needed by every module */
#include <linux/kernel.h>	/* Needed since we are doing kernel dev */
#include <linux/fs.h>		/* Needed by struct file, struct file_operations */
/* Following include in original lkmpg book is <asm/uaccess.h>, but with recent kernel
 * my case (4.15) use linux/uaccess.h
 * reference: https://medium.com/@avenger.v14/hi-when-building-the-enhanced-example-with-character-device-i-encountered-a-build-error-for-55079354f704
 *            https://lkml.org/lkml/2017/9/30/189
 */
#include <linux/uaccess.h>

#define DEVICE_NAME "chardev"
#define BUF_LEN 80

#define DRIVER_AUTHOR "Prabhakar Patil <prpatil12@gmail.com>"
#define DRIVER_DESC "A sample character driver"

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

static int Major;		/* major number dyn allocated and given by kernel */
static int Device_Open = 0;	/* device opened or released */
static char msg[BUF_LEN];	/* message returned to user space */
static char *msg_ptr = NULL;

/* declarations */
int device_open(struct inode *, struct file *);
int device_release(struct inode *, struct file *);
ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);


struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.read = device_read,
	.write = device_write
};

int __init driver_init(void)
{
	printk(KERN_INFO "Hello, driver\n");

	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {
		printk(KERN_INFO "Device %s registration failed %d\n", DEVICE_NAME, Major);
		return Major;
	}

	printk(KERN_INFO "I was assigned %d major number. To talk to the\n", Major);
	printk(KERN_INFO "driver, create the device file with\n");
	printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, Major);
	return 0;
}

void __exit driver_exit(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
	printk(KERN_INFO "Goodbye, driver\n");
}

module_init(driver_init);
module_exit(driver_exit);

int device_open(struct inode *inode, struct file *file)
{
	static int count = 0;

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg, "I already told you %d times Hello world!\n", count++);
	msg_ptr = msg;
	try_module_get(THIS_MODULE);
	return 0;
}

int device_release(struct inode *inode, struct file *file)
{
	Device_Open--;

	/*
	 * Decrement the usage count, else once you open the device file,
	 * you will never get rid of this module
	 */
	module_put(THIS_MODULE);
	return 0;
}

ssize_t device_read(struct file *file,
		    char __user *buf,
		    size_t length,
		    loff_t *offset)
{
	int bytes_read = 0;

	/* convey end of file */
	if (*msg_ptr == 0)
		return 0;

	/* put data into user buffer */
	while (length && *msg_ptr) {
		/*
		 * Ther buf is in user data segment, not in kernel data
		 * segment, so '*'(dereference) assignment won't work.
		 * We've to user 'put_user' macro to copy data from kernel
		 * data segment to user data segment.
		 * put_user(value_to_put, user_space_address)
		 */
		put_user(*(msg_ptr++), buf++);
		bytes_read++;
		length--;
	}

	/* most read function returns number of bytes put in user buffer */
	return bytes_read;
}

ssize_t device_write(struct file *file,
		     const char __user *buf,
		     size_t length,
		     loff_t *offset)
{
	printk(KERN_ALERT "Sorry, This operation is not permitted on %s device\n", DEVICE_NAME);
	return -EPERM;
}
