/**
 * TODO: implementation of the multilevel feedback queue scheduling
 */
#include "../lib/kqueue.h"
#include <assert.h>
#include <stdio.h>

#define DOWNGRADE_TIME  30
#define UPGRADE_TIME    20
#define NO_PROCESSES    5
/**
 *        ready queue all are Round Robin
 *     ==========================
 *            Q1 (pri) 1
 *  ->  --------------------   Qt = 2 unit
 *      --------------------    >>-------------------->> | CPU |
 *            Q2 (pri) 2                                 |     |
 *      --------------------   Qt = 4 unit >----------------> ^
 *      --------------------                                  |
 *            Q3 (pri) 3                                      |
 *      --------------------   Qt = 8 unit >-------------------
 *      --------------------
 * 
 *     ==========================
 * 
 * NOTE: if the process has exceeded the Qt of that queue and not yet finished 
 *        then: moves to below queue
 *      also then specific time as passed it cannot remain in same queue even if O/P
 *        for o/p within Qt it will remain in the queue
 * 
 *      if the process has been on the lower queue for a long time it 
 *        needs to be upgraded to the first queue after certain time
 */

extern u_int64_t CLK;

struct readyQueue 
{
  struct __LinkListHeaders    *Q1;
  int priQ1;
  int Qt1;

  struct __LinkListHeaders    *Q2;
  int priQ2;
  int Qt2;

  struct __LinkListHeaders    *Q3;
  int priQ3;
  int Qt3;
};

void printOutLoudPatQueue(struct __LinkListHeaders *ptr) {
  struct __LinkList *iter = ptr->front;
  while (iter!=BLACKHOLE) {
    printf("%p -> ", iter->data_ptr);
    iter = iter->next;
  }
  printf("[LAST]\n---\n");
}

void currStateOfQueue(struct readyQueue *rQue) {
  printf("Queue [ 1 ]\npri: %d, Quantum Time: %d\n",rQue->priQ1, rQue->Qt1);
  printOutLoudPatQueue(rQue->Q1);
  printf("Queue [ 2 ]\npri: %d, Quantum Time: %d\n", rQue->priQ2, rQue->Qt2);
  printOutLoudPatQueue(rQue->Q2);
  printf("Queue [ 3 ]\npri: %d, Quantum Time: %d\n",rQue->priQ3, rQue->Qt3);
  printOutLoudPatQueue(rQue->Q3);
}

/////// SETUP for the readyQueue  ///////
int initReadyQueue(struct readyQueue **pTable) {
  CLK = 1;
  *pTable = (struct readyQueue *) malloc(sizeof(struct readyQueue));
  assert(*pTable);

  (*pTable)->Q1 = initKQueue();
  assert((*pTable)->Q1);
  (*pTable)->priQ1 = 1;
  (*pTable)->Qt1 = 2;

  (*pTable)->Q2 = initKQueue();
  assert((*pTable)->Q2);
  (*pTable)->priQ2 = 2;
  (*pTable)->Qt2 = 4;

  (*pTable)->Q3 = initKQueue();
  assert((*pTable)->Q3);
  (*pTable)->priQ3 = 3;
  (*pTable)->Qt3 = 8;

  return 0;
}

int schedulerRoundRobin(struct proc ***processTT, struct readyQueue **queueTable){

  // check which can be loaded to the queue
  for (int i = 0; i < NO_PROCESSES; i++) {

    switch((*processTT)[i]->state) {
      case RUNNING:
        printf("ITS a 'RUNNING' PROC\n");
        break;

      case RUNNABLE:
        printf("ITS a 'RUNNABLE' PROC\n");
        (*queueTable)->Q1 = __push_rear((*queueTable)->Q1, (*processTT)[i]);
        break;
      
      case WAITING:
        printf("ITS a 'WAITING' PROC\n");
        break;

      case EMBRYO:
        printf("ITS a 'EMBRYO' PROC\n");
        break;

      case ZOMBIE:
        printf("ITS a 'ZOMBIE' PROC\n");
        break;

      case SLEEPING:
        printf("ITS a 'SLEEPING' PROC\n");
        break;
      
      default:
        fprintf(stderr, "Internal err 0x000\n");
    }
  }
  currStateOfQueue(*queueTable);

  return 0;
}


////////  TESTING  ////////
void buildProcessTable(struct proc ***processTT){
  for (int i = 0;i <NO_PROCESSES; i++) {
    printf("Enter the arr and the burst times for process_[ %d ]\n", i);
    uint xx, yy;
    scanf("%d %d", &xx, &yy);
    (*processTT)[i]->arrivalTime = xx;
    (*processTT)[i]->burstTime = yy;
  }
 
}

void turnONQueue(struct proc ***processTT){
  *processTT = (struct proc **)malloc(sizeof(struct proc *) * NO_PROCESSES);
  assert(*processTT);
  char namePP[16] = {'\0'};
  for (int i = 0; i < NO_PROCESSES; i++) {
    namePP[0] = (char)(65 + i);
    namePP[1] = (char)(65 + i + 1);
    (*processTT)[i] = initProcess(namePP, i + 1);
  }
  
}