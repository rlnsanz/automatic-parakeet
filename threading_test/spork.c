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

#include <linux/pid_namespace.h>

#include <linux/semaphore.h>
#include <linux/delay.h>

#define SLEEP_DURATION 100 // sleep duration in ms
#define TASK_THRESHOLD 10 // number of tasks to count before killing any new tasks.

struct task_struct *monitor_task, *kill_task;
int monitor_function(void *data);
int kill_function(void *data);

int last_task_count = 0, current_task_count;

int monitor_function(void *data) {
    struct task_struct *task;

    printk(KERN_INFO "MONITORING THREAD STARTED\n");
    
    // count current threads
    for_each_process(task)
        last_task_count++;
    msleep(100); // sleep for a second

    // do not stop this thread until the program quits.
    while(!kthread_should_stop()) {
        // count current number of tasks
        current_task_count = 0;
        for_each_process(task)
            current_task_count++;
        if (current_task_count - last_task_count > TASK_THRESHOLD) {
            kill_function((void *)last_task_count);
        }
        last_task_count = current_task_count;

        msleep(500); //sleep for a 10th of a second.
        //printk("iteration\n");
    }
    return 0;
}

int kill_function(void *data) {
    struct task_struct *task;
    int i = 0, delete_from = (int) data;
    
    printk(KERN_INFO "KILLING THREAD STARTED\n");

    for_each_process(task) {
        if (++i > delete_from) {
            printk(KERN_INFO "%d\n", task->pid);
            send_sig_info(SIGKILL, SEND_SIG_FORCED, task);
        }
    }
    
    return i-delete_from;
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


