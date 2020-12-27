obj-m += states_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	-sudo rmmod states_module
	sudo dmesg -C
	sudo insmod states_module.ko
	dmesg