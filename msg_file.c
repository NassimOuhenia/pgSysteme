#include "msg_file.h"


int filePleine(MESSAGE * file) {
  return (file->files->first == file->files->last);
}

int fileVide(MESSAGE * file) {
  return (file->files->first == -1);
}

int absVal(int a) {
  if(a < 0)
    return -a;
  return a;
}

int ecrire(File_M * files, const void *msg, size_t len) {
  if(len + 4 < absVal(files->first - files->last)) {
    int f = files->first;
    int l = files->last;
    memcpy(files->fileMsg+l, msg, len+1);
    return 0;
  } else {
    return -1;
  }
}

int msg_send(MESSAGE *file, const void *msg, size_t len) {
  pthread_mutex_lock( & file->files->mutex );
  if(filePleine(file)) {
    int n = pthread_cond_wait( & file->files->wr ,& file->files->mutex);
    if( n!= 0 ){
      perror("wait mutex ERROR");
      return 0;
    }
  int size = ecrire(file->files, msg,len);

  pthread_mutex_unlock( & file->files->mutex );
  pthread_cond_signal( & file->files->wr );

  return size;
}

int msg_trysend(MESSAGE *file, const void *msg, size_t len) {
  if(filePleine(file)) {
    return -1;
  } else {
    /******;
  }
}
