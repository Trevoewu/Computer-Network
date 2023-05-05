
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
extern void tolayer2(struct rtpkt packet);
extern void creatertpkt(struct rtpkt *initrtpkt, int srcid, int destid,
                        int mincosts[]);
extern int min(int a, int b);
extern int findMin(int arr[], int len);
// entry [i,j] in the distance table in node 0 is node 0's currently
// computed cost to node i via direct neighbor j. If 0 is not directly
// connected to j, you can ignore this entry. We will use the convention
// that the integer value 999 is ``infinity.''
//条目[i,j]是节点 0 当前计算的通过直接邻居 j 到节点 i 的成本.
// 如果 0 没有直接连接到 j 则可以忽略此条目。我们将使用整数值 999
// 为“无穷大”的约定。
struct distance_table {
  int costs[4][4];
} dt2;
void printdt2(struct distance_table *dtptr);
/* students to write the following two routines, and maybe some others */

void rtinit2() {
  printf("node 2 initializing-------------\n");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      dt2.costs[i][j] = 999;
    }
  }
  dt2.costs[1][2] = 1;
  dt2.costs[0][2] = 3;
  dt2.costs[3][2] = 2;
  dt2.costs[2][2] = 0;
  int minCost[4];
  for (int raw = 0; raw < 4; raw++) {
    minCost[raw] = dt2.costs[raw][2];
  }
  struct rtpkt packet;
  for (int i = 0; i < 4; i++) {
    if (i != 2) {
      creatertpkt(&packet, 2, i, minCost);
      tolayer2(packet);
    }
  }
}

void rtupdate2(rcvdpkt) struct rtpkt *rcvdpkt;
{
  int oldCost[4];
  for (int i = 0; i < 4; i++) {
    oldCost[i] = findMin(dt2.costs[i], 4);
  }
  printf("node 2 recv advertisement from %d\n", rcvdpkt->sourceid);
  for (int i = 0; i < 4; i++) {
    dt2.costs[i][rcvdpkt->sourceid] =
        min(dt2.costs[i][rcvdpkt->sourceid],
            rcvdpkt->mincost[i] + dt2.costs[rcvdpkt->sourceid][2]);
  }
  printdt2(&dt2);
  struct rtpkt adv;
  for (int i = 0; i < 4; i++) {
    adv.mincost[i] = findMin(dt2.costs[i], 4);
  }
  int flag = 0;
  for (int i = 0; i < 4; i++) {
    if (oldCost[i] != adv.mincost[i]) {
      flag = 1;
    }
  }
  if (flag) {
    adv.sourceid = 2;
    adv.destid = 1;
    tolayer2(adv);
    adv.destid = 3;
    tolayer2(adv);
  }
}

void printdt2(dtptr) struct distance_table *dtptr;

{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][1],
         dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][1],
         dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][0], dtptr->costs[3][1],
         dtptr->costs[3][3]);
}
