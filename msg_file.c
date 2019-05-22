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
  return ((files->first == files->last) && ( files->count>0));
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

  pthread_mutexattr_t attr;
  pthread_condattr_t cattr;

  if(pthread_mutexattr_init(&attr) != 0) {
    fprintf(stderr, "pthread_mutexattr_init\n");
    exit(1);
  }

  if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_mutex_init(&(file->mutex), &attr) != 0) {
    fprintf(stderr, "pthread_mutex_init\n");
    exit(1);
  }

  if(pthread_mutexattr_init(&attr) != 0) {
    fprintf(stderr, "pthread_mutexattr_init\n");
    exit(1);
  }

  if(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_mutex_init(&(file->mutexLec), &attr) != 0) {
    fprintf(stderr, "pthread_mutex_init\n");
    exit(1);
  }

  if(pthread_condattr_init(&cattr) != 0) {
    fprintf(stderr, "pthread_condattr_init\n");
    exit(1);
  }

  if(pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_cond_init(&(file->wr), &cattr) != 0) {
    fprintf(stderr, "pthread_cond_init\n");
    exit(1);
  }

  if(pthread_condattr_init(&cattr) != 0) {
    fprintf(stderr, "pthread_condattr_init\n");
    exit(1);
  }

  if(pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED) != 0) {
    fprintf(stderr, "pthread_mutexattr_setpshared\n");
    exit(1);
  }

  if(pthread_cond_init(&(file->rd), &cattr) != 0) {
    fprintf(stderr, "pthread_cond_init\n");
    exit(1);
  }
}

