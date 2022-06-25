#include <linux/sched/signal.h>
#include <linux/module.h>

int entryPoint(void) {
	struct task_struct *task; 
	printk(KERN_INFO "Inserting ListTasksLinear\n");	
	printk(KERN_INFO "Started listing tasks\n");
	for_each_process(task) { 
		printk(KERN_INFO "Name: %-20s State: %ld\tProcess ID: %d\n", task->comm, task->state, task->pid);
	}
	
	printk(KERN_INFO "Stopped listing tasks\n");
	return 0;
}

void exitPoint(void) {
	printk(KERN_INFO "Removing ListTasksLinear\n");
}

module_init(entryPoint);
module_exit(exitPoint);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("List tasks linearly");
MODULE_AUTHOR("Youssef Hassan, 900132871");
