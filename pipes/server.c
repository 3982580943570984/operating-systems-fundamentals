#include <asm-generic/errno-base.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  if (mkfifo("fifo", 0777) == -1) {
    if (errno != EEXIST) {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
  }

  /* Производим чтение запрашиваемой команды */
  printf("open\n");
  int fd = open("fifo", O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  printf("read\n");
  char command[256];
  if (read(fd, command, 256) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }

  command[strcspn(command, "\n")] = 0;
  printf("%s\n", command);

  printf("close\n");
  if (close(fd) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  /* Производим запись результата запрашиваемой команды */
  fd = open("fifo", O_WRONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  FILE* fp = popen(command, "r");
  if (fp == NULL) {
    perror("popen");
    exit(EXIT_FAILURE);
  }

  char result[256];
  while(fgets(result, sizeof(result), fp) != NULL) {
    printf("%s", result);
    if (write(fd, result, strlen(result) + 1) == -1) {
      perror("write");
      exit(EXIT_FAILURE);
    }
  }

  if (close(fd) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
