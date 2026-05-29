#include "../include/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
  long mtype;
  char payload[1024];
} generic_msg_t;
generic_msg_t raw;

msg_welcome_t sendwelcome(int msqid_c2s, int msqid_s2c, int studentid_n,
                          char students[studentid_n][STUDENTID_MAXLEN],
                          strategy_t strategy) {
  size_t mSize = sizeof(msg_hello_t) - sizeof(long);
  msg_hello_t m;
  m.mtype = MSGTYPE_HELLO;
  m.pid = getpid();
  m.studentid_n = studentid_n;
  for (int i = 0; i < studentid_n; i++) {
    strncpy(m.studentids[i], students[i], STUDENTID_MAXLEN - 1);
    m.studentids[i][STUDENTID_MAXLEN - 1] = '\0';
  }
  m.has_strategy = TR_TRUE;
  m.strategy = strategy;

  if (msgsnd(msqid_c2s, &m, mSize, 0) == -1) {
    printf("msgsnd failed\n");
    exit(-1);
  }

  size_t buffer_size = sizeof(msg_welcome_t) - sizeof(long);

  if (msgrcv(msqid_s2c, &raw, buffer_size, 0, 0) == -1) {
    printf("msgrcv failed\n");
    exit(-1);
  }

  if (raw.mtype == MSGTYPE_WELCOME) {
    msg_welcome_t *w = (msg_welcome_t *)&raw;
    return *w;
  } else if (raw.mtype == MSGTYPE_ERROR) {
    msg_error_t *e = (msg_error_t *)&raw;

    printf("Error: %s\n", e->message);
    exit(-1);
  }

  msg_welcome_t null;

  return null;
}

msg_instance_t *recv_instance(int msqid_s2c) {
  size_t buffer_size = sizeof(msg_instance_t) - sizeof(long);

  if (msgrcv(msqid_s2c, &raw, buffer_size, 0, 0) == -1) {
    printf("msgrcv failed\n");
    exit(-1);
  } else if (raw.mtype == MSGTYPE_INSTANCE) {
    msg_instance_t *w = (msg_instance_t *)&raw;
    return w;
  }

  return NULL;
}
