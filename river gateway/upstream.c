#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "globals.h"

int main([[maybe_unused]] int argc,[[maybe_unused]] char ** argv)
{
  /* Открываем именованные каналы на чтение */
  int gateway_upstream_fd = open(gateway_upstream_fifo, O_RDONLY);
  int upstream_gateway_fd = open(upstream_gateway_fifo, O_WRONLY);

  while (true) {
    /* Определяем количество доступных для использования терминалов */
    int terminal_count;
    read(gateway_upstream_fd, &terminal_count, sizeof(int));
    printf("Количество доступных терминалов: %d\n", terminal_count);

    /* Определяем номера доступных терминалов */
    int terminal_indices[TERMINALS_COUNT] = {0};
    for (int i = 0; i < terminal_count; ++i) {
      read(gateway_upstream_fd, &terminal_indices[i], sizeof(int));
      printf("terminal_index = %d\n", terminal_indices[i]);
    }

    /* Сообщаем о желаемых к использованию терминалах */
    write(upstream_gateway_fd, &terminal_count, sizeof(int));
    for (int i = 0; i < terminal_count; ++i) {
      write(upstream_gateway_fd, &terminal_indices[i], sizeof(int));
    }

    sleep(1);
    printf("Итерация закончена\n");
  }

  close(upstream_gateway_fd);
  close(gateway_upstream_fd);

  exit(EXIT_SUCCESS);
}
