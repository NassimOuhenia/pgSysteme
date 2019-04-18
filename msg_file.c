#include "msg_file.h"


size_t msg_message_size(MESSAGE * file) {
  return file->files->len_max;
}

size_t msg_capacite(MESSAGE * file) {
  return ((sizeof(size_t)+file->files->len_max)*file->files->nb_msg);
}

size_t msg_nb(MESSAGE * file) {
  return file->files->count;
}
int filePleine(MESSAGE * file) {
  return ((file->files->first == file->files->last) && (0 < file->files->count));
}

int fileVide(MESSAGE * file) {
  return (file->files->count == 0);
}

int absVal(int a) {
  if(a < 0)
    return -a;
  return a;
}

MESSAGE* creation_file(const char *nom, int options, size_t nb_msg, size_t len_max){


  printf("%s\n", "creation de file normal");
  printf("%ld\n",nb_msg );

  int fd = shm_open(nom, options, S_IRUSR | S_IWUSR);
  if(fd<0) {
    perror("shm_open");
    return NULL;
  }



  size_t len = nb_msg*(len_max+sizeof(size_t)) + sizeof(File_M);

  ftruncate(fd, len);
  printf("%ld\n",len );

  struct stat bufStat;
  fstat(fd,&bufStat);

  printf("%ld\n",bufStat.st_size);

  File_M *tab = (File_M *) mmap( 0 , len , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(tab == MAP_FAILED) {
    perror("erreur de mmap");
    return NULL;
  }

  //initialisation de la file-------------------------------------------------------------------------------------------------------------------------------

  tab->len_max = len_max;
  tab->nb_msg = nb_msg;
  tab->first = -1;
  tab->last = 0;
  tab->count = 0;


  MESSAGE* reponse=malloc(sizeof(MESSAGE));
  reponse->option = options;
  reponse->files = tab;


  printf("%d\n",reponse->files->first);
  return reponse;


}


MESSAGE* ouverture_file(const char *nom, int options){

  printf("%s\n", "ouverture de file");
  int fd = shm_open(nom,  options,S_IRUSR | S_IWUSR );

  if(fd<0) {
    perror("shm_open");
    return NULL;
  }

  struct stat bufStat;
  fstat(fd, &bufStat);
  size_t len = bufStat.st_size;
  printf("%ld\n",bufStat.st_size);


  File_M *tab = (File_M *) mmap(0 , len , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if(tab == MAP_FAILED) {
    perror("erreur de mmap");
    return NULL;
  }



  MESSAGE* reponse = malloc( sizeof(MESSAGE));
  reponse->option = options;
  reponse->files = tab;

  return reponse;
}

MESSAGE* creation_file_anonyme(const char *nom, int options, size_t nb_msg, size_t len_max){

  size_t len = nb_msg*(len_max+sizeof(size_t)) + sizeof(File_M);

  File_M *tab = (File_M *) mmap( 0 , len , PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

  if(tab == MAP_FAILED) {
    perror("erreur de mmap");
    return NULL;
  }


  //initialisation de la file-------------------------------------------------------------------------------------------------------------------------------

  tab->len_max = len_max;
  tab->nb_msg = nb_msg;
  tab->first = -1;
  tab->last = 0;
  tab->count = 0;


  MESSAGE* reponse = malloc(sizeof(MESSAGE));
  reponse->option = options;
  reponse->files = tab;

  return reponse;
}

//----------------------------------88888888888888888888888888888888888888888888888---------------------------------------------------------------------------------------------------------------------

MESSAGE *msg_connect( const char *nom, int options,... ){

  va_list va;
  va_start (va, options);

  if (nom != NULL) {
    /* code */

    if(!(options & O_CREAT)==0){
      size_t nb_msg = va_arg (va, size_t);
      size_t len_max = va_arg (va, size_t);
      creation_file(nom,options,nb_msg,len_max);
    }else{
      ouverture_file(nom, options);
    }

  } else{
    size_t nb_msg = va_arg (va, size_t);
    size_t len_max = va_arg (va, size_t);
    creation_file_anonyme(nom,options, nb_msg,len_max);

  }
  va_end (va);
}


int ecrire(File_M * files, const void *msg, size_t len) {


  if(len < absVal(files->first - files->last)||files->first==-1) {
    int l = files->last;
    memcpy(files->fileMsg+l, &len, sizeof(size_t));
    memcpy(files->fileMsg+l+sizeof(size_t), msg, len+1);
    //  printf("%s\n", files->fileMsg);
    return 0;
  } else {
    printf("%s\n","nn");
    return -1;
  }
}

void majEcriture(MESSAGE * file, size_t len) {

  file->files->count++;
  file->files->last = (file->files->last+len+sizeof(size_t))%msg_capacite(file);

}

int lire(File_M * files, void *msg, size_t len) {

  size_t lenMsg;
  int fst = files->first;

  memcpy(&lenMsg, files->fileMsg+fst, sizeof(size_t));


  memcpy(msg, files->fileMsg+fst+sizeof(size_t), lenMsg);

  return lenMsg;

}

void majLecture(MESSAGE * file, size_t len) {

  file->files->count--;
  file->files->first = (file->files->first+len+sizeof(size_t))%msg_capacite(file);

}


int msg_send(MESSAGE *file, const void *msg, size_t len) {

  if(len > file->files->len_max) {
    errno = EMSGSIZE;
    return -1;
  }

  pthread_mutex_lock( & file->files->mutex );

  if(filePleine(file)) {

    int n = pthread_cond_wait( & file->files->wr ,& file->files->mutex );
    if( n!= 0 ){
      pthread_mutex_unlock( & file->files->mutex );
      perror("wait mutex ERROR");
      return -1;
    }
  }

//printf("%s\n",(char *)msg);
  int size = ecrire(file->files, msg,len);

  if(!size) {
    if(fileVide(file)) {
      printf("%s\n","filevide");
      file->files->first = 0;
    }
    majEcriture(file,len);
  }

  pthread_mutex_unlock( & file->files->mutex );
  pthread_cond_signal( & file->files->rd );

  return size;
}

//-------------------------------------------------------------------------------------------------------------------------

int msg_trysend(MESSAGE *file, const void *msg, size_t len) {

  if(len > file->files->len_max) {
      printf("%s\n","filevide");
    errno = EMSGSIZE;
    return -1;
  }

  if(filePleine(file)) {
      printf("%s\n","filevide2");
    errno = EAGAIN;

    return -1;

  } else {

int response=pthread_mutex_trylock(& file->files->mutex);
  printf("%s\n","filevide55");
    printf("%d\n",response);
if(response==0){
    printf("%s\n","ecriture-------------------");

  int size = ecrire(file->files, msg,len);

  if(!size) {
    if(fileVide(file)) {
      printf("%s\n","filevide");
      file->files->first = 0;
    }
    majEcriture(file,len);
  }

  pthread_mutex_unlock( & file->files->mutex );
  pthread_cond_signal( & file->files->rd );
    return 0;

}

    ////on verra après
    return -1;
  }
}

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {

  if(len < file->files->len_max) {
    errno = EMSGSIZE;
    return -1;
  }

  pthread_mutex_lock( & file->files->mutex );
  if(fileVide(file)) {
    int n = pthread_cond_wait( & file->files->rd ,& file->files->mutex );
    if( n!= 0 ){
      pthread_mutex_unlock( & file->files->mutex );
      perror("wait mutex ERROR");
      return 0;
    }
  }

  int size = lire(file->files, msg,len);

  if(size != -1) {
    majLecture(file,len);
  }

  pthread_mutex_unlock( & file->files->mutex );
  pthread_cond_signal( & file->files->wr );

  return size;
}

ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len) {

  if(len < file->files->len_max) {
    errno = EMSGSIZE;
    return -1;
  }

  if(fileVide(file)) {
    errno = EAGAIN;
    return -1;
  } else {
    int response=pthread_mutex_trylock(& file->files->mutex);
    if(response){

      int size = lire(file->files, msg,len);

      if(size != -1) {
        majLecture(file,len);
      }

      pthread_mutex_unlock( & file->files->mutex );
      pthread_cond_signal( & file->files->wr );

      return size;
    }
return -1;

  }
}


int msg_disconnect(MESSAGE *file){
size_t len = file->files->nb_msg*(file->files->len_max+sizeof(size_t)) + sizeof(File_M);
int m=munmap(file->files, len);
return m;


}

int msg_unlink(const char *nom){
  int t=shm_unlink(nom);
  return t;
}

/*
int main (void) {
printf("hello world\n");
return 0;
}
*/
