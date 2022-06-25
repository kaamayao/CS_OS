//used to import the function for_each_process
#include <linux/sched/signal.h>
//used for the functions: module_exit(), module_init(), MODULE_LICENCE(), MODULE_DESCRIPTION, MODULE_AUTHOR()
//library of functions to dynamically load modules to the linux kernel
#include <linux/module.h>

/* This function is called when the module is loaded. */
int entryPoint(void) {
	struct task_struct *task; //Pointer to the task whose info will be printed

	printk(KERN_INFO "Inserting ListTasksLinear\n");	
	
	printk(KERN_INFO "Started listing tasks\n");

	for_each_process(task) { //Loop over the tasks using the macro for_each_process

		/* on each iteration task points to the next task */
		//task->comm is the task' name
		//task->state is the task's state (-1 unrunnable, 0 runnable, >0 stopped)
		//task->pid is the task's process ID
		printk(KERN_INFO "Name: %-20s State: %ld\tProcess ID: %d\n", task->comm, task->state, task->pid);
	}
	
	printk(KERN_INFO "Stopped listing tasks\n");
	return 0;
}

/* This function is called when the module is removed. */
void exitPoint(void) {
	printk(KERN_INFO "Removing ListTasksLinear\n");
}

/* Macros for registering module entry and exit points. */
//Defines the function that's going to be called at the module insertion time 
//(since we are compiling It as module) or boot time. Returns negative int if fails
module_init(entryPoint);
//Defines the function that's going to be called at the module removal time 
//(since we are compiling It as module). Gets called when module usage count reaches. 
//This function can sleep but never fails
//optional function, if not present you can only remove module by using "rmmmod -f"
module_exit(exitPoint);

//Warns the codebase about the status of the code's license, in this case is a GPL license
//hence the code gets flagged as open source
MODULE_LICENSE("GPL");
//Describe what the module does, in this case it only says the name of the file since this module 
//only prints kernel logs
MODULE_DESCRIPTION("List tasks linearly");
//Declares the module's author, in this case this code is created by SGG
MODULE_AUTHOR("Youssef Hassan, 900132871");
