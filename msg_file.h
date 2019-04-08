#include <stdlib.h> /* pour malloc/free */
#include <stdio.h>  /* pour printf/fprintf */
#include <unistd.h> /* pour execve */
#include <string.h> /* pour strerror */
#include <errno.h>  /* pour errno */
#include <sys/types.h> /* pour waitpid */
#include <sys/wait.h> /* pour waitpid */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdarg.h>
#include <syslog.h>


typedef struct en_tete {
  size_t len_max, nb_msg;
  int first, last;
  sem_t * semaphore;
}EN_TETE;

typedef struct fileM {
  EN_TETE e;
  char * file;
}FILE_M;

typedef struct message {
  int option;
  FILE_M * file;
}MESSAGE;


MESSAGE *msg_connect(const char *nom, int options,...);

int msg_disconnect(MESSAGE *file);

int msg_unlink(const char *nom);

int msg_send(MESSAGE *file, const char *msg, size_t len);
int msg_trysend(MESSAGE *file, const char *msg, size_t len);

size_t msg_receive(MESSAGE *file, char *msg, size_t len);
size_t msg_tryreceive(MESSAGE *file, char *msg, size_t len);

size_t msg_message_size(MESSAGE *);
size_t msg_capacite(MESSAGE *);
size_t msg_nb(MESSAGE *);
