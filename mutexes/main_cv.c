#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 8

char buffer[BUFFER_SIZE];
size_t buffer_length = 0;
int data_ready = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* sender_thread(void* arg) {
  FILE * file = fopen("./input.txt", "r");

  if (file == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  while (!feof(file)) {
    pthread_mutex_lock(&mutex);

    while (data_ready) {
      pthread_cond_wait(&cond, &mutex);
    }

    buffer_length = fread(buffer, 1, BUFFER_SIZE, file);
    data_ready = 1;
    
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
  }

  pthread_mutex_lock(&mutex);
  data_ready = 2;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);

  fclose(file);

  return NULL;
}

void* receiver_thread(void* arg) {
  FILE * file = fopen("./output.txt", "w");

  if (file == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  while (1) {
    pthread_mutex_lock(&mutex);
    
    while (!data_ready) {
      pthread_cond_wait(&cond, &mutex);
    }

    if (data_ready == 2) {
      pthread_mutex_unlock(&mutex);
      break;
    }

    fwrite(buffer, 1, buffer_length, file);
    data_ready = 0;
    
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
  }

  fclose(file);

  return NULL;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
  pthread_t tid1, tid2;

  if (pthread_create(&tid1, NULL, &sender_thread, NULL) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&tid2, NULL, &receiver_thread, NULL) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }
  
  if (pthread_join(tid1, NULL) != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(tid2, NULL) != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_destroy(&mutex) != 0) {
    perror("pthread_mutex_destroy");
    exit(EXIT_FAILURE);
  }

  system("diff ./input.txt ./output.txt");

  exit(EXIT_SUCCESS);
}


