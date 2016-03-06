# Code adapted from examples in linux/Documentation/kbuild/modules.txt

ifneq ($(KERNELRELEASE),)

obj-m := spork.o

else

default:
	$(MAKE) -C /lib/modules/`uname -r`/build M=$PWD 

modules:
	$(MAKE) -C /lib/modules/`uname -r`/build M=$PWD modules

modules_install:
	$(MAKE) -C /lib/modules/`uname -r`/build M=$PWD modules_install
endif
