#include "ipc.h"
#include "stdio.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include <stdlib.h>
#include <string.h>
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

  int msqid_s2c = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_S2C);
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
  }

  printf("\n\n\n");
  printf("SHM KITCHEN\n");

  key_t key = ftok(TRATTORIA_FTOK_PATH, PROJ_KITCHEN);

  int shmid = shmget(key, sizeof(shm_kitchen_t), S_IRUSR | S_IWUSR);
  printf("SHMID: %d\n", shmid);

  shm_kitchen_t *ptr = (shm_kitchen_t *)shmat(shmid, NULL, SHM_RDONLY);
  if (ptr == (void *)-1) {
    printf("shmat failed\n");
    exit(-1);
  }

  printf("table_n: %d\n", ptr->tables_n);
  printf("pending_orders: %d\n", ptr->pending_orders);

  if (shmdt(ptr) == -1) {
    printf("shmdt failed\n");
    exit(-1);
  }

  return 0;
}
