obj-m = my_ramfs.o
my_ramfs-y = inode.o super.o
CCFLAGS += -Wall -g -rdynamic -Wunknown-pragmas
EXTRA_CFLAGS+= -I$(PWD) -g
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
install:
	sudo insmod my_ramfs.ko
uninstall:
	sudo rmmod my_ramfs
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
