/*
 * sleep.c - create /proc file, and if several process try open it
 * 	     at same time, put all but on to sleep
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define SUCCESS 0
#define BUF_LEN 80
#define PROCFS_NAME "sleep"

#define AUTHOR "Prabhakar Patil <prpatil@gmail.com>"
#define LICENSE "GPL"
#define DESCRIPTION "A sample non-blocking open call"

MODULE_AUTHOR(AUTHOR);
MODULE_LICENSE(LICENSE);
MODULE_DESCRIPTION(DESCRIPTION);

/* global declarations */
static struct proc_dir_entry *pde = NULL;
static char msg[BUF_LEN];

/* 1 if proc file is already opened */
static int already_open = 0;
/* Queue of process who want our file */
DECLARE_WAIT_QUEUE_HEAD(WaitQueue);

int procfs_open(struct inode *inodep, struct file* filep)
{
	/*
	 * if file's flag include O_NONBLOCK, it means process doesn't want
	 * to wait for file. In this case if file is already open, we should return
	 * failure code, -EAGAIN, meaning you will have try again later, instead of
	 * blocking a process which would rather stay awake
	 */
	if ((filep->f_flags & O_NONBLOCK) && (already_open==1))
		return -EAGAIN;

	/* this is correct place increment module ref count */
	try_module_get(THIS_MODULE);

	/*
	 * if file is already opened by another process and current process
	 * does not used NONBLOCK, then put current process to sleep.
	 * The process being put to sleep, will stay in open call itself
	 * untill another process does not close the file.
	 */
	while (already_open) {
		int i;
		unsigned long is_sig = 0;

		/*
		 * The following function puts current process to sleep.
		 * Execution will resumed right after this call, either because
		 * someone called wake_up(&WaitQueue), only procfs_release can do that
		 * while file is closed, or when signal, such as Ctrl-C, is sent to the
		 * processs.
		 */
		wait_event_interruptible(WaitQueue, !already_open);

		/* Here we will resume from above two conditions.
		 * If we're woke up because of signal, then we're no longer blocking again.
		 * We will return with error, -EINTR. This allows process to be stopped or killed
		 *
		 * Linux kernel has 64 signals in total.
		 * e.g. if any single bit in range of 64bit variable is ON, except blocked signals,
		 *      then it means signal is recieved to that process.
		 */
		for (i = 0; i < _NSIG_WORDS && is_sig == 0; i++)
			is_sig = (current->pending.signal.sig[i]) & ~(current->blocked.sig[i]);

		if (is_sig) {
			/*
			 * This is correct place to decrement module ref count here,
			 * because for the process where open is interrupted, there will
			 * no corresponding close. If we do not decrement module ref count
			 * here, we will be left with +ve module ref count. And there will be
			 * no way to bring this ref count to 0, giving us an immortal module,
			 * which can be killed by rebooting machine
			 */
			printk(KERN_INFO "signal received 0x%lx\n", is_sig);

			module_put(THIS_MODULE);
			return -EINTR;
		}
	}

	/* if we've got here means, already_open is 0,
	 * meaning file is ready to use, so mark the file is opened
	 */
	already_open = 1;

	return SUCCESS;
}

int procfs_release(struct inode *inodep, struct file* filep)
{
	/*
	 * set already_open to 0, so one of the process in WaitQueue will be able
	 * to open file successfully and set already_open back to one. All other
	 * process who waked up but did not get chance to open file successfully will
	 * be put back to sleep again in procfs_open.
	 */
	already_open = 0;

	/* wake up all the process, who are waiting for file in WaitQueue and they can have it */
	wake_up(&WaitQueue);
	module_put(THIS_MODULE);
	return SUCCESS;
}

ssize_t procfs_read(struct file *filep,
		    char __user *buffer,
		    size_t length,
		    loff_t *offsetp)
{
	static int finished = 0;
	int i;
	char message[BUF_LEN + 30];

	/* if reading is finished read call needs to return 0
	 * meaning there is no more data to put in user buffer
	 */
	if (finished == 1) {
		finished = 0;
		return 0;
	}

	sprintf(message, "Last input: %s\n", msg);
	for (i = 0; i < (BUF_LEN+30) && i < length && message[i]; i++)
		put_user(message[i], buffer+i);
	finished = 1;
	/* return how many bytes are put in user call buffer */
	return i;
}

ssize_t procfs_write(struct file *filep,
		     const char __user *buffer,
		     size_t length,
		     loff_t *offsetp)
{
	int wb;

	for (wb=0; wb < (BUF_LEN-1) && wb < length; wb++) {
		get_user(msg[wb], buffer+wb);
	}
	msg[wb++] = '\0';
	return wb;
}

struct file_operations procfs_fops = {
	.open = procfs_open,
	.release = procfs_release,
	.read = procfs_read,
	.write = procfs_write
};

int init_module(void)
{
	pde = proc_create(PROCFS_NAME, 0666, NULL, &procfs_fops);
	if (pde == NULL) {
		printk(KERN_ALERT "%s: failed to create entry for %s\n", __func__, PROCFS_NAME);
		return -ENOMEM;
	}

	printk(KERN_INFO "Hello sleep proc module\n");

	return 0;
}

void cleanup_module(void)
{
	proc_remove(pde);
	printk("Good bye sleep proc module\n");
}


