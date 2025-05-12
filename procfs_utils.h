#include <linux/proc_fs.h>

/**
 * Calculate offset of element in proc_dir_entry
 *
 * This deals with the condition that `__randomize_layout` is
 * enabled in the kernel config.
 */
void proc_find_init(void);

#define get_member_ptr(base, offset, type) \
    ((type *) (((char *) (base)) + (offset)))


/**
 * Get the `proc_dir_entry` of the root
 */
struct proc_dir_entry *get_proc_root(void);

/**
 * Get the name of the node
 */
char *get_name(struct proc_dir_entry *node);

/**
 * Get the proc_ops of the node
 */
struct proc_ops *get_proc_ops(struct proc_dir_entry *node)

