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

struct proc_dir_entry *proc_get_root(void)
{
    return proc_root;
}

char *proc_get_name(struct proc_dir_entry *node)
{
    return *get_member_ptr(node, name_offset, char *);
}

struct proc_ops *proc_get_ops(struct proc_dir_entry *node)
{
    return *get_member_ptr(node, proc_ops_offset, struct proc_ops *);
}

int cmp(char *x, char *y)
{
    if (strlen(x) != strlen(y))
        return (int) strlen(x) - (int) strlen(y);
    return strcmp(x, y);
}


struct proc_dir_entry *proc_find_child(struct proc_dir_entry *parent, char *name)
{
    if (!parent || !name)
        return NULL;

    struct rb_node *node =
        get_member_ptr(parent, subdir_offset, struct rb_root)->rb_node;
    while (node) {
        int k = cmp(name, *get_member_ptr(
                              node, -subdir_node_offset + name_offset, char *));
        printk("%s\n", *get_member_ptr(node, -subdir_node_offset + name_offset,
                                       char *));
        if (k < 0)
            node = node->rb_left;
        else if (k > 0)
            node = node->rb_right;
        else
            return get_member_ptr(node, -subdir_node_offset,
                                  struct proc_dir_entry);
    }

    return NULL;
}

static char buf[PATH_MAX];

struct proc_dir_entry *proc_find_by_path(char *path)
{
    if (strncmp(path, "/proc", 5) || strlen(path) > PATH_MAX)
        return NULL;

    strncpy(buf, path, PATH_MAX);
    char *token;
    char *pos = buf + 6;
    struct proc_dir_entry *now = proc_root;
    while ((token = strsep(&pos, "/")) != NULL) {
        printk("path: %s\n", token);
        now = proc_find_child(now, token);
        if (!now)
            break;
    }

    return now;
}
