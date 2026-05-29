#include "../include/ipc.h"
#include "msg_queues.c"
// #include "shared_memory.c"
#include "staff.c"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
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
  int simulation_done = 0;
  int msqid_c2s = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_C2S);
  int msqid_s2c = getmsqid(TRATTORIA_FTOK_PATH, PROJ_MSG_S2C);
  int semid =
      semget(ftok(TRATTORIA_FTOK_PATH, PROJ_SEM), SEM_NSEMS, S_IRUSR | S_IWUSR);
  key_t kitchen_key = ftok(TRATTORIA_FTOK_PATH, PROJ_KITCHEN);
  key_t diningroom_key = ftok(TRATTORIA_FTOK_PATH, PROJ_DININGROOM);
  key_t blackboard_key = ftok(TRATTORIA_FTOK_PATH, PROJ_BLACKBOARD);
  key_t cashdesk_key = ftok(TRATTORIA_FTOK_PATH, PROJ_CASHDESK);
  shm_kitchen_t *kitchen = get_shm_kitchen(kitchen_key);
  shm_diningroom_t *diningroom = get_shm_diningroom(diningroom_key);
  shm_blackboard_t *blackboard = get_shm_blackboard(blackboard_key);
  shm_cashdesk_t *cashdesk = get_shm_cashdesk(cashdesk_key);
  pthread_t threads[MAX_STAFF];
  staff_args_t staff_args[MAX_STAFF];

  char students[1][STUDENTID_MAXLEN] = {"VR518120"};

  msg_welcome_t welcome =
      sendwelcome(msqid_c2s, msqid_s2c, 1, students, STRATEGY_PROFIT);

  staff_role_assignment roles = assign_roles(welcome.staff_n, welcome.staff);

  recv_instance(msqid_s2c);

  for (int i = 0; i < welcome.staff_n; i++) {
    staff_args[i].id = i;
    staff_args[i].member = &welcome.staff[i];
    staff_args[i].kitchen = kitchen;
    staff_args[i].dining = diningroom;
    staff_args[i].board = blackboard;
    staff_args[i].cashdesk = cashdesk;
    staff_args[i].roles = &roles;
    staff_args[i].semid = semid;
    staff_args[i].simulation_done = simulation_done;

    pthread_create(&threads[i], NULL, staff_loop, (void *)&staff_args[i]);
  }

  recv_instance_done(msqid_s2c);

  for (int i = 0; i < welcome.staff_n; i++) {
    staff_args[i].simulation_done = 1;
    pthread_join(threads[i], NULL);
  }

  recv_end(msqid_s2c);

  return 0;
}
