#include "../include/ipc.h"
#include "shared_memory.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
  int waiter;
  int cook;
  int helper;
  int cashier;
} staff_role_assignment;

typedef struct {
  int id;
  staff_member_t *member;
  shm_kitchen_t *kitchen;
  shm_diningroom_t *dining;
  shm_blackboard_t *board;
  shm_cashdesk_t *cashdesk;
  staff_role_assignment *roles;
  int semid;
  int simulation_done;
} staff_args_t;

staff_role_assignment assign_roles(int staff_n, staff_member_t staff[staff_n]) {
  staff_role_assignment roles = {-1, -1, -1, -1};
  int max_waiter = 0;
  int max_cook = 0;
  int max_helper = 0;
  int max_cashier = 0;

  for (int i = 0; i < staff_n; i++) {
    if (staff[i].skills[0] > max_waiter) {
      max_waiter = staff[i].skills[0];
      roles.waiter = i;
    }
    if (staff[i].skills[1] > max_cook) {
      max_cook = staff[i].skills[1];
      roles.cook = i;
    }
    if (staff[i].skills[2] > max_helper) {
      max_helper = staff[i].skills[2];
      roles.helper = i;
    }
    if (staff[i].skills[3] > max_cashier) {
      max_cashier = staff[i].skills[3];
      roles.cashier = i;
    }
  }

  return roles;
}

void *staff_loop(void *void_args) {
  staff_args_t *args = (staff_args_t *)void_args;

  while (args->simulation_done == 0) {
    if (args->roles->waiter == args->id) {
      for (int i = 0; i < args->board->tables_n; i++) {
        if (args->dining->tables[i].state == TABLE_TAKEN &&
            args->board->tables[i].waiter == -1) {
          sem_wait_blackboard(args->semid);
          args->board->tables[i].waiter = args->id;
          sem_post_blackboard(args->semid);
        } else if (args->dining->tables[i].state == TABLE_SERVED &&
                   args->board->tables[i].waiter != -1) {
          sem_wait_blackboard(args->semid);
          args->board->tables[i].waiter = -1;
          sem_post_blackboard(args->semid);
        } else if (args->dining->tables[i].state == TABLE_FREED &&
                   args->board->tables[i].cleaner == -1) {
          sem_wait_blackboard(args->semid);
          args->board->tables[i].cleaner = args->id;
          sem_post_blackboard(args->semid);
        } else if (args->cashdesk->pending_payments != 0 &&
                   args->board->cashier == -1) {
          sem_wait_blackboard(args->semid);
          args->board->cashier = args->id;
          sem_post_blackboard(args->semid);
        }
      }
    } else if (args->roles->cook == args->id) {
      if (args->kitchen->pending_orders != 0) {
        sem_wait_blackboard(args->semid);
        args->board->cook = args->id;
        sem_post_blackboard(args->semid);
      } else {
        sem_wait_blackboard(args->semid);
        args->board->dishwasher = args->id;
        sem_post_blackboard(args->semid);
      }
    }

    usleep(10 * 1000);
  }

  return NULL;
}
