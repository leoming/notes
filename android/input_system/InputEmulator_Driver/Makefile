obj-m	:= InputEmulator-yf.o
InputEmulator-yf-objs := InputEmulator.o

KID := ~/works/Lollipop/kernel
PWD := $(shell pwd)
ARCH = arm
CROSS_COMPILE=arm-eabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
all:
	make -C $(KID) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=${PWD} modules
clean:
	rm -rf *.o *.ko *.mod.c .tmp_verisons
