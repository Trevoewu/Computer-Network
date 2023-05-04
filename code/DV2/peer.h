
#include <malloc/_malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define LINK_CHANGES 0

struct event *eventList = NULL; /* the event list */

/* possible events: */
#define FROM_LAYER2 2
#define LINK_CHANGE 10

float clockTime = 0.000;

/* a advertisement is the packet sent from one routing update process to
   another via the call toLayer3() */
struct advertisement {
  int sourceid;   /* id of sending router sending this pkt */
  int destId;     /* id of router to which pkt being sent
                     (must be an immediate neighbor) */
  int mincost[4]; /* min cost to node 0 ... 3 */
};

struct event {
  float evTime;                 /* event time */
  int evType;                   /* event type code */
  int evEntity;                 /* entity where event occurs */
  struct advertisement *advPtr; /* ptr to packet (if any) assoc w/ this event */
  struct event *prev;
  struct event *next;
};
extern void init0();
extern void update0(struct advertisement *);
extern void update1(struct advertisement *);
extern void update2(struct advertisement *);
extern void update3(struct advertisement *);
extern void linkHandler0(int, int);
extern void linkHandler1(int, int);
extern void routeInit0();
extern void routeInit1();
extern void routeInit2();
extern void routeInit3();
extern int toLayer2(struct advertisement packet);
extern void insertEvent(struct event *);
void createAdv(struct advertisement *initAdv, int srcId, int destId,
               int minCost[]) {
  int i;
  initAdv->sourceid = srcId;
  initAdv->destId = destId;
  for (i = 0; i < 4; i++)
    initAdv->mincost[i] = minCost[i];
}
int min(int a, int b) { return a > b ? b : a; }
struct distance_table {
  int costs[4][4];
} dt;
void update(int srcId, struct advertisement *recAdv) {
  if (recAdv == NULL)
    return;
  // copy adv to distance table
  for (int i = 0; i < 4; i++) {
    dt.costs[recAdv->sourceid][i] = recAdv->mincost[i];
  }
  // update distance table

  for (int neighbor = 0; neighbor < 4; neighbor++) {
    if (neighbor != srcId) {
      for (int i = 0; i < 4; i++) {
        dt.costs[srcId][i] =
            min(dt.costs[srcId][i],
                dt.costs[neighbor][i] + dt.costs[srcId][neighbor]);
      }
    }
  }
}
void forwardAdv(int srcId, struct distance_table dt, int neighbor[]) {
  struct advertisement *initAdv;
  for (int i = 0; i < 4; i++) {
    if (neighbor[i] != -1) {
      printf("sent adv to %d", neighbor[i]);
      createAdv(initAdv, srcId, neighbor[i], dt.costs[srcId]);
      toLayer2(*initAdv);
    }
  }
}