#ifndef GLOBALS_H
#define GLOBALS_H

#include <sys/types.h>

typedef enum terminal_working_state {
  LOWERING_WATER_LEVEL,
  RAISING_WATER_LEVEL,
  FIXING_WATER_LEVEL,
  IDLING
} terminal_working_state;

struct terminal_t {
  terminal_working_state working_state;
  bool is_working;
  bool upper_site_opened;
  bool lower_site_opened;
};

enum { TERMINALS_COUNT = 5 };

struct gateway_t {
  struct terminal_t terminals[TERMINALS_COUNT];
};

/* Определяем расположение файлов для именованных каналов */
const char * gateway_upstream_fifo = "/tmp/gateway_upstream";
const char * gateway_downstream_fifo = "/tmp/gateway_downstream";
const char * upstream_gateway_fifo = "/tmp/upstream_gateway";
const char * downstream_gateway_fifo = "/tmp/downstream_gateway";

/* Определяем время на перемещение терминала */
const uint time_for_action = 10;

#endif // !GLOBALS_H
