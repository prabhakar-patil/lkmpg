/*
 * procfs2.c - create a read-write file in /proc
 * e.g.
 * $ cat /proc/buffer1k
 * $ echo "HelloWorld" > /proc/buffer1k
 */
#include <linux/module.h>	/* Needed by modules */
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/proc_fs.h>	/* we're dealing with procfs */
#include <linux/uaccess.h>	/* put_user macro */

#define PROCFS_NAME "buffer1k"
#define PROCFS_MAX_SIZE 1024

#define BUF_LEN 80

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prabhakar Patil <prpatil12@gmail.com>");

static char msg[BUF_LEN];
static char *msg_ptr = NULL;

static char procfs_buf[PROCFS_MAX_SIZE];
static ssize_t procfs_buf_size=0;

/* declaratons */
ssize_t procfs_read(struct file *, char __user *, size_t, loff_t *);
ssize_t procfs_write(struct file *, const char __user *, size_t, loff_t *);
int procfs_open(struct inode *, struct file *);
int procfs_release(struct inode *, struct file *);

/* structure which hold information about /proc file */
struct proc_dir_entry *Our_Proc_File = NULL;

/* proc file entry file operations */
struct file_operations procfs_fops = {
	.read = procfs_read,
	.write= procfs_write,
	.open = procfs_open,
	.release = procfs_release
};

ssize_t procfs_read(struct file *filep,
		    char __user *buffer,
		    size_t length,
		    loff_t *offset)
{
	int read_bytes = 0;
	/*
	 * We give all the information in one go, so if user asks
	 * more information, the answer is always no
	 *
	 * This is important because the standard read function from
	 * the library would continue to issue read system call untill
	 * the kernel replies that it has no more information or buffer
	 * is filled
	 */
	if (*msg_ptr == 0) {
		/* we've finished read, return 0 */
		read_bytes = 0;
	}
	else {
		/* fill the buffer and return the buffer size */
		while (*msg_ptr && length) {
			put_user(*(msg_ptr++), buffer++);
			length--;
			read_bytes++;
		}
	}
	return read_bytes;
}

ssize_t procfs_write(struct file *filep,
		     const char __user *buffer,
		     size_t length,
		     loff_t *offset)
{
	/* get buffer size */	
	procfs_buf_size = length;
	if (procfs_buf_size > PROCFS_MAX_SIZE)
		procfs_buf_size = PROCFS_MAX_SIZE;

	/* write data to buffer */
	if (copy_from_user(procfs_buf, buffer, procfs_buf_size))
		return -EFAULT;

	printk("From user: %s\n", procfs_buf);
	return procfs_buf_size;
}


int procfs_open(struct inode *inodep, struct file *filep)
{
	sprintf(msg, "HelloWorld procfs!\n");
	msg_ptr = msg;
	try_module_get(THIS_MODULE);
	return 0;
}

int procfs_release(struct inode *inodep, struct file *filep)
{
	module_put(THIS_MODULE);
	return 0;
}

int init_module(void)
{
	/*
	 * deviation from lkmpg for 2.6.x,
	 * old 'create_proc_entry' call is no more exist in
	 * latest kernel, in my case 4.15.0
	 */
	Our_Proc_File = proc_create(PROCFS_NAME,
				    0666,
				    NULL,
				    &procfs_fops);
	if (Our_Proc_File == NULL) {
		remove_proc_entry (PROCFS_NAME, NULL);
		printk(KERN_INFO "Cannot initialize /proc/%s\n", PROCFS_NAME);
		return -ENOMEM;
	}


	printk(KERN_INFO "/proc/%s created\n", PROCFS_NAME);
	return 0; /* everything is okay*/
}

void cleanup_module(void)
{
	proc_remove(Our_Proc_File);
	printk(KERN_INFO "/proc/%s removed\n", PROCFS_NAME);
}
