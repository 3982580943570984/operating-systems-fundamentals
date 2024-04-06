#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "globals.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  srand((unsigned)time(NULL) + 1);

  /* Открываем именованные каналы на чтение */
  int downstream_gateway_fd = open(downstream_gateway_fifo, O_WRONLY);

  while (true) {
    /* Симулируем потребность в терминалах */
    int terminal_required = rand() % 2;

    if (terminal_required)
      printf("На нижнем течении требуется один терминал\n");

    /* Сообщаем о желаемых к использованию терминалах */
    write(downstream_gateway_fd, &terminal_required, sizeof(int));

    sleep(5);
  }

  close(downstream_gateway_fd);

  exit(EXIT_SUCCESS);
}
