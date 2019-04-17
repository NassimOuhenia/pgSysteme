CC=gcc
CFLAGS=-Wall -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200112L -DNDEBUG
LDLIBS=-lrt -pthread

ALL =   msg_file_test clean

all : $(ALL)

msg_file_test.o: msg_file_test.c msg_file.h
msg_file_test: msg_file_test.o msg_file.o
msg_file.o: msg_file.c


clean:
	rm -rf *.o
