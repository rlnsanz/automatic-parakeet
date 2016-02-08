/*
 *	This program will test the my_syscall syscall
 *	Author: Daniel D'Souza, Rolando S. Garcia
 */

#include <linux/unistd.h>
#include <stdio.h>
#define __NR_my_syscall 359

int main() {
	long j = syscall(__NR_my_syscall);
	printf("%ld\n", j);
	return 0;
}
