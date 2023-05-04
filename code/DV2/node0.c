
#include <stdio.h>
extern struct rtpkt {
  int sourceid;   /* id of sending router sending this pkt */
  int destid;     /* id of router to which pkt being sent
                     (must be an immediate neighbor) */
  int mincost[4]; /* min cost to node 0 ... 3 */
};
extern int TRACE;
extern int YES;
extern int NO;
struct distance_table {
  int costs[4][4];
} dt0;
void printdt0(struct distance_table *dtptr);
extern void tolayer2(struct rtpkt packet);
extern void creatertpkt(struct rtpkt *initrtpkt, int srcid, int destid,
                        int mincosts[]);
extern int min(int a, int b) { return a > b ? b : a; }
int findMin(int arr[], int len) {
  int min = arr[0];
  for (int i = 0; i < len; i++) {
    if (arr[i] < min) {
      min = arr[i];
    }
  }
  return min;
}
// entry [i,j] in the distance table in node 0 is node 0's currently
// computed cost to node i via direct neighbor j. If 0 is not directly
// connected to j, you can ignore this entry. We will use the convention
// that the integer value 999 is ``infinity.''
//条目[i,j]是节点 0 当前计算的通过直接邻居 j 到节点 i 的成本.
// 如果 0 没有直接连接到 j 则可以忽略此条目。我们将使用整数值 999
// 为“无穷大”的约定。

/* students to write the following two routines, and maybe some others */
void routerInit(int router, struct distance_table DTable, int neighborList[],
                int neighborLen) {
  printf("router %d is initializing", router);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      DTable.costs[i][j] = 999;
    }
  }
}
void rtinit0() {
  printf("node 0 initializing-------------\n");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      dt0.costs[i][j] = 999;
    }
  }
  dt0.costs[0][0] = 0;
  dt0.costs[1][0] = 1;
  dt0.costs[2][0] = 3;
  dt0.costs[3][0] = 7;
  int minCost[4];
  for (int raw = 0; raw < 4; raw++) {
    minCost[raw] = dt0.costs[raw][0];
  }
  struct rtpkt packet;
  // send adv to node 1,2,3
  for (int i = 1; i < 4; i++) {
    creatertpkt(&packet, 0, i, minCost);
    tolayer2(packet);
  }
}

void rtupdate0(rcvdpkt) struct rtpkt *rcvdpkt;
{
  int oldCost[4];
  for (int i = 0; i < 4; i++) {
    oldCost[i] = findMin(dt0.costs[i], 4);
  }
  printf("node 0 recv advertisement from %d\n", rcvdpkt->sourceid);
  for (int i = 0; i < 4; i++) {
    dt0.costs[i][rcvdpkt->sourceid] =
        min(dt0.costs[i][rcvdpkt->sourceid],
            rcvdpkt->mincost[i] + dt0.costs[rcvdpkt->sourceid][0]);
  }
  printf("current state:\n");
  printdt0(&dt0);

  struct rtpkt adv;
  for (int i = 0; i < 4; i++) {
    adv.mincost[i] = findMin(dt0.costs[i], 4);
  }
  int flag = 0;
  for (int i = 0; i < 4; i++) {
    if (oldCost[i] != adv.mincost[i]) {
      flag = 1;
    }
  }
  if (flag) {
    adv.sourceid = 0;
    for (int i = 1; i < 4; i++) {
      adv.destid = i;
      tolayer2(adv);
    }
  }
}

void printdt0(dtptr) struct distance_table *dtptr;

{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n", dtptr->costs[1][1], dtptr->costs[1][2],
         dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n", dtptr->costs[2][1], dtptr->costs[2][2],
         dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][1], dtptr->costs[3][2],
         dtptr->costs[3][3]);
}

void linkhandler0(linkid, newcost) int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */

{}
