#include "msg_file.h"
#include <string.h>

//test decriture et attente quand ces plein

int main(void) {

  pid_t pids[7];

  int i=O_RDWR|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("kllls",i,4,9);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("Creation de la file par le pere---------------first = %d last = %d\n",mess->files->first,mess->files->last);
  pid_t pidlire = fork();

  if (pidlire==-1) {
    perror("error fork");
    exit(1);
  } else if (pidlire == 0) {

    printf("Connection du fils %d pour une lecture first = %d last = %d \n",getpid(),mess->files->first,mess->files->last);
    char* receivBuf=malloc(sizeof(char)*10);

    int t=msg_tryreceive(mess, receivBuf, 5);

    printf("Apres Connection  du fils %d pour une lecture first = %d last = %d \n",getpid(),mess->files->first,mess->files->last);
    exit(0);
  }

  for(int i = 0; i < 7; i++) {
    pids[i] = fork();

    if(pids[i] == -1) {
      /* erreur de fork */
      perror("error fork");
      exit(1);
    } else if(pids[i] == 0) {
      /* processus fils */
      const pid_t pid_fils = getpid();

      int i=O_RDWR ;

      MESSAGE* mess=msg_connect("kllls",i);

      if(mess == NULL) {
        perror("erreur de connexion");
        return 0;
      }

      int t = msg_trysend(mess, "moiii", 5);
      char * hh;

      printf("deconnexion...\n");
      sleep(1);
      msg_disconnect(mess);

      exit(0);
    } else {
      printf("processus père: création du processus fils %d/4 %d ok\n", i+1, pids[i]);
    }

  }

  for(int i = 0; i < 7; i++) {

    waitpid(pids[i], NULL, 0);
    printf("-------------------------processus père: fils %d mort\n", pids[i]);
    sleep(1);
    printf("Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);

    if (pids[i]=pids[5]) {
      char* receivBuf = malloc(sizeof(char)*10);
      int t = msg_tryreceive(mess, receivBuf, 9);
    }
    printf("Apres Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);
  }

  printf("Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);
  char* receivBuf=malloc(sizeof(char)*10);

  int t=msg_tryreceive(mess, receivBuf, 5);

  printf("Apres Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);

  printf("moiiiisis\n");
  msg_unlink("kllls");
  exit(0);
}
