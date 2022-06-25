#include <linux/module.h>
#include <linux/hash.h>
#include <linux/gcd.h>

int simple_init(void)
{
       printk(KERN_INFO "Loading Module! \n");
       printk(KERN_INFO "GOLDEN RATIO PRIME: %llu \n", GOLDEN_RATIO_PRIME);
       return 0;
}

void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");
       printk(KERN_INFO "GDC(3300,24): %lu\n", gcd(3300, 24));
}

module_init( simple_init );
module_exit( simple_exit );
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
