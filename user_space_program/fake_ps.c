/*
 *	This program will test the my_syscall syscall
 *	Author: Daniel D'Souza (with help of course)
 */

#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define __NR_my_syscall 360

struct CustomProcInfo {
	int pid;
	//char tty[64];
	//long long time;
	char command[16];
};

int main() {
	struct CustomProcInfo *info;
	int number_of_processes = 0;
	int i;

	//Get information about the current state of processes.
	if (syscall(__NR_my_syscall, &number_of_processes, NULL)) {
		printf("The syscall returned an error.\n");
		return -1;
	} else if(number_of_processes == 0) {
		printf("Could not count processes.\n");
		return -1;
	} else {
		printf("Counted %d processes.\n", number_of_processes);
	}

	//Allocate memory for the information. Fill array.
	info = malloc(number_of_processes * sizeof(struct CustomProcInfo));
	printf("total size=%d \n", number_of_processes * sizeof(struct CustomProcInfo));
	if (syscall(__NR_my_syscall, &number_of_processes, info)) {
		printf("The syscall returned an error.\n");
		return -1;
	}

	//Get information from information.
	
	//Iterate through array, and print out values.
	printf("PID TTY          TIME CMD\n");
	for(i = 0; i < number_of_processes; i++) {
		printf("%d\t%s\n",
				info[i].pid),
				info[i].command;
	}
	free(info);
	return 0;
}
