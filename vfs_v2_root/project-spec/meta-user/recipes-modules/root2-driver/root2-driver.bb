SUMMARY = "Root driver for Paragon system"
DESCRIPTION = "Loadable kernel module for root fpga - includes VFS"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

INHIBIT_PACKAGE_STRIP = "1"

SRC_URI = "file://Makefile \
           file://bram/status/status_bram.c \
           file://bram/status/status_bram.h \
           file://bram/sender/send_bram.c \
           file://bram/sender/send_bram.h \
           file://bram/receiver/receive_bram.c \
           file://bram/receiver/receive_bram.h \
           file://vfs/tree.c \
           file://vfs/tree.h \
           file://aux/ip.c \
           file://aux/ip.h \
           file://fops/bram/bram_fops.c \
           file://fops/bram/bram_fops.h \
           file://fops/status/status_fops.c \
           file://fops/status/status_fops.h \
           file://fops/mult/mult_fops.c \
           file://fops/mult/mult_fops.h \
           file://fops/testing/test_fops.c \
           file://fops/testing/test_fops.h \
           file://hub64.c \
           file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.