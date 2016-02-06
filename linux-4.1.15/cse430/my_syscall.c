/*
 *	my_syscall.c
 *	author: Daniel D'Souza
 */

#include <linux/syscalls.h>
#include <linux/kernel.h>

asmlinkage void sys_my_syscall(void) {
	printk(KERN_INFO "This is the new system call Daniel D'Souza implemented.\n");
}
