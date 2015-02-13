# $Revision: 9582 $ $Date: 2008-12-29 10:05:29 -0800 (Mon, 29 Dec 2008) $
# Copyright (c) Bullseye Testing Technology
# This source file contains confidential proprietary information.

# Linux kernel 2.4.x makefile for building BullseyeCoverage run-time.
# See http://www.bullseye.com/help/env_linuxKernel.html

ifeq ($(MAKING_MODULES),1)
	O_TARGET := libcov-lkm.o
else
	O_TARGET := libcov-linuxKernel.o
endif

obj-m := libcov-lkm.o
obj-y := libcov-linuxKernel.o

export-objs := libcov-linuxKernel.o libcov-lkm.o

include $(TOPDIR)/Rules.make
