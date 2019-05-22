#include "msg_file.h"
#include <string.h>

//nos test
int main(void) {

  int i=O_RDWR;//|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("kll",i,4,8);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  printf("apres send de message: moiii\n");
  int t = msg_send(mess, "moiii", 5);
  sleep(1);

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  return 0;


}
