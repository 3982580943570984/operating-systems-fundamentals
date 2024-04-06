#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "globals.h"

void *raise_terminal(void *arg) {
  struct terminal_t *terminal = (struct terminal_t *)arg;

  pthread_mutex_lock(&gateway_mutex);
  printf("Поднимаем терминал\n");
  terminal->lower_site_opened = false;
  terminal->working_state = RAISING_WATER_LEVEL;
  pthread_mutex_unlock(&gateway_mutex);
  sleep(((unsigned)(time_for_action * 0.8)));

  pthread_mutex_lock(&gateway_mutex);
  terminal->upper_site_opened = true;
  terminal->working_state = FIXING_WATER_LEVEL;
  pthread_mutex_unlock(&gateway_mutex);
  sleep(((unsigned)(time_for_action * 0.2)));

  pthread_mutex_lock(&gateway_mutex);
  terminal->working_state = IDLING;
  pthread_mutex_unlock(&gateway_mutex);

  return NULL;
}

void *handle_terminal_raise(void *arg) {
  struct terminal_t *terminal = (struct terminal_t *)arg;

  /* Поднимаем терминал */
  raise_terminal(terminal);

  /* Сигнализируем о наличии свободного терминала */
  pthread_cond_signal(&terminal_available_cond);

  return NULL;
}

void *lower_terminal(void *arg) {
  struct terminal_t *terminal = (struct terminal_t *)arg;

  pthread_mutex_lock(&gateway_mutex);
  printf("Опускаем терминал\n");
  terminal->upper_site_opened = false;
  terminal->working_state = LOWERING_WATER_LEVEL;
  pthread_mutex_unlock(&gateway_mutex);
  sleep(((unsigned)(time_for_action * 0.8)));

  pthread_mutex_lock(&gateway_mutex);
  terminal->lower_site_opened = true;
  terminal->working_state = FIXING_WATER_LEVEL;
  pthread_mutex_unlock(&gateway_mutex);
  sleep(((unsigned)(time_for_action * 0.2)));

  pthread_mutex_lock(&gateway_mutex);
  terminal->working_state = IDLING;
  pthread_mutex_unlock(&gateway_mutex);

  return NULL;
}

void *handle_terminal_lower(void *arg) {
  struct terminal_t *terminal = (struct terminal_t *)arg;

  /* Поднимаем терминал */
  lower_terminal(terminal);

  /* Сигнализируем о наличии свободного терминала */
  pthread_cond_signal(&terminal_available_cond);

  return NULL;
}

void *handle_terminal_raise_and_lower(void *arg) {
  struct terminal_t *terminal = (struct terminal_t *)arg;

  /* Поднимаем терминал */
  raise_terminal(terminal);

  /* Опускаем терминал */
  lower_terminal(terminal);

  /* Сигнализируем о наличии свободного терминала */
  pthread_cond_signal(&terminal_available_cond);

  return NULL;
}

void *handle_terminal_lower_and_raise(void *arg) {
  struct terminal_t *terminal = (struct terminal_t *)arg;

  /* Опускаем терминал */
  lower_terminal(terminal);

  /* Поднимаем терминал */
  raise_terminal(terminal);

  /* Сигнализируем о наличии свободного терминала */
  pthread_cond_signal(&terminal_available_cond);

  return NULL;
}

