#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/jiffies.h>
#include <linux/rcupdate.h>
#include <asm/uaccess.h>
#include <generated/uapi/asm/unistd_32.h>
// Might not need
#define __NR_get_proc_custom 361
#define MAX_NUM_OF_PROG 350	// NEED TO DETERMINE (500 is my guess)
#define MAX_NUM_NEW_PRO 5	// NEED TO DETERMINE (5 is my guess)


int wait_jiffies_from_start(int jif1, int time_to_wait){
	int jif2 = jiffies;
	while(jif2-jif1 < time_to_wait) jif2 = jiffies;
	return jif2;
}

int print(void){
	// Varriable decleration
	struct task_struct *task;
	int nr_tasks1 = 0, nr_tasks2 = 0, nr_tasks3 = 0;
	unsigned long jif1 = 0, jif2 = 0;
	
	// Start jiffies
	jif1 = jiffies;
	// Iterates through each task and counts them
	for_each_process(task) nr_tasks1++;

	// Waits HZ jiffies (should be 1 second)
	jif2 = wait_jiffies_from_start(jif1,HZ);
	
	// Iterates through each task and counts them (more tasks than last time?)
	for_each_process(task) nr_tasks2++;
	
	// If either of the arbitrary maxes are met (print and kill the new tasks)
	if(nr_tasks2 > MAX_NUM_OF_PROG || nr_tasks2-nr_tasks1 > MAX_NUM_NEW_PRO){
		for_each_process(task){
			if(++nr_tasks3 > nr_tasks1){
			// Currently kills all tasks that were created since program start
				printk("Task name is %s\n", task->comm);
				send_sig_info(SIGKILL, SEND_SIG_FORCED, task);
			}
		}
	}
	//printk("Task name is %s\nPid is %d\n", task->comm, task->pid);
	//printk("There are %d tasks\n",nr_tasks1);
	//printk("There are %d tasks\n", nr_tasks2);
	//printk("There have been %lu jiffies.\n", jif2-jif1);
	
	return 0;
}


int print_init(void){
	print();
	return 0;
}
void print_exit(void){
	
}


module_init(print_init);
module_exit(print_exit);
