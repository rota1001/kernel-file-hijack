# kernel file hijack
This is a linux kernel rookit used to hijack file operations or process file operations with path, which is inspired by [rooty](https://github.com/jermeyyy/rooty).
In rooty, it gets the file with `filp_open` and hooks the file operations in it. However, in newer linux versions, some operations such as `proc_ops` cannot be obtained this way.

This rookit gets the file operations with the `proc_dir_entry` structure for procfs, and can obtain the internal `proc_ops` which was used while register.

## Claim
This rootkit is for **educational purpose**, don't use it to attack any target illegally.

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
