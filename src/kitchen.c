// #include "ipc.h"
#include "../include/ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

void checkkitchen(key_t key) {
  printf("SHM KITCHEN\n");

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
}
