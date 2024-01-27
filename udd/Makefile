TARGET := czusb
PWD := $(shell pwd)
KERNEL := $(shell uname -r)
obj-m := $(TARGET).o 
KBUILD_EXTRA_SYMBOLS := /home/rm/work/bdd/Module.symvers

all:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) modules 

clean:
	cp ./compile_commands.json ./json/
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) clean
	cp ./json/compile_commands.json ./
	rm -f ../udd/Module.symvers
