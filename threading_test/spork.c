/*
 *  spork.c
 *  Test threading and semaphores.
 *  Author: Daniel D'Souza
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include <linux/semaphore.h>
#include <linux/delay.h>

struct task_struct *monitor_task, *kill_task;
int monitor_function(void *data);
int kill_function(void *data);

int monitor_function(void *data) {
    printk(KERN_INFO "MONITORING THREAD STARTED");
    
    // do not stop this thread until the program quits.
    while(!kthread_should_stop()) {
	// run basic detection task here (the count).
	// if some condition
        //     kill_task = kthread_create(&kill_function, (void *)data, "kill_task");
        //     acutally just call the method, this should be easier...
        
        // this will put the process to sleep, and keep it that way...
        // set_current_state(TASK_INTERRUPTIBLE);
        // schedule();

        msleep(10); //sleep for a 10th of a second.
    }
    return 0;
}

int kill_function(void *data) {
    printk(KERN_INFO "KILLING THREAD STARTED");

    // add detection and kill code here.
    
    return 0;
}

// Get the party started
int init_module(void) {
    int data;
    data = 20;
    printk(KERN_INFO "Loading SPORK");
    monitor_task = kthread_create(&monitor_function, (void *)data, "fork_watchdog");
    printk(KERN_INFO "Monitoring Task: %s",monitor_task->comm);
    return(0);
}

// Cleanup the module.
void cleanup_module(void) {
    kthread_stop(monitor_task);
}

MODULE_AUTHOR("Daniel D'Souza");
MODULE_LICENSE("GPL");
