/*
 * hello-5.c - Demonstrate command line argument passing to a module
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prabhakar Patil <prpatil12@gmail.com>");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";
static int myintArray[2] = {-1, -1};
static int arr_argc = 0;

/*
 * module_param(foo, int, 0000)
 * The firts parameter is the parameter name
 * The second parameter is it's data type
 * The third paramter is the permission bits, for
 * exposing paramters in sysfs (if non-zero) at later stage
 */
module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");

module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");

module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");

module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");

/*
 * module_param_array(name, type, num, perm)
 * The first paramter is name of variable
 * The second paramter is data type of elements in array
 * The third paramter is pointer to the variable that will store the number of
 * elements of the array initialized by the user at module load time
 * The fourth paramter is the permission bits, for exposing paramters in sysfs
 */
module_param_array(myintArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintArray, "An array of integers");

static int __init hello_5_init(void)
{
	int i;
	printk(KERN_INFO "Hello, world 5\n========================\n");
	printk(KERN_INFO "myshort is a short integer: %hd\n", myshort);
	printk(KERN_INFO "myint is an integer: %d\n", myint);
	printk(KERN_INFO "mylong is a long integer: %ld\n", mylong);
	printk(KERN_INFO "mystring is a string: %s\n", mystring);

	for (i = 0; i < sizeof(myintArray)/sizeof(myintArray[0]); i++) {
		printk(KERN_INFO "myintArray[%d]: %d\n", i, myintArray[i]);
	}
	printk(KERN_INFO "got %d arguments for myintArray.\n", arr_argc);
	return 0;
}

static void __exit hello_5_exit(void)
{
	printk(KERN_INFO "Goodbye, word 5\n");
}

module_init(hello_5_init);
module_exit(hello_5_exit);
