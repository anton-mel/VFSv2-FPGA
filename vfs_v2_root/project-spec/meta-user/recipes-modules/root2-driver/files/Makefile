obj-m += root2-driver.o
root2-driver-objs := \
	hub64.o \
	bram/status/status_bram.o \
	bram/sender/send_bram.o \
	bram/receiver/receive_bram.o \
	vfs/tree.o \
	aux/ip.o \
	fops/bram/bram_fops.o \
	fops/status/status_fops.o \
	fops/mult/mult_fops.o \
	fops/testing/test_fops.o

MY_CFLAGS += -g -DDEBUG
ccflags-y += ${MY_CFLAGS}

SRC := $(shell pwd)
ccflags-y += -I$(SRC)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
