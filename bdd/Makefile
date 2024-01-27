TARGET := czblk
PWD := $(shell pwd)
KERNEL := $(shell uname -r)

obj-m := $(TARGET).o 
$(TARGET)-objs += block_drv.o

all:
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) modules -I ./main.h

clean:
	cp ./compile_commands.json ./json/
	make -C /lib/modules/$(KERNEL)/build M=$(PWD) clean
	cp ./json/compile_commands.json ./
	rm -f ../udd/Module.symvers
