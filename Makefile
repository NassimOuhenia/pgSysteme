CC=gcc
LDLIBS=-lrt -pthread

ALL =   msg_file_test clean

all : $(ALL)

msg_file_test.o: msg_file_test.c msg_file.h
msg_file_test: msg_file_test.o msg_file.o
msg_file.o: msg_file.c


clean:
	rm -rf *.o
