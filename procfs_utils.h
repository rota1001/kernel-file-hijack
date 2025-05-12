#include <linux/limits.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

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
struct proc_dir_entry *proc_get_root(void);

/**
 * Get the name of the node
 */
char *proc_get_name(struct proc_dir_entry *node);

/**
 * Get the proc_ops of the node
 */
struct proc_ops *proc_get_ops(struct proc_dir_entry *node);

/**
 * Find the child with the specific name
 * @parent: the proc_dir_entry structure of parent
 * @s: string would be inserted
 *
 * Return: return a pointer to the child structure of NULL if not found
 */
struct proc_dir_entry *proc_find_child(struct proc_dir_entry *parent, char *name);

/**
 * Find the `proc_dir_entry` by path
 */
struct proc_dir_entry *proc_find_by_path(char *path);
