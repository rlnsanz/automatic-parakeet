obj-m += spork.o

KDIR = /home/jkhutch3/Downloads/linux-4.1.15
DIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
