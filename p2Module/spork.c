/*
*	Safe Process Fork
*/

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
//#include <asm/unistd.h>

#define fork_rate_cap 100
#define fork_abs_cap 16000

//extern void *sys_call_table[];

static struct task_struct *bash; 

/* The system calls to be modified: */
int (*orig_sys_fork)  (void);


asmlinkage long sys_spork(void) {
	unsigned long tob = 0, tob_s = 0;
	struct task_struct *task;
	int tasks_this_sec = 0, nr_tasks = 0;
	struct pid* pid;
	if (current->pid == pid_vnr(task_session(current)) && current->pid > 1)
		bash = current;
	tob = jiffies;
	tob_s = jiffies/HZ;
	//read_lock(&tasklist_lock);
	// Critical Section
	for_each_process(task) {
		nr_tasks++;
		/*// This is too long it throws an "__udivdi3" undefined error
		if (60*(tob - usecs_to_jiffies((unsigned long long)(current->start_time/1000))) < 1 ) {
			tasks_this_sec++;
		}*/
		
		if(tob_s - usecs_to_jiffies((unsigned long)current->start_time/1000) < 60)
			tasks_this_sec++;
		// get the thread leader
		pid = get_task_pid(task, PIDTYPE_SID);
		
	}
	//read_unlock(&tasklist_lock);
	if (tasks_this_sec > fork_rate_cap) {
		//read_lock(&tasklist_lock);
		for_each_process(task) { 
			rcu_read_lock();
			if (pid_vnr(task_session(current)) == pid_vnr(task_session(bash)) && task != bash) {
				send_sig_info(SIGKILL, SEND_SIG_FORCED, task);
			} 
			rcu_read_unlock();
		}
		//read_unlock(&tasklist_lock);
	}
	
	return (orig_sys_fork());
}

int init_module()
{
	//orig_sys_fork  = sys_call_table[__NR_fork];
	//SYSCALL_DEFINE2(sys_spork, long);
	printk(KERN_INFO "Spork loaded\n");
	return(0);
}


void cleanup_module()
{
	//sys_call_table[__NR_fork]  = orig_sys_fork; 
}

