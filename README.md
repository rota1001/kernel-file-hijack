# kernel file hijack
This is a linux kernel rookit used to hijack file operations or process file operations with path, which is inspired by [rooty](https://github.com/jermeyyy/rooty).
In rooty, it gets the file with `filp_open` and hooks the file operations in it. However, in newer linux versions, some operations such as `proc_ops` cannot be obtained this way.

This rookit gets the file operations with the `proc_dir_entry` structure for procfs, and can obtain the internal `proc_ops` which was used while register.

## Usage
