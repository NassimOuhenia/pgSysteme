CC=gcc
CFLAGS=-Wall -std=c11
CPPFLAGS=-D_POSIX_C_SOURCE=200112L -DNDEBUG
LDLIBS=-lrt -pthread

ALL =   msg_file clean

all : $(ALL)

ecrivain_nomme.o: msg_file.c msg_file.h
msg_file: msg_file.o


clean:
	rm -rf *.o
