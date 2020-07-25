obj-m += chardev.o

KERNEL_DIR=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	gcc -o app app.c

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f app

