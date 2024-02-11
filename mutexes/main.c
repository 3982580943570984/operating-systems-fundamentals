#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 8

char buffer[BUFFER_SIZE];
size_t buffer_length = 0;
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
    printf("sender_thread: pthread_mutex_lock\n");
    
    buffer_length = fread(buffer, 1, BUFFER_SIZE, file);
    
    printf("sender_thread: pthread_mutex_unlock\n");
    pthread_mutex_unlock(&mutex);
    
    usleep(100);
  }

  fclose(file);

  return NULL;
}

void* receiver_thread(void* arg) {
  FILE * file = fopen("./output.txt", "w");

  if (file == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  while (buffer_length) {
    pthread_mutex_lock(&mutex);
    printf("receiver_thread: pthread_mutex_lock\n");
    
    fwrite(buffer, 1, buffer_length, file);
    buffer_length = 0;
    
    printf("receiver_thread: pthread_mutex_unlock\n");
    pthread_mutex_unlock(&mutex);

    usleep(100);
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

