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
