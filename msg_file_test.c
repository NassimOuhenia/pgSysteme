#include "msg_file.h"
#include <string.h>

#define SEND "send"
#define RECEIVE "receive"
#define QUIT "quit"
#define DISCONNECT "disconnect"

//nos test
int main(void) {

  int i=O_RDWR;//|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("accmklsdlj1jl",i,20,1000);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  int t=msg_send(mess, "moi", 3);
  char * hh;
  printf("apre send\n");
  printf("first = %d\n",mess->files->first);
  printf("last = %d\n",mess->files->last);

  //ssize_t f= msg_receive(mess, hh, 3);
  printf("first = %d\n",mess->files->first);

  printf("moi ? = %s\n", hh);

  /*
  char commande[100];
  char arg[100]; // a remplacer par len_max du msg

  scanf("%s", commande);

  while (strcmp(commande, QUIT)) {
    if(!strcmp(commande, RECEIVE)) {
      printf("good");
      return 0;
    } else if(!strcmp(commande, QUIT)) {
      printf("good");
      return 0;
    } else if(!strcmp(commande, DISCONNECT)) {
      printf("good");
      return 0;
    } else if(strstr(commande, SEND)) {
      printf("good");
      return 0;
    }
    scanf("%s", commande);
  }*/

  return 0;


}
