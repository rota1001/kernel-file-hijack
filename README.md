# kernel file hijack
This is a linux kernel rootkit used to hijack file operations or process file operations with path, which is inspired by [rooty](https://github.com/jermeyyy/rooty).
In rooty, it gets the file with `filp_open` and hooks the file operations in it. However, in newer linux versions, some operations such as `proc_ops` cannot be obtained this way.

This rootkit gets the file operations with the `proc_dir_entry` structure for procfs, and can obtain the internal `proc_ops` which was used while register.

This rootkit has been tested on ubuntu 24.04, which has the kernel version of 6.11.0, and it is expected to work on even newer versions.

## Claim
This rootkit is for **Educational Purpose**, do not use it to attack any target illegally.

## Compilation
### Install Dependency
```
sudo apt install linux-headers-`uname -r`
```
### Compile
```
git clone git@github.com:rota1001/kernel-file-hijack.git
cd kernel-file-hijack
make
```
This gives you a kernel object file `rootkit.ko` and a usermode program `user`.

## Run it
If you want to insert it using `insmod`, you can run this command:
```
sudo insmod rootkit.ko
```
If you want to use user program to insert it, you can run this command:
```
sudo ./user
```
You can use `dmesg` to see the debug message printed in the examples:
```
sudo dmesg
```
You can try to cat `/proc/kallsyms`, the read function of which is hooked by us in the example. You can see the output `"yee"`:
```
cat /proc/kallsyms
# yee
```
## Unload
You can use `rmmod` to unload it:
```
sudo rmmod rootkit
```
