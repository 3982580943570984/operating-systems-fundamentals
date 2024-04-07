#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>
#include <sys/types.h>

/* Счетчики запросов от верхнего и нижнего течения */
int requests_from_upstream = 0;
int requests_from_downstream = 0;

/* Определяем время на перемещение терминала */
const uint time_for_action = 10;

/* Примитивы синхронизации для обработки запросов */
pthread_cond_t new_request_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t new_request_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Примитивы синхронизации для обработки работы с терминалами */
pthread_cond_t terminal_available_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t gateway_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Расположение файлов для именованных каналов */
const char *upstream_gateway_fifo = "/tmp/upstream_gateway";
const char *downstream_gateway_fifo = "/tmp/downstream_gateway";

typedef enum terminal_working_state {
  LOWERING_WATER_LEVEL,
  RAISING_WATER_LEVEL,
  FIXING_WATER_LEVEL,
  IDLING
} terminal_working_state;

const char *working_state_str[] = {
    "LOWERING_WATER_LEVEL",
    "RAISING_WATER_LEVEL",
    "FIXING_WATER_LEVEL",
    "IDLING",
};

struct terminal_t {
  terminal_working_state working_state;
  bool is_working;
  bool upper_site_opened;
  bool lower_site_opened;
};

enum { TERMINALS_COUNT = 2, NOT_AVAILABLE = -1 };

/* Модель шлюза */
struct gateway_t {
  struct terminal_t terminals[TERMINALS_COUNT];
} gateway = {.terminals = {
                 [0] = {.working_state = IDLING,
                        .is_working = true,
                        .upper_site_opened = true,
                        .lower_site_opened = false},
                 [1] = {.working_state = IDLING,
                        .is_working = true,
                        .upper_site_opened = false,
                        .lower_site_opened = true},
             }};

#endif // !GLOBALS_H
