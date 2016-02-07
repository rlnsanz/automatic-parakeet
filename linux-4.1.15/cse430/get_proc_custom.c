/*
 *	get_proc_custom.c
 *	author: Daniel D'Souza
 *
 *	this file should be placed along with get_proc_custom.h and its makefile in the <kerneldir>/cse340 folder.
 *	DISCLAIMER: The use of absolutes instead of the predef macros is bad.
 */
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <asm/uaccess.h>
// #include <../cse430/get_proc_custom.h>

struct CustomProcInfo {
	int pid;
	char tty[64];
	unsigned long long int time; //cputime_t is an unsigned 64bit integer.
	//This should really be using the macro, but this is easier.
	char command[16];
};

asmlinkage long sys_get_proc_custom(int __user *proc_count, const struct CustomProcInfo __user *proc_array) {
	//declare variables up here to complie with c90
	int last_pid = 0;
	int i = 0;
	struct task_struct *task;
	struct CustomProcInfo *info;
	char null_string[] = "?";

	printk(KERN_INFO "DEBUG: Starting test.\n");
	
	//Case 1: Get the number of processes, so that we can malloc the required space in the user program.
	if (proc_array == NULL) {
		printk(KERN_INFO "DEBUG: Counting tasks...\n");
		for_each_process(task) {
			last_pid++;
		}
		if (copy_to_user(proc_count, &last_pid, sizeof(int))) {
			return -EFAULT;
		}
		printk(KERN_INFO "DEBUG: FOUND %d tasks... \n", last_pid);
	} 
	//Case 2: copy the relavant information into a temporary array, and then copy the information to userspace.
	else {
		//get the number of pids to get.
		if (copy_from_user(&last_pid, proc_count, sizeof(int))) {
			return -EFAULT;
		}

		//create an array, and fill it.
		printk(KERN_INFO "DEBUG: size is %d \n", last_pid);
		info = kmalloc(last_pid * sizeof(struct CustomProcInfo), GFP_KERNEL);
		for_each_process(task) {
			printk(KERN_INFO "DEBUG: ID is %d, i=%d \n", task->pid, i);
			info[i].pid = task->pid; //assign the pid as int into the user array.

			printk(KERN_INFO "DEBUG: TIME is %llu \n", task->utime + task->stime);
			info[i].time = task->utime + task->stime; //assign the time as u64 into the user array.
		
			printk(KERN_INFO "DEBUG: ORIG: %s : %d\n" , task->comm, sizeof(task->comm));
			strncpy(info[i].command, task->comm, sizeof(task->comm)); //copy the the command name into the user array.
			info[i].command[15] = '\0'; //adding the terminator may not be necessary, but is a good idea anyway.
			printk(KERN_INFO "DEBUG: COPY: %s : %d\n" , info[i].command, sizeof(info[i].command));
			
			if (task->signal->tty == NULL || task->signal->tty->name == NULL) {
				printk(KERN_INFO "DEBUG: TTY: ?\n");
				strncpy(info[i].tty, null_string, sizeof(info[i].tty)); //copy the tty into the user array. BAD DANIEL, not using size macros...
			} else {
				printk(KERN_INFO "DEBUG: TTY: %s\n", task->signal->tty->name);
				strncpy(info[i].tty, task->signal->tty->name, sizeof(info[i].tty)); //copy the tty into the user array. BAD DANIEL, not using size macros...
			}
			info[i].tty[63] = '\0';
			printk(KERN_INFO "DEBUG: TTYout: %s\n", info[i].tty);

			i++;	
		}

		//use to check the total size. Remove when finished.
		printk("DEBUG: total size= %d \n", last_pid * sizeof(struct CustomProcInfo));

		//copy the populated user array to the user
		if (copy_to_user(proc_array, info, last_pid * sizeof(struct CustomProcInfo))) {
			return -EFAULT;
		}

		//free the memory allocated to the user array
		kfree(info);
	}
	return 0;
}
