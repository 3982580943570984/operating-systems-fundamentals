#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int fd[2];

void* sender_thread(void* arg) {
  const char* messages[] = {"Привет", "Это тест", "Передача сообщений", "Через канал", "Конец"};
  int n = sizeof(messages) / sizeof(messages[0]);

  for (int i = 0; i < n; ++i) {
    write(fd[1], messages[i], strlen(messages[i]) + 1);
    sleep(1);
  }

  return NULL;
}

void* receiver_thread(void* arg) {
  char buffer[1024];

  while (1) {
    int bytes_read = read(fd[0], buffer, sizeof(buffer));
    if (bytes_read <= 0) break;
    printf("Получено сообщение: %s\n", buffer);
  }

  return NULL;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
  pthread_t tid1, tid2;

  if (pipe(fd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&tid1, NULL, sender_thread, NULL) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&tid2, NULL, receiver_thread, NULL) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }
  
  if (pthread_join(tid1, NULL) != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (close(fd[1]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(tid2, NULL) != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (close(fd[0]) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
