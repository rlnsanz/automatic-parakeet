/*
*	System Call
*	table_call.c
*/

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/pid.h>
#include <linux/mm_types.h>
#include <linux/highmem.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <asm/page.h>
#include <asm/pgtable.h>

// Module unique code
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/unistd.h>
#include <linux/pid_namespace.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/sys.h>
#include <linux/sysinfo.h>

#define SLEEP_DURATION 1000 // sleep duration in ms
//#define __NR_table_count 360

int monitor_function(void *data);
int sys_table_count(struct task_struct *task, int print_bool);
struct task_struct *monitor_task;
static struct task_struct *task = NULL;

int monitor_function(void *data) {
    unsigned int count, countKB;
    // do not stop this thread until the program quits.
    printk("monitor_function ran. This log at line 42.\n");
    //while(!kthread_should_stop()) {
        //syscall(__NR_table_count,(int) data ,1);
    printk("Entered while loop. This log at line 45.\n");
	count = 0;
	for_each_process(task){
        count += sys_table_count(task, 1);
        if (count >= 2)
	    	count -= 2;
	    msleep(SLEEP_DURATION); //sleep for a 10th of a second.
	}
	countKB = count*4;
	printk("TWSS: %d\n",countKB);
        //printk("iteration\n");
    //}
    return 0;
}

MODULE_AUTHOR("James and Daniel");
MODULE_LICENSE("GPL");

// Get the party started
int init_module(void) {
    int data;
    data = 20;
    printk(KERN_INFO "Loading SPORK\n");
    monitor_task = kthread_create(&monitor_function, (void *)data, "fork_watchdog");
    if (monitor_task == NULL) {
    	printk("Attempted to dereference monitor_task line 68");
    }
    else {
    	printk("valid monitor_task");
    }
    if ((monitor_task)) {
    	printk(KERN_INFO "Created monitoring Task: %s\n",monitor_task->comm);
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

// FROM SYSCALL MADE earlier


// sources:
// https://www.kernel.org/doc/gorman/html/understand/understand006.html
// http://stackoverflow.com/questions/8980193/walking-page-tables-of-a-process-in-linux

/**
 * This funtion takes in a pid and a "print_bool".
 * If print_bool == -1 then the function will not print any thing.
 * The function finds each of the tables associated with a pid and counts how many
 * have been accessed recently, then it sets the "accessed" bit to 0.
*/

int sys_table_count(struct task_struct *task, int print_bool) {
	
	//printk("This\nis\nthe\nsystem\noutput\n."); // This printed for me

	unsigned int count;
	int page_found;
	unsigned long long start;
	unsigned long long end;
	unsigned long long addr;
	struct vm_area_struct *vma;
	
	printk("Entered sys_table_count. This log at line 106.\n");

	if (task == NULL) {
		return 0;
	}

	count = 0;
	if (task->mm == NULL) {
		return 0;
	}
	vma = task->mm->mmap;
	
	while(vma != NULL){
		//if(print_bool != -1)printk("We are testing on the %d set.\n",count);
		start = vma->vm_start;
		end = vma->vm_end;
		while(start < end){
			// START OF "get_pte"
			pgd_t *pgd;
			pmd_t *pmd;
			pte_t *ptep, pte;
			pud_t *pud;
			struct page *page = NULL;
			addr = start;
			
			page_found = 0;
			
			pgd = pgd_offset(task->mm, addr);
			if (pgd_none_or_clear_bad(pgd)) page_found = -1;
				/* search in swap
				if(print_bool != -1)printk("Break at PGD. Address is not available.\n");
				return -1;*/
			pud = pud_offset(pgd, addr);
			//if (page_found != -1 && pud_none_or_clear_bad(pud)) page_found = -1;
				/* search in swap
				if(print_bool != -1)printk("Break at PUD. Address is not available.\n");
				return -1;*/
			pmd = pmd_offset(pud, addr);
			//if (page_found != -1 && pmd_none_or_clear_bad(pmd)) page_found = -1;
				/* search in swap
				if(print_bool != -1)printk("Break at PMD. Address is not available.\n");
				return -1;*/
			ptep = pte_offset_map(pmd, addr);
			if (page_found != -1 && !ptep) page_found = -1;
				/*
				if(print_bool != -1)printk("Break at PTEP. Invalid page address 0.\n");
				return -1;*/
			
			
			pte = *ptep;
			if (page_found != -1 && is_swap_pte(pte)) {
				if(print_bool != -1) printk("Page frame is in swap.\n");
				pte_unmap(ptep);
				page_found = 0;
			}
			page = pte_page(pte);
			if (page_found != -1 && page) {
				page_found = 1;
				pte_unmap(ptep);
			}
			if(page_found != 1){
				if(print_bool != -1) printk("Page not found\n");
			}
			// END OF "get_pte"
			
			if(page_found == 1 && pte_young(pte)){
				count = count+1;
				pte_mkold(pte);
			}
			
			
			start = start + 4096;
		}
		vma = vma->vm_next;
	}
	
	if (print_bool != -1) printk("WSS: %d\n", count);
	return count;
}