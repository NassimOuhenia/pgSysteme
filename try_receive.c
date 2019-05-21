#include "msg_file.h"
#include <string.h>

#define SEND "send"
#define RECEIVE "receive"
#define QUIT "quit"
#define DISCONNECT "disconnect"

//nos test
int main(void) {

  int i=O_RDWR;//|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("aw",i,4,8);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  //int t=msg_send(mess, "moiii", 5);
  char * hh;
  printf("apre send\n");
  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  ssize_t f= msg_tryreceive(mess, hh, 3);
  printf("first = %d\n",mess->files->first);

  printf("moi ? = %s\n", hh);

  return 0;


}
