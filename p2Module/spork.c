/*
 *	Safe Process Fork
 *
 *  spork.c
 *  Test threading and semaphores.
 *  Authors: Daniel D'Souza, Rolando Garcia, James Hutchins
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include <linux/pid.h>
#include <linux/pid_namespace.h>

#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/slab.h>

#define SLEEP_DURATION 1000 // sleep duration in ms
#define TASK_THRESHOLD 256 // number of tasks to count before killing any new tasks.

struct task_struct *monitor_task, *kill_task;
int monitor_function(void *data);
int kill_function(void *data);

int last_task_count = 0, current_task_count;

static struct bash_chain {
	int nr;
	struct task_struct *ptr_bash_i;
	struct bash_chain *next;
} bash, *last_bash;

static const char* command = "bash";

int monitor_function(void *data) {
    struct task_struct *task;
    int maxbashval = 0;
    struct task_struct *maxbash = NULL;
    struct bash_chain *bash_iterator = NULL;
    printk(KERN_INFO "MONITORING THREAD STARTED\n");

    // count current threads
    for_each_process(task){
    	// Always produces one link more than we need
    	if (strcmp(task->comm, command) == 0) {
    		printk(KERN_INFO "bash recognized");
			last_bash->ptr_bash_i = task;
			last_bash->next = (struct bash_chain*) kmalloc(sizeof(struct bash_chain), __GFP_WAIT);
			last_bash = last_bash->next;
			last_bash->nr = 0;
			last_bash->ptr_bash_i = NULL;
			last_bash->next = NULL;
    	}
        last_task_count++;
    }
    msleep(1000); // sleep for a second
    // do not stop this thread until the program quits.
    while(!kthread_should_stop()) {
        // count current number of tasks
        current_task_count = 0;
        for_each_process(task)
            current_task_count++;
        if (current_task_count - last_task_count > TASK_THRESHOLD || last_task_count > TASK_THRESHOLD) {
            printk(KERN_INFO "FORK BOMB DETECTED\n");
            for_each_process(task) { 
            	// Find the bash to which this task belongs
				bash_iterator = &bash;
		        while(bash_iterator->ptr_bash_i != NULL) {
		        	if (task_session(task) == task_session(bash_iterator->ptr_bash_i)) {
						bash_iterator->nr = bash_iterator->nr + 1;
						break;
					}
					else {
						bash_iterator = bash_iterator->next;
					}
		        }
		        if (bash_iterator->ptr_bash_i == NULL && strcmp(task->comm, command) == 0) {
        	    		printk(KERN_INFO "bash recognized\n");
						last_bash->ptr_bash_i = task;
						last_bash->next = (struct bash_chain*) kmalloc(sizeof(struct bash_chain), __GFP_WAIT);
						last_bash = last_bash->next;
						last_bash->nr = 0;
						last_bash->ptr_bash_i = NULL;
						last_bash->next = NULL;
		        }
			}
			bash_iterator = &bash;				
	        while(bash_iterator != NULL && bash_iterator->ptr_bash_i != NULL) {
	        	if (bash_iterator->nr > maxbashval) {
					maxbashval = bash_iterator->nr;
					maxbash = bash_iterator->ptr_bash_i;
				}
				bash_iterator = bash_iterator->next;
	        }
	        printk("MAXBASH PID: %d", maxbash->pid);
	        for_each_process(task) {
	        	rcu_read_lock();
				if (maxbash != NULL && task_session(task) == task_session(maxbash)) {
					send_sig_info(SIGKILL, SEND_SIG_FORCED, task);
				}
				rcu_read_unlock();	 
	        }
			printk(KERN_INFO "FORK BOMB DIFFUSED\n");
			current_task_count = 0;
        }
        last_task_count = current_task_count;
        msleep(1000); //sleep for a 10th of a second.
        //printk("iteration\n");
    }
    return 0;
}

MODULE_AUTHOR("Daniel D'Souza, Rolando Garcia, and James Hutchins");
MODULE_LICENSE("GPL");

// Get the party started
int init_module(void) {
    int data;
    bash.nr = 0;
    bash.next = NULL;
    last_bash = &bash;
    data = 20;
    printk(KERN_INFO "Loading SPORK\n");
    monitor_task = kthread_create(&monitor_function, (void *)data, "fork_watchdog");
    printk(KERN_INFO "Created monitoring Task: %s\n",monitor_task->comm);
    if ((monitor_task)) {
        printk(KERN_INFO "Waking up process\n");
        wake_up_process(monitor_task);
    }
    return(0);
}

// Cleanup the module.
void cleanup_module(void) {
	struct bash_chain *dyn = NULL;
    if (!kthread_stop(monitor_task)) {
        printk(KERN_INFO "MONITORING THREAD STOPPED\n");
        dyn = bash.next;
        while(dyn != NULL) {
        	struct bash_chain *garbage = dyn;
        	dyn = dyn->next;
        	kfree(garbage);
        }
    }
}


