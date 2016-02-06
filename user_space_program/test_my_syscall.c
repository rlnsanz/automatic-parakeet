/*
 *	This program will test the my_syscall syscall
 *	Author: Daniel D'Souza (with help of course)
 */

#include <linux/unistd.h>
#include <stdio.h>
#define __NR_my_syscall 360

int main() {
	long j = syscall(__NR_my_syscall);
	printf("%ld", j);
	return 0;
}
