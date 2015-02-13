# $Revision: 10367 $ $Date: 2009-09-04 14:25:13 -0700 (Fri, 04 Sep 2009) $
# Copyright (c) Bullseye Testing Technology
# This source file contains confidential proprietary information.

CC=gcc -D_REENTRANT -fPIC -O3

build: ../lib/libcov.a ../lib/libcov-noThread.a ../lib/libcov32.a ../lib/libcov32-noThread.a

../lib/libcov.a: libcov.o
	ar -rc $@ $?
../lib/libcov-noThread.a: libcov-noThread.o
	ar -rc $@ $?
../lib/libcov32.a: libcov32.o
	ar -rc $@ $?
../lib/libcov32-noThread.a: libcov32-noThread.o
	ar -rc $@ $?

libcov.o: libcov-posix.c
	$(CC) -c -o $@ libcov-posix.c
libcov-noThread.o: libcov-posix.c
	$(CC) -c -o $@ -DLibcov_noAutoSave libcov-posix.c
libcov32.o: libcov-posix.c
	$(CC) -c -o $@ -m32 libcov-posix.c
libcov32-noThread.o: libcov-posix.c
	$(CC) -c -o $@ -m32 -DLibcov_noAutoSave libcov-posix.c
