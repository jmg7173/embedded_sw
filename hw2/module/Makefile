#Makefile for a basic kernel module

obj-m   := dev_driver.o
dev_driver-objs := io_driver.o device_driver.o 

KDIR    :=/work/achroimx_kernel
PWD     :=$(shell pwd)

all: driver
#all: driver 

driver:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

install:
	cp -a fpga_dot_driver.ko /nfsroot
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf *.o
	rm -rf Module.symvers
	rm -rf modules.order
	rm -rf .tmp*
	rm -rf .fpga*
	rm -rf .*.cmd
