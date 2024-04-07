#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#include "globals.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  initscr(); /* Инициализация ncurses */
  cbreak();  /* Режим без буферизации */
  noecho();  /* Не отображать вводимые символы */

  srand((unsigned)time(NULL));

  /* Открываем именованный канал на запись */
  int upstream_gateway_fd = open(upstream_gateway_fifo, O_WRONLY);

  while (true) {
    int ch = getch();

    /* Симулируем потребность в терминалах */
    if (ch == 'u') {
      int terminal_required = 1;
      write(upstream_gateway_fd, &terminal_required, sizeof(int));
      printw("Terminal required\n");
    }
  }

  close(upstream_gateway_fd);
  endwin();

  exit(EXIT_SUCCESS);
}
