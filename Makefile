CC=gcc
LDLIBS= -Wall -lrt -pthread

ALL =   msg_file_test read send try_send try_receive msg_file_test2

all : $(ALL)

send.o: send.c msg_file.h
read.o: read.c msg_file.h
read: read.o msg_file.o
send: send.o msg_file.o
try_send.o: try_send.c msg_file.h
try_receive.o: try_receive.c msg_file.h
try_receive: try_receive.o msg_file.o
try_send: try_send.o msg_file.o
msg_file_test.o: msg_file_test.c msg_file.h
msg_file_test2.o: msg_file_test2.c msg_file.h
msg_file_test: msg_file_test.o msg_file.o
msg_file_test2: msg_file_test2.o msg_file.o
msg_file.o: msg_file.c


clean:
	rm -rf *.o
