#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

int main ([[maybe_unused]] int argc, char ** argv) {
  key_t key = ftok("semaphores.c", 'a');
  if (key == -1) {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  int sem_id = semget(key, 1, IPC_CREAT | 0777);
  if (sem_id == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  if (semctl(sem_id, 0, SETVAL, 1) == -1) {
    perror("semctl");
    exit(EXIT_FAILURE);
  }

  printf("Введите строку для записи в файл: ");
  char* buffer = NULL;
  size_t buffer_len = 0;
  ssize_t getline_res = getline(&buffer, &buffer_len, stdin);
  if (getline_res == -1) {
    perror("getline");
    free(buffer);
    exit(EXIT_FAILURE);
  }

  struct sembuf sb_lock = {0, -1, SEM_UNDO};
  printf("Locking...\n");
  semop(sem_id, &sb_lock, 1);
  printf("Locked\n");

  int fd = open("output.txt", O_WRONLY | O_CREAT, 0777);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  if (write(fd, buffer, getline_res) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  if (close(fd)) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  system("cat output.txt");

  sleep(5);
  struct sembuf sb_unlock = {0, 1, SEM_UNDO};
  semop(sem_id, &sb_unlock, 1);

  exit(EXIT_SUCCESS);
}
