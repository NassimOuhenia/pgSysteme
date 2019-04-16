#include <stdlib.h> /* pour malloc/free */
#include <stdio.h>  /* pour printf/fprintf */
#include <unistd.h> /* pour execve */
#include <string.h> /* pour strerror */
#include <errno.h>  /* pour errno */
#include <sys/types.h> /* pour waitpid */
#include <sys/wait.h> /* pour waitpid */
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdarg.h>

#define MODE S_IRUSR | S_IWUSR

typedef struct fileM {

  size_t len_max, nb_msg, count;
  int first, last;
  pthread_mutex_t mutex;
  pthread_cond_t wr;
  pthread_cond_t rd;
  void * fileMsg;

}File_M;

typedef struct message {

  int option;
  File_M * files;

}MESSAGE;


size_t msg_message_size(MESSAGE *);
size_t msg_capacite(MESSAGE *);
size_t msg_nb(MESSAGE *);

MESSAGE *msg_connect(const char *nom, int options,...);

int msg_disconnect(MESSAGE *file);
int msg_unlink(const char *nom);

int msg_send(MESSAGE *file, const void *msg, size_t len);
int msg_trysend(MESSAGE *file, const void *msg, size_t len);

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len);
ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len);
