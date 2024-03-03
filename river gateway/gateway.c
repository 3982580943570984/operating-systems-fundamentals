#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include "globals.h"

/* TODO: изменить рабочее состояние терминала (IDLING -> RAISING_WATER_LEVEL -> FIXING_WATER_LEVEL -> IDLING) */
/* TODO: изменить состояние створов (открыта сторона -> закрыты оба -> открыта другая сторона) */
/* TODO: эмулировать поведение терминала в реальном времени с использованием sleep() (RAISING_WATER_LEVEL - 80%, FIXING_WATER_LEVEL - 20%) */
void * raise_the_terminal(void * arg) {
  printf("raise_the_terminal\n");
  struct terminal_t * terminal = (struct terminal_t *)arg;

  terminal->lower_site_opened = false;
  terminal->working_state = RAISING_WATER_LEVEL;
  sleep(((int)(time_for_action * 0.8)));

  terminal->upper_site_opened = true;
  terminal->working_state = FIXING_WATER_LEVEL;
  sleep(((int)(time_for_action * 0.2)));

  terminal->working_state = IDLING;

  return NULL;
}

void * lower_the_terminal(void * arg) {
  printf("lower_the_terminal\n");
  struct terminal_t * terminal = (struct terminal_t *)arg;

  terminal->upper_site_opened = false;
  terminal->working_state = LOWERING_WATER_LEVEL;
  sleep(((int)(time_for_action * 0.8)));

  terminal->lower_site_opened = true;
  terminal->working_state = FIXING_WATER_LEVEL;
  sleep(((int)(time_for_action * 0.2)));

  terminal->working_state = IDLING;

  return NULL;
}

/* TODO: использование потоков для эмуляции работы терминалов */
/* TODO: добавить проверки на ошибки (perror, exit) */
/* TODO: сделать моделирование шлюза случайным, используя цикл */
/* TODO: добавить обработку сигнала выхода (SIGKILL) для закрытия FIFOs */
/* TODO: представляем, что на текущий момент верхнее и нижнее течение полностью используют доступные терминалы,
 *       или соглашаемся генерировать случайное число в диапазоне количества доступных терминалов*/
/* TODO: объединить цикл получения информации об используемых терминалах с запуском работы терминала */
/* TODO: использовать примитив синхронизации для доступа к gateway.terminals */
int main([[maybe_unused]] int argc,[[maybe_unused]] char ** argv)
{
  /* Открываем именованные каналы на чтение */
  int gateway_upstream_fd = open(gateway_upstream_fifo, O_WRONLY);
  int gateway_downstream_fd = open(gateway_downstream_fifo, O_WRONLY);
  int upstream_gateway_fd = open(upstream_gateway_fifo, O_RDONLY);
  int downstream_gateway_fd = open(downstream_gateway_fifo, O_RDONLY);

  /* Моделируем шлюз */
  struct gateway_t gateway = {
    .terminals = {
      [0] = { .working_state = IDLING, .is_working = true, .upper_site_opened = true, .lower_site_opened = false },
      [1] = { .working_state = IDLING, .is_working = true, .upper_site_opened = true, .lower_site_opened = false },
      [2] = { .working_state = IDLING, .is_working = true, .upper_site_opened = false, .lower_site_opened = true },
      [3] = { .working_state = IDLING, .is_working = true, .upper_site_opened = false, .lower_site_opened = true },
      [4] = { .working_state = IDLING, .is_working = true, .upper_site_opened = false, .lower_site_opened = true },
    }
  };

  while (true) {
    /* Производим проверку состояния терминалов */
    int upper_indexes[TERMINALS_COUNT] = {0};
    int lower_indexes[TERMINALS_COUNT] = {0};
    int upper_count = 0;
    int lower_count = 0;
    for (int i = 0; i < TERMINALS_COUNT; ++i) {
      struct terminal_t terminal = gateway.terminals[i];

      /* Терминал должен находиться в рабочем и доступном состоянии */
      printf("terminal.working_state == IDLING = %d\n", terminal.working_state == IDLING);
      if (!terminal.is_working || terminal.working_state != IDLING) {
        continue;
      }

      /* Определяем количество терминалов, доступных вверху и снизу */
      if (terminal.upper_site_opened) {
        upper_indexes[upper_count++] = i;
      } else if (terminal.lower_site_opened) {
        lower_indexes[lower_count++] = i;
      } else {
        printf("%s", "Ошибка: створы терминала имеют невалидное состояние");
      }
    }

    /* Сообщаем верхнему и нижнему течению о номерах терминалов, доступных для использования */
    printf("upper_count = %d\n", upper_count);
    write(gateway_upstream_fd, &upper_count, sizeof(int));
    for (int i = 0; i < upper_count; ++i) {
      write(gateway_upstream_fd, &upper_indexes[i], sizeof(int));
    }

    printf("lower_count = %d\n", lower_count);
    write(gateway_downstream_fd, &lower_count, sizeof(int));
    for (int i = 0; i < lower_count; ++i) {
      write(gateway_downstream_fd, &lower_indexes[i], sizeof(int));
    }

    /* Получаем информацию о желаемых к использованию терминалах */
    int used_upper_count = 0;
    read(upstream_gateway_fd, &used_upper_count, sizeof(int));
    printf("used_upper_count = %d\n", used_upper_count);

    int used_upper_terminals[TERMINALS_COUNT] = {0};
    for (int i = 0; i < used_upper_count; ++i) {
      read(upstream_gateway_fd, &used_upper_terminals[i], sizeof(int));
    }

    int used_lower_count = 0;
    read(downstream_gateway_fd, &used_lower_count, sizeof(int));
    printf("used_lower_count = %d\n", used_lower_count);

    int used_lower_terminals[TERMINALS_COUNT] = {0};
    for (int i = 0; i < used_lower_count; ++i) {
      read(downstream_gateway_fd, &used_lower_terminals[i], sizeof(int));
    }

    /* Запускаем процесс работы терминалов в различных потоках */
    for (int i = 0; i < used_upper_count; ++i) {
      int index = used_upper_terminals[i];
      struct terminal_t * terminal = &gateway.terminals[index];

      pthread_t thread;
      pthread_create(&thread, NULL, lower_the_terminal, (void*)terminal);
    }

    for (int i = 0; i < used_lower_count; ++i) {
      int index = used_lower_terminals[i];
      struct terminal_t * terminal = &gateway.terminals[index];

      pthread_t thread;
      pthread_create(&thread, NULL, raise_the_terminal, (void*)terminal);
    }

    /* Производим проверку состояния шлюза каждую секунду */
    sleep(1);
  }

  /* Закрываем именованные каналы */
  close(gateway_upstream_fd);
  close(gateway_downstream_fd);
  close(upstream_gateway_fd);
  close(downstream_gateway_fd);

  exit(EXIT_SUCCESS);
}
