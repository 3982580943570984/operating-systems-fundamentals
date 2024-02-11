#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main([[maybe_unused]] int argc,[[maybe_unused]] char** argv) {
  /* Производим чтение со стандартного потока */
  int fd = open("fifo", O_WRONLY);
  if (fd == -1) {
    perror("open");
    return EXIT_FAILURE;
  }

  char* command = NULL;
  size_t command_len = 0;
  ssize_t getline_res;
  getline_res = getline(&command, &command_len, stdin);
  if (getline_res == -1) {
    perror("getline");
    free(command);
    exit(EXIT_FAILURE);
  }

  if (write(fd, command, getline_res) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  /* Производим чтение результата запрашиваемой команды */
  fd = open("fifo", O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  char buf[256];
  if (read(fd, buf, 256) == -1) {
    perror("read");
    exit(EXIT_FAILURE);
  }
  printf("%s", buf);

  int count = atoi(buf);
  
  printf("%d\n", atoi(argv[1]));

  if (atoi(argv[1]) < count) {
    printf("argv[1] < count\n");
  } else {
    printf("argv[1] >= count\n");
  }

  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
