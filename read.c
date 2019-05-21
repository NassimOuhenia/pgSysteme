#include "msg_file.h"
#include <string.h>

//nos test
int main(void) {

  int i=O_RDWR;//|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("a",i,4,8);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  //int t=msg_send(mess, "moi", 3);
  char * hh;
  printf("apre send\n");
  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  ssize_t f= msg_receive(mess, hh, 8);
  printf("first = %d\n",mess->files->first);

  return 0;


}
