#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main ([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  printf("%d\n", getpid());
  sleep(1000);

	pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    printf("Дочерний процесс: %d\n", getpid());
    int sleep_time = 5;
    sleep(sleep_time);
    exit(EXIT_SUCCESS);
  } else {
    printf("Родительский процесс: %d\n", getpid());
    int elapsed_time = 0;
    int wait_time = 1;
    int status;
    while(elapsed_time < wait_time) {
      if (waitpid(pid, &status, WNOHANG) == 0) {
        sleep(1);
        ++elapsed_time;
      } else {
        printf("Родительский процесс: дочерний процесс завершился до истечения времени ожидания.\n");
        exit(EXIT_SUCCESS);
      }
    }

    printf("Родительский процесс: время ожидания истекло.\n");
  }
}
