#include "msg_file.h"


size_t msg_message_size(MESSAGE * file) {
  return file->files->len_max;
}

size_t msg_capacite(MESSAGE * file) {
  return (file->files->len_max*file->files->nb_msg);
}

size_t msg_nb(MESSAGE * file) {
  return file->files->count;
}
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

  if(len < absVal(files->first - files->last)) {
    int l = files->last;
    memcpy(files->fileMsg+l, &len, sizeof(size_t));
    memcpy(files->fileMsg+l+sizeof(size_t), msg, len+1);
    printf("%s\n", files->fileMsg);
    return 0;
  } else {
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

  if(len < lenMsg) {
    errno = EMSGSIZE;
    return -1;
  }

  memcpy(msg, files->fileMsg+fst+sizeof(size_t), lenMsg);

  return lenMsg;

}

void majLecture(MESSAGE * file, size_t len) {

  file->files->count--;
  file->files->first = (file->files->first+len+sizeof(size_t))%msg_capacite(file);

}


int msg_send(MESSAGE *file, const void *msg, size_t len) {

  pthread_mutex_lock( & file->files->mutex );

  if(filePleine(file)) {
    int n = pthread_cond_wait( & file->files->wr ,& file->files->mutex );
    if( n!= 0 ){
      pthread_mutex_unlock( & file->files->mutex );
      perror("wait mutex ERROR");
      return 0;
    }
  }
  int size = ecrire(file->files, msg,len);

  if(!size) {
    if(fileVide(file)) {
      file->files->first = 0;
    }
    majEcriture(file,len);
  }

  pthread_mutex_unlock( & file->files->mutex );
  pthread_cond_signal( & file->files->rd );

  return size;
}

int msg_trysend(MESSAGE *file, const void *msg, size_t len) {
  if(filePleine(file)) {
    errno = EAGAIN;
    return -1;
  } else {
    ////on verra après
    return 0;
  }
}

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {
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
  if(fileVide(file)) {
    errno = EAGAIN;
    return -1;
  } else {
    //on verra après
    return 0;
  }
}


int main (void) {
  printf("hello world\n");
  return 0;
}
