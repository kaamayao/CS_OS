/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

//used for the functions: module_exit(), module_init(), MODULE_LICENCE(), MODULE_DESCRIPTION, MODULE_AUTHOR()
//library of functions to dynamically load modules to the linux kernel
#include <linux/module.h>
//Defines GOLDEN_RATIO_PRIME constant, library of hashable values predetermined by the current architecture
#include <linux/hash.h>
//Defines the gcd function
#include <linux/gcd.h>

/* This function is called when the module is loaded. */
int simple_init(void)
{
       //prints "Loading Module" to the kernel log messages
       printk(KERN_INFO "Loading Module! \n");
       //prints Golden ratio value to the kernel log messages
       printk(KERN_INFO "GOLDEN RATIO PRIME: %llu \n", GOLDEN_RATIO_PRIME);
       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
       // Prints "Removing Module" to the Kernel log messages
	printk(KERN_INFO "Removing Module\n");
       // Prints GDC value between 3300 and 24 to the Kernel log messages
       printk(KERN_INFO "GDC(3300,24): %lu\n", gcd(3300, 24));
}

/* Macros for registering module entry and exit points. */

//Defines the function that's going to be called at the module insertion time 
//(since we are compiling It as module) or boot time. Returns negative int if fails
module_init( simple_init );
//Defines the function that's going to be called at the module removal time 
//(since we are compiling It as module). Gets called when module usage count reaches. 
//This function can sleep but never fails
//optional function, if not present you can only remove module by using "rmmmod -f"
module_exit( simple_exit );

//Warns the codebase about the status of the code's license, in this case is a GPL license
//hence the code gets flagged as open source
MODULE_LICENSE("GPL");
//Describe what the module does, in this case it only says the name of the file since this module 
//only prints kernel logs
MODULE_DESCRIPTION("Simple Module");
//Declares the module's author, in this case this code is created by SGG
MODULE_AUTHOR("SGG");
