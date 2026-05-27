#include "../include/ipc.h"
#include "msg_queues.c"
#include "shared_memory.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

int getmsqid(char *path, int id) {
  key_t key = ftok(path, id);
  if (key == -1) {
    printf("ftok failed\n");
    exit(-1);
  }

  int msqid = msgget(key, S_IRUSR | S_IWUSR);
  if (msqid == -1) {
    printf("msqid failed\n");
    exit(-1);
  }

  return msqid;
}

int main(int argc, char **argv) {
  int msqid_c2s = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_C2S);
  int msqid_s2c = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_S2C);
  key_t kitchen_key = ftok(TRATTORIA_FTOK_PATH, PROJ_KITCHEN);

  char students[1][STUDENTID_MAXLEN] = {"VR518120"};

  sendwelcome(msqid_c2s, msqid_s2c, 1, students, STRATEGY_PROFIT);

  recv_instance(msqid_s2c);

  return 0;
}
