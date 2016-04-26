#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// I needed to change this because this is 361 in my kernel -- James
#define __NR_table_count 360

int main(int argc, char *argv[]) {
	syscall(__NR_table_count, atoi(argv[1]), atoi(argv[2]));
	return 0;
}
