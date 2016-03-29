/*
*	Rolando S. Garcia
*	StudentID: 1202559235
*	Lecture: MWF 12:00 P.M. - 12:50 P.M.
*	
*	System Call
*	get_addr.c
*/

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/pid.h>
#include <linux/mm_types.h>
#include <linux/highmem.h>
#include <asm/page.h>
#include <asm/pgtable.h>

// sources:
// https://www.kernel.org/doc/gorman/html/understand/understand006.html
// http://stackoverflow.com/questions/8980193/walking-page-tables-of-a-process-in-linux

asmlinkage long sys_get_addr(int pid, unsigned long addr)
{
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *ptep, pte;
	pud_t *pud;
	struct task_struct *task;
	struct page *page = NULL;
	task = pid_task(find_vpid(pid), PIDTYPE_PID); 
	// task->mm->pgd is pointer to page global directory, which is an array of pointers to page middle directory pmd
	pgd = pgd_offset(task->mm, addr);
	if (pgd_none_or_clear_bad(pgd)) {
		// search in swap
		printk(KERN_INFO "Break at PGD. Address is not available.");
		return -1;
	}
	pud = pud_offset(pgd, addr);
	if (pud_none_or_clear_bad(pud)) {
		// search in swap
		printk(KERN_INFO "Break at PUD. Address is not available.");
		return -1;
	}
	//page middle directory
	pmd = pmd_offset(pud, addr);
	if (pmd_none_or_clear_bad(pmd)) {
		// search in swap
		printk(KERN_INFO "Break at PMD. Address is not available.");
		return -1;
	}
	//page table entries pte
	ptep = pte_offset_map(pmd, addr);
	if (!ptep) {
		printk(KERN_INFO "Break at PTEP. Invalid page address 0.");
		return -1;
	}
	pte = *ptep;
	page = pte_page(pte);
	if (page) {
		printk(KERN_INFO "Page frame located at %p", page);
		pte_unmap(ptep); 
		return 0;
	}
	printk(KERN_INFO "PAGE is not available.");
	pte_unmap(ptep); 
	return -1;	
}
