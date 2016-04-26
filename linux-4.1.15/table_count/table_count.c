/*
*	System Call
*	table_call.c
*/

#include <linux/kernel.h>
#include <linux/syscalls.h>
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

// sources:
// https://www.kernel.org/doc/gorman/html/understand/understand006.html
// http://stackoverflow.com/questions/8980193/walking-page-tables-of-a-process-in-linux

/**
 * This funtion takes in a pid and a "print_bool".
 * If print_bool == -1 then the function will not print any thing.
 * The function finds each of the tables associated with a pid and counts how many
 * have been accessed recently, then it sets the "accessed" bit to 0.
*/

asmlinkage long sys_table_count(int pid, int print_bool){
	
	//printk("This\nis\nthe\nsystem\noutput\n."); // This printed for me
	int count;
	int page_found;
	unsigned long start;
	unsigned long end;
	unsigned long addr;
	struct task_struct *task;
	struct vm_area_struct *vma;
	
	count = 0;
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
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
			struct task_struct *task;
			struct page *page = NULL;
			task = pid_task(find_vpid(pid), PIDTYPE_PID);
			addr = start;
			
			page_found = 0;
			
			pgd = pgd_offset(task->mm, addr);
			if (pgd_none_or_clear_bad(pgd)) page_found = -1;
				/* search in swap
				if(print_bool != -1)printk("Break at PGD. Address is not available.\n");
				return -1;*/
			pud = pud_offset(pgd, addr);
			if (page_found != -1 && pud_none_or_clear_bad(pud)) page_found = -1;
				/* search in swap
				if(print_bool != -1)printk("Break at PUD. Address is not available.\n");
				return -1;*/
			pmd = pmd_offset(pud, addr);
			if (page_found != -1 && pmd_none_or_clear_bad(pmd)) page_found = -1;
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
				if(print_bool != -1)printk("Page frame is in swap.\n");
				pte_unmap(ptep);
				page_found = 0;
			}
			page = pte_page(pte);
			if (page_found != -1 && page) {
				page_found = 1;
				pte_unmap(ptep);
			}
			if(page_found != 1){
				if(print_bool != -1)printk("Page not found\n");
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
	
	if(print_bool != -1)printk("%d: %d",pid,count);
	return count;
}
