#include <linux/list.h>
#include <linux/ftrace.h>

struct hook {
    unsigned long org_func;
    unsigned long evil_func;
    struct list_head list;
    struct ftrace_ops ops;
};

/**
 * Start hooking
 *
 * @org_func: The function address to be hooked
 * @evil_func: The evil function
 */
void hook_start(unsigned long org_func, unsigned long evil_func);

/**
 * Release all the hook
 */
void hook_release(void);