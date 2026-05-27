#include "../include/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

msg_instance_t *recvinstance(int msqid_s2c) {
  typedef struct {
    long mtype;
    char payload[1024];
  } generic_msg_t;
  generic_msg_t raw;
  size_t buffer_size = sizeof(msg_instance_t) - sizeof(long);

  if (msgrcv(msqid_s2c, &raw, buffer_size, 0, 0) == -1) {
    printf("msgrcv failed\n");
  } else if (raw.mtype == MSGTYPE_INSTANCE) {
    msg_instance_t *w = (msg_instance_t *)&raw;
    printf("MSG INSTANCE\n");
    printf("instance_id: %d\n", w->instance_id);
    printf("speed: %d\n", w->speed);
    printf("families_n: %d\n", w->families_n);
    if (w->strategy == STRATEGY_NONE) {
      printf("straregy: NONE\n");
    } else if (w->strategy == STRATEGY_PROFIT) {
      printf("strategy: PROFIT\n");
    } else {
      printf("strategy: REPUTATION\n");
    }
    return w;
  }

  printf("\n\n\n");

  return NULL;
}
