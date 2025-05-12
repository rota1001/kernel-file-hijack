#include "procfs_utils.h"


int name_offset = 0;
int parent_offset = 0;
int subdir_offset = 0;
int subdir_node_offset = 0;
int proc_ops_offset = 0;
struct proc_dir_entry *proc_root = 0;

ssize_t dummy_proc_read(struct file *file,
                        char __user *buf,
                        size_t,
                        loff_t *off)
{
    return 0;
}

void proc_find_init(void)
{
    static struct proc_dir_entry *parent, *child, *grandchild;
    parent = proc_mkdir("parent", NULL);

    if (!parent)
        goto DONE;

    child = proc_mkdir("child", parent);
    if (!child)
        goto REMOVE;

    grandchild = proc_create("grandchild", 0644, child,
                             &(struct proc_ops){.proc_read = dummy_proc_read});

    if (!grandchild)
        goto REMOVE;

    unsigned long *parent_arr = (unsigned long *) parent;
    unsigned long *child_arr = (unsigned long *) child;
    unsigned long *grandchild_arr = (unsigned long *) grandchild;

    // find `char *name` offset
    for (int i = 0; i < 100; i++) {
        char buf[7];
        if (copy_from_kernel_nofault(buf, (const void *) parent_arr[i], 6) < 0)
            continue;
        buf[6] = 0;
        if (!strcmp(buf, "parent")) {
            name_offset = i * sizeof(unsigned long);
            break;
        }
    }
    printk("name_offset: 0x%x\n", name_offset);

    // find `struct proc_dir_entry *parent` offset
    for (int i = 0; i < 100; i++)
        if (child_arr[i] == (unsigned long) parent) {
            parent_offset = i * sizeof(unsigned long);
            break;
        }
    printk("parent_offset: 0x%x\n", parent_offset);


    // find `struct rb_root subdir`
    for (int i = 0; i < 100; i++)
        if (parent_arr[i] - (unsigned long) child ==
            child_arr[i] - (unsigned long) grandchild) {
            subdir_offset = i * sizeof(unsigned long);
            break;
        }
    printk("subdir_offset: 0x%x\n", subdir_offset);

    // find `struct rb_node subdir_node`
    subdir_node_offset = *get_member_ptr(parent, subdir_offset, unsigned long) -
                         (unsigned long) child;
    printk("subdir_node: 0x%x\n", subdir_node_offset);

    // find `struct proc_ops *proc_ops`
    for (int i = 0; i < 100; i++) {
        unsigned long addr;
        if (copy_from_kernel_nofault(
                &addr,
                (const void *) (grandchild_arr[i] +
                                offsetof(struct proc_ops, proc_read)),
                sizeof(addr)) < 0)
            continue;

        if (addr == (unsigned long) dummy_proc_read) {
            proc_ops_offset = i * sizeof(unsigned long);
            break;
        }
    }
    printk("proc_ops_offset: 0x%x\n", proc_ops_offset);

    // find root
    proc_root = *get_member_ptr(parent, parent_offset, struct proc_dir_entry *);

REMOVE:
    proc_remove(parent);
DONE:
}

struct proc_dir_entry *get_proc_root(void)
{
    return proc_root;
}

char *get_name(struct proc_dir_entry *node)
{
    return *get_member_ptr(node, name_offset, char *);
}

struct proc_ops *get_proc_ops(struct proc_dir_entry *node)
{
    return *get_member_ptr(node, proc_ops_offset, struct proc_ops *);
}

