#include "hook.h"
#include <linux/vmalloc.h>

LIST_HEAD(hook_list);


static void ftrace_func(unsigned ip,
                        unsigned long parent_ip,
                        struct ftrace_ops *ops,
                        struct ftrace_regs *fregs)
{
    if (within_module(parent_ip, THIS_MODULE))
        return;

    struct hook *hook_struct = container_of(ops, struct hook, ops);
    struct pt_regs *regs = ftrace_get_regs(fregs);
    regs->ip = hook_struct->evil_func;
}


void hook_start(unsigned long org_func, unsigned long evil_func)
{
    struct hook *new_hook = (struct hook *) vmalloc(sizeof(struct hook));
    if (!new_hook)
        return;
    new_hook->org_func = org_func;
    new_hook->evil_func = evil_func;
    new_hook->ops.func = (ftrace_func_t) ftrace_func;
    new_hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_RECURSION |
                          FTRACE_OPS_FL_IPMODIFY;
    if (ftrace_set_filter_ip(&new_hook->ops, new_hook->org_func, 0, 0))
        goto ERR1;

    if (register_ftrace_function(&new_hook->ops))
        goto ERR2;

    list_add(&new_hook->list, &hook_list);
    return;
ERR2:
    ftrace_set_filter_ip(&new_hook->ops, new_hook->org_func, 1, 0);
ERR1:
    vfree(new_hook);
}