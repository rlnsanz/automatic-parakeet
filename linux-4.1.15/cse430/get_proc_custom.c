/*
 *	get_proc_custom.c
 *	author: Daniel D'Souza
 */
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <asm/uaccess.h>
// #include <../cse430/get_proc_custom.h>

struct CustomProcInfo {
	int pid;
	//char tty[64];
	//long time;
	//This should really be using the macro, but this is easier.
	char command[16];
};

asmlinkage long sys_get_proc_custom(int __user *proc_count, const struct CustomProcInfo __user *proc_array) {
	//declare variables up here to complie with c90
	int last_pid = 0;
	int i = 0;
	struct task_struct *task;
	struct CustomProcInfo *info;

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
			info[i].pid = task->pid;
			strncpy(info[i].command, task->comm, 16);
			//info[i].command[16] = '\0';
			i++;	
		}
		printk("DEBUG: total size= %d \n", last_pid * sizeof(struct CustomProcInfo));
		if (copy_to_user(proc_array, info, last_pid * sizeof(struct CustomProcInfo))) {
			return -EFAULT;
		}
		kfree(info);
	}
	return 0;
}
