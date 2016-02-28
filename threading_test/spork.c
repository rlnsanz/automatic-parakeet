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
    printk(KERN_INFO "MONITORING THREAD STARTED\n");
    
    // do not stop this thread until the program quits.
    while(!kthread_should_stop()) {
	// run basic detection task here (the count).
	// if some condition
        //     kill_task = kthread_create(&kill_function, (void *)data, "kill_task");
        //     acutally just call the method, this should be easier...
        
        // this will put the process to sleep, and keep it that way...
        // set_current_state(TASK_INTERRUPTIBLE);
        // schedule_timeout(1000);

        msleep(1000); //sleep for a 10th of a second.
        printk("iteration\n");
    }
    return 0;
}

int kill_function(void *data) {
    printk(KERN_INFO "KILLING THREAD STARTED");

    // add detection and kill code here.
    
    return 0;
}

MODULE_AUTHOR("Daniel D'Souza");
MODULE_LICENSE("GPL");
// Get the party started
int init_module(void) {
    int data;
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
    if (!kthread_stop(monitor_task))
        printk(KERN_INFO "MONITORING THREAD STOPPED\n");
}


