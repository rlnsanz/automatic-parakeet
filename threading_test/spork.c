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

struct task_struct *monitor_task;
int monitor_function(void *data);
int kill_forks(void);

int monitor_function(void *data) {
    printk(KERN_INFO "IN THREAD FUNCTION");
    while(!kthread_should_stop()) {
        schedule();

    }
    return 0;
}

int kill_forks(void) {

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
