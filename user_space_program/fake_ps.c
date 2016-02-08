/*
 *	This program will test the my_syscall syscall
 *	Author: Daniel D'Souza (with help of course)
 */

#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define __NR_get_proc_custom 360

struct CustomProcInfo {
	int pid;
	char tty[64];
	unsigned long long int time;
	char command[16];
};

int main() {
	struct CustomProcInfo *info;
	int number_of_processes = 0;
	int i;
	unsigned long long int hours, minutes, seconds; //TODO: make these smaller, and fix assignment respective assigment logic?

	//Get information about the current state of processes.
	if (syscall(__NR_get_proc_custom, &number_of_processes, NULL)) {
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
	if (syscall(__NR_get_proc_custom, &number_of_processes, info)) {
		printf("The syscall returned an error.\n");
		return -1;
	}

	//Iterate through array, and print out values.
	printf("PID TTY          TIME CMD\n");
	for(i = 0; i < number_of_processes; i++) {
		hours = info[i].time / 360000;
		info[i].time -= hours * 360000;
		minutes = info[i].time / 6000;
		info[i].time -= minutes * 6000;
		seconds = info[i].time / 100;

		printf("%d\t%s\t%02llu:%02llu:%02llu\t%s\n",
				info[i].pid,
				info[i].tty,
				hours,
				minutes,
				seconds,
				info[i].command);
	}
	free(info);
	return 0;
}
