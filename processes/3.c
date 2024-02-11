#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char** argv) {
  if (argc != 4) {
    return EXIT_FAILURE;
  }

  int wait_time = atoi(argv[1]);
  pid_t pid = atoi(argv[2]);
  int signal_code = atoi(argv[3]);

  printf("Ожидание %d секунд перед отправкой сигнала %d процессу с PID %d...\n", wait_time, signal_code, pid);
  sleep(wait_time);

  if (kill(pid, signal_code) == -1) {
    perror("Ошибка при отправке сигнала");
    return EXIT_FAILURE;
  }

  printf("Сигнал %d успешно отправлен процессу с PID %d\n", signal_code, pid);
  return EXIT_SUCCESS;
}