void *handle_requests(void *arg) {
  while (true) {
    /* Производим ожидание до сигнала о наличии запроса */
    pthread_mutex_lock(&new_request_mutex);
    while (!requests_from_upstream && !requests_from_downstream) {
      pthread_cond_wait(&new_request_cond, &new_request_mutex);
    }
    pthread_mutex_unlock(&new_request_mutex);

    /* Проверяем наличие свободного терминала */
    pthread_mutex_lock(&gateway_mutex);
    int available_on_upstream = NOT_AVAILABLE, available_on_downstream = NOT_AVAILABLE;
    for (int i = 0; i < TERMINALS_COUNT; ++i) {
      if (available_on_upstream != NOT_AVAILABLE && available_on_downstream != NOT_AVAILABLE) break;

      if (gateway.terminals[i].working_state == IDLING) {
        if (gateway.terminals[i].upper_site_opened && available_on_upstream == NOT_AVAILABLE) {
          available_on_upstream = i;
        } else if (gateway.terminals[i].lower_site_opened && available_on_downstream == NOT_AVAILABLE) {
          available_on_downstream = i;
        }
      }
    }

    /* Производим ожидание до сигнала о наличии свободного терминале */
    while (available_on_upstream == NOT_AVAILABLE && available_on_downstream == NOT_AVAILABLE) {
      pthread_cond_wait(&terminal_available_cond, &gateway_mutex);

      /* После пробуждения снова проверяем наличие свободного терминала */
      for (int i = 0; i < TERMINALS_COUNT; ++i) {
        if (available_on_upstream != NOT_AVAILABLE && available_on_downstream != NOT_AVAILABLE) break;

        if (gateway.terminals[i].working_state == IDLING) {
          if (gateway.terminals[i].upper_site_opened && available_on_upstream == NOT_AVAILABLE) {
            available_on_upstream = i;
          } else if (gateway.terminals[i].lower_site_opened && available_on_downstream == NOT_AVAILABLE) {
            available_on_downstream = i;
          }
        }
      }
    }
    pthread_mutex_unlock(&gateway_mutex); 

    printf("is available on upstream = %d\n", available_on_upstream != NOT_AVAILABLE);
    printf("is available on downstream = %d\n", available_on_downstream != NOT_AVAILABLE);

    pthread_mutex_lock(&gateway_mutex);
    pthread_mutex_lock(&new_request_mutex);
    struct terminal_t *available_terminal = NULL;
    if (requests_from_upstream && requests_from_downstream) {
      /* Обработка запроса от верхнего и нижнего течения */

      pthread_t request_from_upstream_and_downstream_thread;
      if (available_on_upstream != NOT_AVAILABLE && available_on_downstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_downstream];
        pthread_create(&request_from_upstream_and_downstream_thread, NULL,
                       handle_terminal_raise_and_lower,
                       (void *)available_terminal);
      } else if (available_on_upstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_upstream];
        pthread_create(&request_from_upstream_and_downstream_thread, NULL,
                       handle_terminal_lower_and_raise,
                       (void *)available_terminal);
      } else if (available_on_downstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_downstream];
        pthread_create(&request_from_upstream_and_downstream_thread, NULL,
                       handle_terminal_raise_and_lower,
                       (void *)available_terminal);
      }
      pthread_detach(request_from_upstream_and_downstream_thread);

      --requests_from_upstream;
      --requests_from_downstream;
    } else if (requests_from_upstream) {
      /* Обработка запроса от верхнего течения */

      pthread_t request_from_upstream_thread;
      if (available_on_upstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_upstream];
        pthread_create(&request_from_upstream_thread, NULL,
                       handle_terminal_lower,
                       (void *)available_terminal);
      } else if (available_on_downstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_downstream];
        pthread_create(&request_from_upstream_thread, NULL, handle_terminal_raise_and_lower,
                       (void *)available_terminal);
      }
      pthread_detach(request_from_upstream_thread);

      --requests_from_upstream;
    } else if (requests_from_downstream) {
      /* Обработка запроса от нижнего течения */

      pthread_t request_from_downstream_thread;
      if (available_on_downstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_downstream];
        pthread_create(&request_from_downstream_thread, NULL, handle_terminal_raise,
                       (void *)available_terminal);
      } else if (available_on_upstream != NOT_AVAILABLE) {
        available_terminal = &gateway.terminals[available_on_upstream];
        pthread_create(&request_from_downstream_thread, NULL,
                       handle_terminal_lower_and_raise,
                       (void *)available_terminal);
      }
      pthread_detach(request_from_downstream_thread);

      --requests_from_downstream;
    }
    pthread_mutex_unlock(&new_request_mutex);
    pthread_mutex_unlock(&gateway_mutex);
  }

  return NULL;
}

void *handle_upstream_signals(void *arg) {
  /* Открываем именованные каналы на чтение */
  int upstream_gateway_fd = open(upstream_gateway_fifo, O_RDONLY);

  while (true) {
    /* Ожидаем сигнал о необходимости в терминале на верхнем течении*/
    int terminal_required = 0;
    read(upstream_gateway_fd, &terminal_required, sizeof(int));

    /* Если терминал не требуется, переходим на следующую итерацию ожидания */
    if (!terminal_required)
      continue;

    /* Изменяем количество запросов на терминал для верхнего течения */
    pthread_mutex_lock(&new_request_mutex);
    ++requests_from_upstream;
    pthread_mutex_unlock(&new_request_mutex);

    /* Сообщаем о новом запросе потоку для обработки запросов */
    pthread_cond_signal(&new_request_cond);
  }

  /* Закрываем именованные каналы */
  close(upstream_gateway_fd);

  return NULL;
}

void *handle_downstream_signals(void *arg) {
  /* Открываем именованный канал на чтение */
  int downstream_gateway_fd = open(downstream_gateway_fifo, O_RDONLY);

  while (true) {
    /* Ожидаем сигнал о необходимости в терминале */
    int terminal_required = 0;
    read(downstream_gateway_fd, &terminal_required, sizeof(int));

    /* Если терминал не требуется, переходим на следующую итерацию ожидания */
    if (!terminal_required)
      continue;

    /* Изменяем количество запросов на терминал для нижнего течения */
    pthread_mutex_lock(&new_request_mutex);
    ++requests_from_downstream;
    pthread_mutex_unlock(&new_request_mutex);

    /* Сообщаем о новом запросе потоку для обработки запросов */
    pthread_cond_signal(&new_request_cond);
  }

  /* Закрываем именованный канал */
  close(downstream_gateway_fd);

  return NULL;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  /* Поток для обработки запросов от верхнего течения */
  pthread_t upstream_thread;
  pthread_create(&upstream_thread, NULL, handle_upstream_signals, NULL);

  /* Поток для обработки запросов от нижнего течения */
  pthread_t downstream_thread;
  pthread_create(&downstream_thread, NULL, handle_downstream_signals, NULL);

  /* Поток для обработки запросов */
  pthread_t requests_thread;
  pthread_create(&requests_thread, NULL, handle_requests, NULL);

  pthread_join(upstream_thread, NULL);
  pthread_join(downstream_thread, NULL);
  pthread_join(requests_thread, NULL);

  pthread_cond_destroy(&new_request_cond);
  pthread_cond_destroy(&terminal_available_cond);

  pthread_mutex_destroy(&new_request_mutex);
  pthread_mutex_destroy(&gateway_mutex);

  exit(EXIT_SUCCESS);
}
