#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int var = 1;

int main ([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  } 

  if (dup2(fd, STDOUT_FILENO) == -1) {
    perror("dup2");
    exit(EXIT_FAILURE);
  }
  close(fd);

  printf("Исходное значение var: %d\n", var);
  ++var;
  printf("Значение var после инкремента: %d\n", var);

	pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    printf("Дочерний процесс: %d\n", getpid());
    printf("Дочерний процесс до увеличения var: %d\n", var);
    var += 5;
    printf("Дочерний процесс после увеличения: %d\n", var);
  } else {
    printf("Родительский процесс: %d\n", getpid());
    printf("Родительский процесс до увеличения: %d\n", var);
    var += 3;
    printf("Родительский процесс после увеличения: %d\n", var);
  }
}
