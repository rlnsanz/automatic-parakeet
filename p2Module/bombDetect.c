#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pid_namespace.h>

// MACROS (arbitrary numbers)
#define MAX_NUM_NEW_PRO 5	// NEED TO DETERMINE (5 is my guess)

/**
 * returns new jiffies value
 * Given start time "jif1" and number of jiffies to wait "time_to_wait" this function waits
 * until the new number of jiffies (current time) "jif2" equals or exceeds time_to_wait.
*/
int wait_jiffies_from_start(unsigned long jif1, int time_to_wait){
	int jif2 = jiffies;
	while(jif2-jif1 < time_to_wait) jif2 = jiffies;
	return jif2;
}

/**
 * This function counts the number of processes twice
 * Compares the second count "nr_tasks2" to an arbitrary "MAX_NUM_OF_PROG".
 * 	If the second count is greater than the max allowable number of programs it 
 *	has determined there is a fork bomb.
 * It also compares if the number of programs created between "jif1" and "jif2" to an arbitray
 *	"MAX_NUM_NEW_PRO". If the difference between "nr_tasks2" and "nr_tasks1" exceeds this
 *	arbitrary value, then the program determines there is a fork bomb.
*/
int detectBomb(void){
	// Varriable decleration
	struct task_struct *task;
	int nr_tasks1 = 0, nr_tasks2 = 0, nr_tasks3 = 0;
	unsigned long jif1 = 0, jif2 = 0;
	
	// Start jiffies
	jif1 = jiffies;
	// Iterates through each task and counts them
	for_each_process(task) nr_tasks1++;

	// Waits HZ jiffies (should be 1 second)
	jif2 = wait_jiffies_from_start(jif1,10);
	
	// Iterates through each task and counts them (more tasks than last time?)
	for_each_process(task) nr_tasks2++;
	
	// If either of the arbitrary maxes are met (print and kill the new tasks)
	if(nr_tasks2-nr_tasks1 > MAX_NUM_NEW_PRO){
		for_each_process(task){
			if(++nr_tasks3 > nr_tasks1){
				printk("%d\n",task->pid);
				send_sig_info(SIGKILL, SEND_SIG_FORCED, task);
			}
		}
		printk("Tasks per 10 jiffies = %d\n", nr_tasks2-nr_tasks1);
	}
	
	return 0;
}


int detect_bomb_init(void){
	// Remove the i < j and just make it 1 to run forever
	int i = 0, j = 200;
	while(i < j){
		detectBomb();
		i++;
	}
	return 0;
}
void detect_bomb_exit(void){
	
}


module_init(detect_bomb_init);
module_exit(detect_bomb_exit);
