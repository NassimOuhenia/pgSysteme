#include "msg_file.h"


//nos test
int main(void) {

  int i=O_RDWR|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("mw",i,20,1000);

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  //int t=msg_send(mess, "moi", 3);
  int t=msg_trysend(mess, "moi", 1000);
  char * hh;
  printf("apre send\n");
  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

//  ssize_t f= msg_receive(mess, hh, 3);
ssize_t f= msg_tryreceive(mess, hh, 3);
  printf("first = %d\n",mess->files->first);

  printf("moi ? = %s\n", hh);

   int fin=msg_disconnect(mess);
   printf("last = %d\n",fin);
//printf("apres deconnection = %d\n",mess->files->first);

 int sort=msg_unlink("mw");
  printf("last = %d\n",sort);

}
