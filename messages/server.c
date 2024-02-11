#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

struct msgbuf {
  long mtype;
  char mtext[256];
} message;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  key_t key_in = ftok("/tmp/msg_key_path1", 1);
  key_t key_out = ftok("/tmp/msg_key_path2", 1);

  int msg_id_in = msgget(key_in, 0777 | IPC_CREAT);
  int msg_id_out = msgget(key_out, 0777 | IPC_CREAT);

  int running = 1;
  while (running) {
    printf("Введите сообщение: ");
    fgets(message.mtext, sizeof(message.mtext), stdin);
    message.mtype = 1;

    if (msgsnd(msg_id_out, &message, sizeof(message.mtext), 0) == -1) {
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }

    int receiving = 1;
    while (receiving) {
      if (msgrcv(msg_id_in, &message, sizeof(message.mtext), 0, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
      } else {
        receiving = 0;
          printf("Получено сообщение: %s", message.mtext);
      }
    }
  }

  exit(EXIT_SUCCESS);
}

