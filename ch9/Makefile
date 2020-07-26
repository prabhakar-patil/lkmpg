obj-m += sleep.o

KERNEL_DIR="/lib/modules/$(shell uname -r)/build"

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
	gcc -o cat_noblock cat_noblock.c

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f cat_noblock
