#include "../include/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

shm_kitchen_t *get_shm_kitchen(int key) {
  int shmid = shmget(key, sizeof(shm_kitchen_t), S_IRUSR | S_IWUSR);
  shm_kitchen_t *ptr = (shm_kitchen_t *)shmat(shmid, NULL, SHM_RDONLY);

  if (ptr == (void *)-1) {
    printf("shmat failed\n");
    exit(-1);
  }

  // "rilascia" il segmento di memoria,
  // ma forse dovremmo fare una funzione che rilasci tutto alla fine
  /*if (shmdt(ptr) == -1) {
    printf("shmdt failed\n");
    exit(-1);
  }*/

  return ptr;
}

shm_diningroom_t *get_shm_diningroom(int key) {
  int shmid = shmget(key, sizeof(shm_diningroom_t), S_IRUSR | S_IWUSR);
  shm_diningroom_t *ptr = (shm_diningroom_t *)shmat(shmid, NULL, SHM_RDONLY);

  if (ptr == (void *)-1) {
    printf("shmat failed\n");
    exit(-1);
  }

  return ptr;
}

shm_blackboard_t *get_shm_blackboard(int key) {
  int shmid = shmget(key, sizeof(shm_blackboard_t), S_IRUSR | S_IWUSR);
  shm_blackboard_t *ptr = (shm_blackboard_t *)shmat(shmid, NULL, SHM_RDONLY);

  if (ptr == (void *)-1) {
    printf("shmat failed\n");
    exit(-1);
  }

  return ptr;
}

shm_cashdesk_t *get_shm_cashdesk(int key) {
  int shmid = shmget(key, sizeof(shm_cashdesk_t), S_IRUSR | S_IWUSR);
  shm_cashdesk_t *ptr = (shm_cashdesk_t *)shmat(shmid, NULL, SHM_RDONLY);

  if (ptr == (void *)-1) {
    printf("shmat failed\n");
    exit(-1);
  }

  return ptr;
}
