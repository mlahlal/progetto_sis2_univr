#include "../include/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

void sendwelcome(int msqid_c2s, int msqid_s2c) {
  // int msqid_c2s = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_C2S);
  size_t mSize = sizeof(msg_hello_t) - sizeof(long);
  msg_hello_t m;
  m.mtype = MSGTYPE_HELLO;
  m.pid = getpid();
  m.studentid_n = 1;
  strncpy(m.studentids[0], "VR518120", STUDENTID_MAXLEN - 1);
  m.studentids[0][STUDENTID_MAXLEN - 1] = '\0';
  m.has_strategy = TR_TRUE;
  m.strategy = STRATEGY_PROFIT;

  if (msgsnd(msqid_c2s, &m, mSize, 0) == -1) {
    printf("msgsnd failed\n");
  }

  // int msqid_s2c = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_S2C);
  size_t buffer_size = sizeof(msg_welcome_t) - sizeof(long);
  typedef struct {
    long mtype;
    char payload[1024];
  } generic_msg_t;
  generic_msg_t raw;

  if (msgrcv(msqid_s2c, &raw, buffer_size, 0, 0) == -1) {
    printf("msgrcv failed\n");
  }

  if (raw.mtype == MSGTYPE_WELCOME) {
    msg_welcome_t *w = (msg_welcome_t *)&raw;
    printf("MSG WELCOME\n");
    printf("staff_n: %d\n", w->staff_n);
    printf("tables_n: %d\n", w->tables_n);
    if (w->verify_mode) {
      printf("verify_mode: true\n");
    } else {
      printf("verify_mode: false\n");
    }
    if (w->imposed_strategy == STRATEGY_NONE) {
      printf("straregy: NONE\n");
    } else if (w->imposed_strategy == STRATEGY_PROFIT) {
      printf("strategy: PROFIT\n");
    } else {
      printf("strategy: REPUTATION\n");
    }
  } else if (raw.mtype == MSGTYPE_ERROR) {
    msg_error_t *e = (msg_error_t *)&raw;

    printf("Error: %s\n", e->message);
  }

  printf("\n\n\n");
}
