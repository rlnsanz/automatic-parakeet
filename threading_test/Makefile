obj-m += spork.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

modules_install:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd` modules_install
