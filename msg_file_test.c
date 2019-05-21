#include "msg_file.h"
#include <string.h>

#define SEND "send"
#define RECEIVE "receive"
#define QUIT "quit"
#define DISCONNECT "disconnect"

//nos test
//test decriture et attente quand ces plein

int main(void) {

  pid_t pids[7];


  int i=O_RDWR|O_CREAT| O_EXCL;
  MESSAGE* mess=msg_connect("axdsaaf",i,4,9);

  if(mess == NULL) {
    perror("erreur de connexion");
    return 0;
  }

  printf("Creation de la file par le pere-------------------------------first = %d last = %d\n",mess->files->first,mess->files->last);
pid_t pidlire=fork();
if (pidlire==-1) {
  /* code */
}else if (pidlire==0) {
  /* code */

  printf("Connection du fils %d pour une lecture first = %d last = %d \n",getpid(),mess->files->first,mess->files->last);
  char* receivBuf=malloc(sizeof(char)*10);

  int t=msg_receive(mess, receivBuf, 5);

    printf("Apres Connection  du fils %d pour une lecture first = %d last = %d \n",getpid(),mess->files->first,mess->files->last);
    exit(0);
}


  for(int i = 0; i < 7; i++) {
       pids[i] = fork();

       if(pids[i] == -1) {
            /* erreur de fork */
            exit(1);
       } else if(pids[i] == 0) {
            /* processus fils */
            const pid_t pid_fils = getpid();
            printf(" FILS %d\n",pid_fils );
                      int i=O_RDWR ;
                      MESSAGE* mess=msg_connect("axdsaaf",i);

                      if(mess == NULL) {
                        perror("erreur de connexion");
                        return 0;
                      }




                      int t=msg_send(mess, "moiii", 5);
                      char * hh;
printf("Apres Connection du fils %d pour une ecriture  first = %d last = %d\n",pid_fils,mess->files->first,mess->files->last);

                      msg_disconnect(mess);


            exit(0);
       } else {
            printf("processus père: création du processus fils %d/4 %d ok\n", i+1, pids[i]);
       }

  }

  /* Le père attend la mort des fils */
  for(int i = 0; i < 7; i++) {
       waitpid(pids[i], NULL, 0);
       printf("-------------------------processus père: fils %d mort\n", pids[i]);
       printf("Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);
       if (pids[i]=pids[5]) {
         /* code */
           char* receivBuf=malloc(sizeof(char)*10);
            int t=msg_receive(mess, receivBuf, 9);
       }
         printf("Apres Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);
  }

  printf("Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);
  char* receivBuf=malloc(sizeof(char)*10);

   int t=msg_receive(mess, receivBuf, 5);

    printf("Apres Connection du pere pour une lecture first = %d last = %d \n",mess->files->first,mess->files->last);



  printf("moiiiisis\n");
   msg_unlink("axdsaaf");
   exit(0);
}




/*

int main(void) {
{
     pid_t pids[4];


     int i=O_RDWR|O_CREAT| O_EXCL;
     MESSAGE* mess=msg_connect("ds",i,4,10);

     if(mess == NULL) {
       perror("erreur de connexion");
       return 0;
     }

     printf("Creation de la file par le pere-------------------------------\n");

     printf("first = %d\n",mess->files->first);
     printf("last = %d\n",mess->files->last);

     printf("-------------------------------------------------------------creationfin\n" );


     for(int i = 0; i < 8; i++) {
          pids[i] = fork();

          if(pids[i] == -1) {

               exit(1);
          } else if(pids[i] == 0) {

               const pid_t pid_fils = getpid();





               printf(" FILS %d\n",pid_fils );
                         int i=O_RDWR ;
                         MESSAGE* mess=msg_connect("ds",i);

                         if(mess == NULL) {
                           perror("erreur de connexion");
                           return 0;
                         }
                            srandom(pid_fils);
const int   randtime = rand() % 2;
printf("voilaaa %d\n", randtime);

if(randtime==1){
  printf("Connection du fils %d pour une lecture first = %d last = %d \n",pid_fils,mess->files->first,mess->files->last);

  char* receivBuf=malloc(sizeof(char)*10);

  int t=msg_receive(mess, receivBuf, 10);
  char * hh;
  printf("Apres Connection du fils %d pour une lecture first = %d last = %d\n",pid_fils,mess->files->first,mess->files->last);

  msg_disconnect(mess);


exit(0);

}

                         printf("Connection du fils %d pour une ecriture  first = %d last = %d\n",pid_fils,mess->files->first,mess->files->last);

                         int t=msg_send(mess, "moi", 10);
                         char * hh;
printf("Apres Connection du fils %d pour une ecriture  first = %d last = %d\n",pid_fils,mess->files->first,mess->files->last);

                         msg_disconnect(mess);


               exit(0);
          } else {
               printf("processus père: création du processus fils %d/4 %d ok\n", i+1, pids[i]);
          }

     }

     /* Le père attend la mort des fils */

    /* for(int i = 0; i < 8; i++) {
          waitpid(pids[i], NULL, 0);
          printf("-------------------------processus père: fils %d mort\n", pids[i]);
     }

     printf("moiiiisis\n");
      msg_unlink("ds");

     exit(0);
}}*/
