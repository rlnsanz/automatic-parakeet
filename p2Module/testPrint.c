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

static int print(void){
	// Varriable decleration
	struct task_struct *task;
	int nr_tasks = 0;
	/*
	  unsigned long tob = 0;
	  u64 tob2 = 0, diff_jiff = 0;
	  int tasks_this_min = 0;
	  struct pid* pid;
	*/
	
	// Gets current jiffies (This seems to work, but the task ones do not)
	// These jiffies go up by a couple thousand everytime I run the program.
	/*
	  tob = jiffies;
	  tob2 = get_jiffies_64();
	  printk("%lu\n",tob);
	  printk("%llu\n",tob2);
	*/

	// Iterates through each task
	for_each_process(task) {
		// Counts number of tasks
		nr_tasks++;
		
		// A sample (not all) of things I attempted to get a reliable outpur from task
		/*
		  diff_jiff = tob - task->start_time;
		  diff_jiff = tob2 - task->start_time;
		  printk("%llu\n",task->real_start_time-task->start_time);
		  printk("%llu\n",task->start_time);// Larger and smaller than both jiffies
		  printk("Current time = %llu\n",current->start_time);// Never changes
		  printk("Task time (j) %lu\n",usecs_to_jiffies(task->start_time));
		  printk("%llu = %llu seconds\n", diff_jiff, diff_jiff/HZ);
		*/

		// Prints pid of the tasks
		printk("Task name is %s\nPid is %d\n", task->comm, task->pid);
	}
	
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
