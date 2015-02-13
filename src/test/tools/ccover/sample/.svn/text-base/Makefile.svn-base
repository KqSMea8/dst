# Copyright (c) Bullseye Testing Technology
# Makefile to build sample programs

CC=gcc
CXX=g++

all: calc1 calc2

run: calc1 calc2
	echo 123 456 + 2 / = | ./calc1
	echo 123 + 456 / 2   | ./calc2

calc1: calc1.cpp
	$(CXX) calc1.cpp -o $@
calc2: calc2.c
	$(CC) calc2.c -o $@

clean:
	rm -f *.o calc1 calc2
