#!/bin/bash

set -e

build_dir="./build/"

gateway_upstream_fifo="/tmp/gateway_upstream"
gateway_downstream_fifo="/tmp/gateway_downstream"
upstream_gateway_fifo="/tmp/upstream_gateway"
downstream_gateway_fifo="/tmp/downstream_gateway"

# Создаем директорию для сборки проекта
if [[ ! -d $build_dir ]]; then
  mkdir -p $build_dir
fi

cd $build_dir

# Производим сборку проекта
cmake .. && make all

# Пересоздаем FIFOs
recreate_fifo() {
  local fifo_path="$1"
  if [[ -p $fifo_path ]]; then
    echo "Removing existing FIFO at $fifo_path"
    sudo rm "$fifo_path"
  fi
  echo "Creating new FIFO at $fifo_path"
  mkfifo -m 0777 "$fifo_path"
}

recreate_fifo $gateway_upstream_fifo
recreate_fifo $gateway_downstream_fifo
recreate_fifo $upstream_gateway_fifo
recreate_fifo $downstream_gateway_fifo

# Запускаем проект
xterm -e "./gateway" &
xterm -e "./upstream" &
xterm -e "./downstream" &
wait
