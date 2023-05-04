// todo:
// problem1: 无法收敛, 修改为当distance table 不改变时, 不发送通告
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
} dt3;
void printdt3(dtptr) struct distance_table *dtptr;

{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][2]);
}
/* students to write the following two routines, and maybe some others */

void rtinit3() {
  printf("node 3 initializing-------------\n");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      dt3.costs[i][j] = 999;
    }
  }
  dt3.costs[0][3] = 7;
  dt3.costs[2][3] = 2;
  dt3.costs[3][3] = 3;
  int minCost[4];
  for (int raw = 0; raw < 4; raw++) {
    minCost[raw] = dt3.costs[raw][3];
  }
  struct rtpkt packet;
  for (int i = 0; i < 3; i++) {
    if (i != 1) {
      creatertpkt(&packet, 3, i, minCost);
      tolayer2(packet);
    }
  }
}

void rtupdate3(rcvdpkt) struct rtpkt *rcvdpkt;
{
  int oldCost[4];
  for (int i = 0; i < 4; i++) {
    oldCost[i] = findMin(dt3.costs[i], 4);
  }
  printf("node 3 recv advertisement from %d\n", rcvdpkt->sourceid);
  for (int i = 0; i < 4; i++) {
    dt3.costs[i][rcvdpkt->sourceid] =
        min(dt3.costs[i][rcvdpkt->sourceid],
            rcvdpkt->mincost[i] + dt3.costs[rcvdpkt->sourceid][3]);
  }
  printf("current state:\n");
  printdt3(&dt3);
  struct rtpkt adv;
  for (int i = 0; i < 4; i++) {
    adv.mincost[i] = findMin(dt3.costs[i], 4);
  }
  int flag = 0;
  for (int i = 0; i < 4; i++) {
    if (oldCost[i] != adv.mincost[i]) {
      flag = 1;
    }
  }
  if (flag) {
    adv.sourceid = 3;
    adv.destid = 0;
    tolayer2(adv);
    adv.destid = 2;
    tolayer2(adv);
  }
}
