#include "msg_file.h"
#include <string.h>

//nos test
int main(void) {

  int i=O_RDWR;//|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("aws",i,4,8);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  //int t=msg_send(mess, "moi", 3);

  char * hh;

  ssize_t f= msg_receive(mess, hh, 8);

  printf("après receive first = %d\n",mess->files->first);
  printf("message reçu = %s\n....", hh);
  sleep(1);

  return 0;


}
