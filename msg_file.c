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

int filePleine(File_M * files) {
  return ((files->first == files->last) && (0 < files->count));
}

int fileVide(File_M * files) {
  return (files->count == 0);
}

int absVal(int a) {
  if(a < 0)
  return -a;
  return a;
}

void init_mutex(File_M *file) {
  if(pthread_mutexattr_init(&(file->send)) != 0) {
    fprintf(stderr, "pthread_mutexattr_init\n");
    exit(1);
  }

  if(pthread_mutexattr_setpshared(&file->send, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_mutex_init(&(file->mutex), &file->send) != 0) {
    fprintf(stderr, "pthread_mutex_init\n");
    exit(1);
  }


  if(pthread_condattr_init(&file->c_rd) != 0) {
    fprintf(stderr, "pthread_condattr_init\n");
    exit(1);
  }

  if(pthread_condattr_setpshared(&file->c_rd, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_cond_init(&(file->rd), &file->c_rd) != 0) {
    fprintf(stderr, "pthread_cond_init\n");
    exit(1);
  }

  if(pthread_condattr_init(&file->c_wr) != 0) {
    fprintf(stderr, "pthread_condattr_init\n");
    exit(1);
  }

  if(pthread_condattr_setpshared(&file->c_wr, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_cond_init(&(file->wr), &file->c_wr) != 0) {
    fprintf(stderr, "pthread_cond_init\n");
    exit(1);
  }
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

  init_mutex(tab);

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

  init_mutex(tab);

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


int ecrire(File_M * files, const void *msg, size_t len, int indexEcrire) {

  if(len < absVal(files->first - indexEcrire) || fileVide(files)) {

    memcpy(files->fileMsg+indexEcrire, &len, sizeof(size_t));
    memcpy(files->fileMsg+indexEcrire+sizeof(size_t), msg, len+1);
    //  printf("%s\n", files->fileMsg);
    return 0;
  } else {
    perror("ecrire");
    return -1;
  }
}

int majEcriture(MESSAGE * file, size_t len) {
  int old =   file->files->last;
  file->files->count++;
  file->files->last = (file->files->last+len+sizeof(size_t))%msg_capacite(file);
  return old;
}

int lire(File_M * files, void *msg, size_t len, int indexLire) {

  size_t lenMsg;
  memcpy(&lenMsg, files->fileMsg+indexLire, sizeof(size_t));

  if(len < lenMsg) {
    errno = EMSGSIZE;
    perror("lire");
    return -1;
  }

  memcpy(msg, files->fileMsg+indexLire+sizeof(size_t), lenMsg);
  return lenMsg;

}

int majLecture(MESSAGE * file, size_t len) {
  int old = file->files->first;
  file->files->count--;
  file->files->first = (file->files->first+len+sizeof(size_t))%msg_capacite(file);
  return old;
}


int msg_send(MESSAGE *file, const void *msg, size_t len) {

  pthread_mutex_lock( & file->files->mutex );

  while(filePleine(file->files)) {
    int n = pthread_cond_wait( & file->files->wr ,& file->files->mutex );
  }

  int indexEcrire = majEcriture(file,len);
  pthread_mutex_unlock( & file->files->mutex );

  int size = ecrire(file->files, msg, len, indexEcrire);
  pthread_cond_broadcast( & file->files->rd );

  if(!size) {
    if(file->files->first == -1) {
      file->files->first = 0;
    }
  }

  return size;
}

//-------------------------------------------------------------------------------------------------

int msg_trysend(MESSAGE *file, const void *msg, size_t len) {

  if(pthread_mutex_trylock( & file->files->mutex ) != 0) {
    perror("can't block");
    return -1;
  }

  if(filePleine(file->files)) {
    pthread_mutex_unlock( & file->files->mutex );

    errno = EAGAIN;
    perror("try_send");
    return -1;
  }

  int indexEcrire = majEcriture(file,len);
  pthread_mutex_unlock( & file->files->mutex );

  int size = ecrire(file->files, msg, len, indexEcrire);

  if(!size) {
    if(file->files->first == -1) {
      file->files->first = 0;
    }
  }

  pthread_cond_broadcast( & file->files->rd );
  return size;
}


ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {
  pthread_mutex_lock( & file->files->mutex );
  while(fileVide(file->files)) {
    int n = pthread_cond_wait( & file->files->rd ,& file->files->mutex );
  }

  int indexLire = majLecture(file,len);

  pthread_mutex_unlock( & file->files->mutex );
  int size = lire(file->files, msg, len, indexLire);
  pthread_cond_broadcast( & file->files->wr );

  return size;
}

ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len) {

  if(pthread_mutex_trylock( & file->files->mutex ) != 0) {
    perror("can't block");
    return -1;
  }
  if(fileVide(file->files)) {
    pthread_mutex_unlock( & file->files->mutex );
    errno = EAGAIN;
    perror("try_receive");
    return -1;
  }

  int indexLire = majLecture(file,len);
  pthread_mutex_unlock( & file->files->mutex );

  int size = lire(file->files, msg, len, indexLire);
  pthread_cond_broadcast( & file->files->wr );

  return size;
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
