#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// I needed to change this because this is 361 in my kernel -- James
#define __NR_get_addr 359

int main(int argc, char *argv[]) {
	char *ptr;
	syscall(__NR_get_addr, atoi(argv[1]), strtoul(argv[2], &ptr, 10));
	return 0;
}
