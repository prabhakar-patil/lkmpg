/*
 * chardev.h - the header file with ioctl defination
 */
#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>	/* Needed by _IOR macro */

/* device file name */
#define DEVICE_NAME "chardev2"

/* we cannot rely on dynamic major number since
 * ioctls need to know it
 */
#define MAJOR_NUM 100

/* set the message of device driver */
#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0, char*)
/* Her _IOR means, we're creating ioctl command number for
 * passing information from user space to kernel space
 * 1st arg: MAJOR_NUM of device
 * 2nd arg: number of commands, there could be several with different meaning
 * 3rd arg: the type of data passed from user to kernel module
 */

/* get the message of device driver
 * This IOCTL is used to output, get message of device driver.
 * Message is put in userspace buffer
 */
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char*)

/* get n'th byte from message
 * This ICCTL is used for both input and ouput
 * It receives from the user a number, n and returns
 * msg[n]
 */
#define IOCTL_GET_NTH_BYTE _IOR(MAJOR_NUM, 2, int)

#endif /* CHARDEV_H */
