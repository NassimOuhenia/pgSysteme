CC=gcc
LDLIBS=-lrt -pthread

ALL =   msg_file_test read send

all : $(ALL)

send.o: send.c msg_file.h
read.o: read.c msg_file.h
msg_file_test.o: msg_file_test.c msg_file.h
read: read.o msg_file.o
send: send.o msg_file.o
msg_file_test: msg_file_test.o msg_file.o
msg_file.o: msg_file.c


clean:
	rm -rf *.o