//*************8pour la connection*****
MESSAGE* creation_file(const char *nom, int options, size_t nb_msg, size_t len_max){

  //printf("%s\n", "creation de file normal");

  int fd = shm_open(nom, options, S_IRUSR | S_IWUSR);
  if(fd<0) {
    perror("shm_open");
    return NULL;
  }

  size_t len = nb_msg*(len_max+sizeof(size_t)) + sizeof(File_M);

  ftruncate(fd, len);

  struct stat bufStat;
  fstat(fd,&bufStat);

  File_M *tab = (File_M *) mmap( 0 , len , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(tab == MAP_FAILED) {
    perror("erreur de mmap");
    return NULL;
  }

  //initialisation de la file------------------------------------------------------
  tab->len_max = len_max;
  tab->nb_msg = nb_msg;
  tab->first = 0;
  tab->last = 0;
  tab->fin = -1;
  tab->count = 0;

  init_mutex(tab);

  MESSAGE* reponse=malloc(sizeof(MESSAGE));
  reponse->option = options;
  reponse->files = tab;

  printf("creation de file CAPACITE %ld\n", msg_capacite(reponse));
  return reponse;

}

MESSAGE* ouverture_file(const char *nom, int options){

  //printf("%s\n", "ouverture de file");
  int fd = shm_open(nom,  options,S_IRUSR | S_IWUSR );

  if(fd<0) {
    perror("shm_open");
    return NULL;
  }

  struct stat bufStat;
  fstat(fd, &bufStat);
  size_t len = bufStat.st_size;

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
  tab->first = 0;
  tab->last = 0;
  tab->fin = -1;
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

//**********************************************************************************************************************fin connection****************************************************
int ecrire(File_M * files, const void *msg, size_t len, int indexEcrire) {

  memcpy((files->fileMsg+indexEcrire), &len, sizeof(size_t));

  printf(" apres ecriture de la taille par %d on a ---------------------------%s\n",getpid(),files->fileMsg+indexEcrire );
  memcpy((files->fileMsg+indexEcrire+sizeof(size_t)), msg, len);

  printf(" apres ecriture de la taille par %d on a ---------------------------%s\n",getpid(),files->fileMsg+indexEcrire+sizeof(size_t));

  return 0;
}

size_t calculeEspaceWrite(MESSAGE * file){
  int espace=file->files->first-file->files->last;
  if (espace<0) {
    return msg_capacite(file)+espace;

  }else if(espace>0){

    return espace;
  }else  if(espace==0){
    if (filePleine(file->files)) {

      return 0;
    }else{

      return msg_capacite(file);
    }
  }
}

int majEcriture(MESSAGE * file, size_t len) {

  int old = file->files->last;
  file->files->count++;
  file->files->last = (file->files->last+len+sizeof(size_t))%msg_capacite(file);
    printf("NOMBRE DE MESSAGE ecriture 444444444444444444444444444444444444  %ld first %d last %d\n",   file->files->count,file->files->first,file->files->last);

  return old;
}

int lire(File_M * files, void *msg, size_t len, int indexLire ) {

  size_t lenMsg;
  memcpy(&lenMsg, files->fileMsg+indexLire, sizeof(size_t));

  printf("%ld la longeur du message lu\n",lenMsg);

  if(len < lenMsg) {
    errno = EMSGSIZE;
    perror("lire");
    return -1;
  }

  memcpy(msg, files->fileMsg+indexLire+sizeof(size_t), lenMsg);
    printf("%s la longeur du message lu\n",(char*)msg);

  return lenMsg;

}

int majLecture(MESSAGE * file, size_t len) {

  size_t lenMsg;
  memcpy(&lenMsg, file->files->fileMsg+file->files->first, sizeof(size_t));

  int old = file->files->first;
  file->files->count--;
    printf("nouveauuuuuuuu first est --------------%ld et encien %d message longeur %ld\n",(file->files->first+lenMsg+sizeof(size_t))%msg_capacite(file),old,lenMsg);

  file->files->first = (file->files->first+lenMsg+sizeof(size_t))%msg_capacite(file);
  printf("NOMBRE DE MESSAGE Lecture 444444444444444444444444444444444444444444444444 %ld first %d last %d\n",   file->files->count,file->files->first,file->files->last);

  return old;
}

//*****************************************************************************send*********************************************************************************
int msg_send(MESSAGE *file, const void *msg, size_t len) {

  if(len>file->files->len_max){
    printf("message trop grand pour ecriture\n" );
    return -1;
  }

  pthread_mutex_lock( & file->files->mutex );

  //printf("Connection du proc %d pour une ecriture  first = %d last = %d\n",getpid(),file->files->first,file->files->last);
  //printf("%d lenght %ld capacite restante %ld\n",filePleine(file->files), calculeEspaceWrite(file),msg_capacite(file)-file->files->last);

  if((msg_capacite(file)-file->files->last)<sizeof(size_t)+len) {
    file->files->fin = file->files->last;
    file->files->last=0;
  }

  while(filePleine(file->files) || calculeEspaceWrite(file) < len+sizeof(size_t)) {
    printf("processus %d en attente\n", (int) getpid());
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

//-------------------------------------------------------------
int msg_trysend(MESSAGE *file, const void *msg, size_t len) {

  if(len>file->files->len_max){
    printf("message trop grand pour ecriture\n" );
    return -1;
  }

  if(pthread_mutex_trylock( & file->files->mutex ) != 0) {
    perror("can't block0000000000000000000000000000000000000000000000000000000");
    return -1;
  }

  if((msg_capacite(file)-file->files->last)<sizeof(size_t)+len){
    file->files->fin = file->files->last;

    file->files->last=0;
  }

  while(filePleine(file->files) || calculeEspaceWrite(file) < len+sizeof(size_t)) {
    printf("processus %d en attente\n", (int) getpid());
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

//*************88receive*****************
ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {

  size_t capaciteAvant = msg_capacite(file)-file->files->first;
  if(file->files->len_max < len){
    perror("taille message a lire incorrect\n" );
    return -1;
  }

  pthread_mutex_lock( & file->files->mutexLec );

  while(fileVide(file->files)) {
    int n = pthread_cond_wait( & file->files->rd ,& file->files->mutexLec );
  }

  if(file->files->first == file->files->fin){
    printf("mise a zero 666666666666666666666666666666666666 %ld size %ld\n", capaciteAvant,sizeof(size_t)+len);
    file->files->first = 0;
    file->files->fin = -1;
  }

  int indexLire = majLecture(file,len);

  pthread_mutex_unlock( & file->files->mutexLec );
  pthread_cond_broadcast( & file->files->wr );
  int size = lire(file->files, msg, len, indexLire);

  return size;
}

ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len) {


  if(file->files->len_max < len){
    perror("taille message a lire incorrect\n" );
    return -1;
  }

  if(pthread_mutex_trylock( & file->files->mutexLec ) != 0) {
    perror("can't block lecture 33333333333333333333333333333333333333333333333");
    return -1;
  }

  while(fileVide(file->files)) {
    int n = pthread_cond_wait( & file->files->rd ,& file->files->mutexLec );
  }

  if(file->files->first == file->files->fin){
    printf("mise a zero 666666666666666666666666666666666666\n");
    file->files->first = 0;
    file->files->fin = -1;
  }

  int indexLire = majLecture(file,len);

  pthread_mutex_unlock( & file->files->mutexLec );
  pthread_cond_broadcast( & file->files->wr );
  int size = lire(file->files, msg, len, indexLire);

  return size;
}

int msg_disconnect(MESSAGE *file){
  size_t len = msg_capacite(file) + sizeof(File_M);
  return munmap(file->files, len);
}

int msg_unlink(const char *nom){
  return shm_unlink(nom);
}
