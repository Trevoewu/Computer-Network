#include "Grapes.h"
#include <stdbool.h>
#include <stdio.h>
/* called from layer 5, passed the data to be sent to other side */
bool lastSentIsACk = 1;
int computePacketChecksum(struct pkt packet) {
  int checksum = 0;
  for (int i = 0; i < 20; i++) {
    checksum += packet.payload[i];
  }
  checksum += checksum + packet.ackNum +packet.seqNum;
  return checksum;
}
void sendAllPacketInWin(struct msg *messageList) {
  // make packet
  for (int j = base; j < next; j++) {
    struct pkt packet;
    packet.ackNum = 0;
    packet.seqNum = j;
    struct msg message = messageList[j];
    // compute checksum and load data to packet
    packet.checksum = 0;
    for (int i = 0; i < 20; i++) {
      packet.payload[i] = message.data[i];
      packet.checksum += message.data[i];
    }
    packet.checksum += packet.checksum + packet.ackNum + packet.seqNum;
    printf("A: send packet seq:%d\n", packet.seqNum);
    toLayer3(0, packet);
    // printf("A:current base:%d next:%d\n", base, j + 1);
  }
}
void A_output(struct msg message) {
  printf("A: receive message from Application layer numMsgs = %d\n", numMsgs -1);
  if (bufferNext < BUFFER_SIZE) {
    sendWin[bufferNext] = message;
    bufferNext++;
    printf("A:  current bufferNext:%d\n", bufferNext);
  } else {
    printf("A:  Buffer(%d)(current bufferNext:%d) is overflow stop and wait\n",
           bufferNext,BUFFER_SIZE);
  }
  if (next < base + WINDOWS_SIZE) {
    if (next == base) {
      startTimer(A, 20.0);
    }
    
    printf("A: status: base:%d, next:%d\n",base,next);
    for (int j = next; j < bufferNext && j - base < WINDOWS_SIZE; j++) {
      struct pkt packet;
      packet.ackNum = 0;
      packet.seqNum = next;
      struct msg message = sendWin[j];
      // compute checksum and load data to packet
      packet.checksum = 0;
      for (int i = 0; i < 20; i++) {
        packet.payload[i] = message.data[i];
        packet.checksum += message.data[i];
      }
      packet.checksum += packet.checksum + packet.ackNum + packet.seqNum;
      packet.seqNum = j;
      printf("A: send seq%d \n",packet.seqNum);
      toLayer3(0, packet);
      printf("A:  current base:%d next:%d\n", base, j + 1);
    }
    next++;
  } else {
    printf("A:  send window %d is overflow\n", next-base+1);
  }
}

void B_output(
    struct msg message) /* need be completed only for extra credit */ {}
/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet) {
  printf("A:  receive  ACK%d\n",packet.ackNum);
  if (notCorrupt(packet) && packet.ackNum >= base) {
    printf("A: this is a expected ACK and check success!\n");
    base = packet.ackNum + 1;
    // printf("A:  current base:%d next:%d\n", base, next);
    if (base == next) {
      stopTimer(0);
    } else {
      stopTimer(A);
      startTimer(A, 20.0);
    }
    struct msg message;
    for (int i = 0; i < 20; i++) {
      message.data[i] = packet.payload[i];
    }
    toLayer5(A, message.data);
  } else {
    printf("A:  this is not expected ACK(rang:%d~%d),actually:%d \n", base,
           next - 1, packet.ackNum);
  }
}
bool notCorrupt(struct pkt packet) {
  int expectedChecksum = computePacketChecksum(packet);
  if (expectedChecksum == packet.checksum) {
    // printf("packet not corrupt!expected checksum:%d, actually checksum %d\n",
          //  expectedChecksum, packet.checksum);
    return true;
  }
  printf("packet corrupt!expected checksum:%d, actually checksum %d\n",
         expectedChecksum, packet.checksum);
  return false;
}
/* called when A's timer goes off */
void A_timerInterrupt() {
  printf("A:  timeout send all packet in send window\n");
  sendAllPacketInWin(sendWin);
  startTimer(A, 20.0);
}
/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
  base = 0;
  next = 0;
}
/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from network layer , when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet) {
  printf("B:  ←receive seq%d\n",packet.seqNum);
  int ackNum = seqNum;
  if (packet.seqNum == ackNum && notCorrupt(packet)) {
    printf("B:  check success!\n");
    struct msg message;
    for (int i = 0; i < 20; i++) {
      message.data[i] = packet.payload[i];
    }
    toLayer5(B, message.data);
    struct pkt ACKpacket;
    ACKpacket.ackNum = seqNum;
    seqNum++;
    ACKpacket.checksum = computePacketChecksum(ACKpacket);
    printf("B→ sent ACK%d to A\n", abs(ACKpacket.ackNum));
    toLayer3(B, ACKpacket);

  } else {
    printf("B:  check fail! This is not expected(%d) packet!  drop it! \n",seqNum);
  }
}

/* called when B's timer goes off */
void B_timerInterrupt() {
  printf("B:  timeout send copy to A sequence:%d\n", copy.seqNum);
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init() {}
int main() {
  struct event *eventPtr;
  struct msg message;
  struct pkt packet;
  int i, j;

  init();
  A_init();
  B_init();

  while (1) {
    eventPtr = evenList; /* get next event to simulate */
    if (eventPtr == NULL)
      goto terminate;
    evenList = evenList->next; /* remove this event from event list */
    if (evenList != NULL)
      evenList->preEven = NULL;
    if (TRACE >= 2) {
      printf("\nEVENT time: %f,", eventPtr->evenTime);
      printf("  type: %d", eventPtr->evenType);
      if (eventPtr->evenType == 0)
        printf(", timerInterrupt  ");
      else if (eventPtr->evenType == 1)
        printf(", from layer5 ");
      else
        printf(", from layer3 ");
      printf(" entity: %d\n", eventPtr->evenEntity);
    }
    time = eventPtr->evenTime; /* update time to next event time */
    if (numMsgs == maxMsg)     // all messages have be sent
      break;                   /* all done with simulation */
    if (eventPtr->evenType == FROM_LAYER5) {
      generate_next_arrival(); /* set up future arrival */
      /* fill in message to give with string of same letter */
      j = numMsgs % 26;
      for (i = 0; i < 20; i++)
        message.data[i] = 97 + j;
      if (TRACE > 2) {
        printf("          MAINLOOP: data given to student: ");
        for (i = 0; i < 20; i++)
          printf("%c", message.data[i]);
        printf("\n");
      }
      numMsgs++;
      if (eventPtr->evenEntity == A)
        A_output(message);
      else //接送方触发事件
        B_output(message);
    } else if (eventPtr->evenType == FROM_LAYER3) {
      packet.seqNum = eventPtr->packetPtr->seqNum;
      packet.ackNum = eventPtr->packetPtr->ackNum;
      packet.checksum = eventPtr->packetPtr->checksum;
      for (i = 0; i < 20; i++)
        packet.payload[i] = eventPtr->packetPtr->payload[i];
      if (eventPtr->evenEntity == A) /* deliver packet by calling */
        A_input(packet);             /* appropriate entity */
      else
        B_input(packet);
      free(eventPtr->packetPtr); /* free the memory for packet */
    }

    else if (eventPtr->evenType == TIMER_INTERRUPT) {
      if (eventPtr->evenEntity == A)
        A_timerInterrupt();
      else
        B_timerInterrupt();
    } else {
      printf("INTERNAL PANIC: unknown event type \n");
    }
    free(eventPtr);
  }

terminate:
  printf(
      " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
      time, numMsgs);
  return 0;
}

void init() /* initialize the simulator */
{
  int i;
  float sum, avg;
  printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
  printf("Enter the number of messages to simulate: ");
  scanf("%d", &maxMsg);
  printf("Enter  packet loss probability [enter 0.0 for no loss]:");
  scanf("%f", &lossPro);
  printf("Enter packet corruption probability [0.0 for no corruption]:");
  scanf("%f", &corruptPro);
  printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
  scanf("%f", &arrivalRate);
  printf("Enter TRACE:");
  scanf("%d", &TRACE);
  srand(999); /* srand() 用来设置 rand() 产生随机数时的随机数种子。*/
  sum = (float)0.0; /* test random number generator for students */
  for (i = 0; i < 1000; i++)
    sum = sum + getRand(); /* getRand() should be uniform in [0,1] */
  avg = sum / (float)1000.0;
  if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n");
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine getRand() in the emulator code. Sorry. \n");
    exit(0);
  }

  numOfToLayer3 = 0;       /* number sent into layer 3 */
  numOfLost = 0;           /* number lost in media */
  numCorrupt = 0;          /* number corrupted by media*/
  time = (float)0.0;       /* initialize time to 0.0 */
  generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* getRand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in the range [0,mmm]        */
/****************************************************************************/
float getRand() {
  double mmm = RAND_MAX;     /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                   /* individual students may need to change mmm */
  x = (float)(rand() / mmm); /* x should be uniform in [0,1] */
  return (x);
}

/********************* EVENT HANDLING ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/
void generate_next_arrival() {
  double x, log(), ceil();
  struct event *evenPtr;
  if (TRACE > 2)
    printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

  x = arrivalRate * getRand() * 2; /* x is uniform on [0,2*arrivalRate] */
                                   /* having mean of arrivalRate        */
  evenPtr = (struct event *)malloc(sizeof(struct event));
  evenPtr->evenTime = (float)(time + x);
  evenPtr->evenType = FROM_LAYER5;
  if (BIDIRECTIONAL && (getRand() > 0.5))
    evenPtr->evenEntity = B;
  else
    evenPtr->evenEntity = A;
  insertEvent(evenPtr);
}

void insertEvent(struct event *p) {
  struct event *q, *qOld;

  if (TRACE > 2) {
    printf("            insertEvent: time is %lf\n", time);
    printf("            insertEvent: future time will be %lf\n", p->evenTime);
  }
  q = evenList;    /* q points to header of list in which p struct inserted */
  if (q == NULL) { /* list is empty */
    evenList = p;
    p->next = NULL;
    p->preEven = NULL;
  } else {
    for (qOld = q; q != NULL && p->evenTime > q->evenTime; q = q->next)
      qOld = q;
    if (q == NULL) { /* end of list */
      qOld->next = p;
      p->preEven = qOld;
      p->next = NULL;
    } else if (q == evenList) { /* front of list */
      p->next = evenList;
      p->preEven = NULL;
      p->next->preEven = p;
      evenList = p;
    } else { /* middle of list */
      p->next = q;
      p->preEven = q->preEven;
      q->preEven->next = p;
      q->preEven = p;
    }
  }
}

void printEvenList() {
  struct event *q;
  /* int i; // Unreferenced local variable removed */
  printf("--------------\nEvent List Follows:\n");
  for (q = evenList; q != NULL; q = q->next) {
    printf("Event time: %f, type: %d entity: %d\n", q->evenTime, q->evenType,
           q->evenEntity);
  }
  printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stopTimer(int AorB) /* A or B is trying to stop timer */
{
  struct event *q; /* ,*qOld; // Unreferenced local variable removed */

  if (TRACE > 2)
    printf("          STOP TIMER: stopping timer at %f\n", time);
  /* for (q=evenList; q!=NULL && q->next!=NULL; q = q->next)  */
  for (q = evenList; q != NULL; q = q->next)
    if ((q->evenType == TIMER_INTERRUPT && q->evenEntity == AorB)) {
      /* remove this event */
      if (q->next == NULL && q->preEven == NULL)
        evenList = NULL;        /* remove first and only event on list */
      else if (q->next == NULL) /* end of list - there is one in front */
        q->preEven->next = NULL;
      else if (q == evenList) { /* front of list - there must be event after */
        q->next->preEven = NULL;
        evenList = q->next;
      } else { /* middle of list */
        q->next->preEven = q->preEven;
        q->preEven->next = q->next;
      }
      free(q);
      return;
    }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void startTimer(int AorB, float increment) /* A or B is trying to stop timer */
{
  struct event *q;
  struct event *evenPtr;
  /* char *malloc(); // malloc redefinition removed */

  if (TRACE > 2)
    printf("          START TIMER: starting timer at %f\n", time);
  /* be nice: check to see if timer is already started, if so, then  warn */
  /* for (q=evenList; q!=NULL && q->next!=NULL; q = q->next)  */
  for (q = evenList; q != NULL; q = q->next)
    if ((q->evenType == TIMER_INTERRUPT && q->evenEntity == AorB)) {
      printf("Warning: attempt to start a timer that is already started\n");
      return;
    }

  /* create future event for when timer goes off */
  evenPtr = (struct event *)malloc(sizeof(struct event));
  evenPtr->evenTime = (float)(time + increment);
  evenPtr->evenType = TIMER_INTERRUPT;
  evenPtr->evenEntity = AorB;
  insertEvent(evenPtr);
}

/************************** toLayer3 ***************/
void toLayer3(int AorB, struct pkt packet) /* A or B is trying to stop timer */
{
  struct pkt *myPacketPtr;
  struct event *evenPtr, *q;
  /* char *malloc(); // malloc redefinition removed */
  float lasTime, x, getRand();
  int i;

  numOfToLayer3++;

  /* simulate losses: */
  if (getRand() < lossPro) {
    numOfLost++;
    if (TRACE > 0)
      printf("          toLayer3: packet being lost\n");
    return;
  }

  /* make a copy of the packet student just gave me since he/she may decide */
  /* to do something with the packet after we return back to him/her */
  myPacketPtr = (struct pkt *)malloc(sizeof(struct pkt));
  myPacketPtr->seqNum = packet.seqNum;
  myPacketPtr->ackNum = packet.ackNum;
  myPacketPtr->checksum = packet.checksum;
  for (i = 0; i < 20; i++)
    myPacketPtr->payload[i] = packet.payload[i];
  if (TRACE > 2) {
    printf("          toLayer3: seq: %d, ack %d, check: %d ",
           myPacketPtr->seqNum, myPacketPtr->ackNum, myPacketPtr->checksum);
    for (i = 0; i < 20; i++)
      printf("%c", myPacketPtr->payload[i]);
    printf("\n");
  }

  /* create future event for arrival of packet at the other side */
  evenPtr = (struct event *)malloc(sizeof(struct event));
  evenPtr->evenType = FROM_LAYER3;      /* packet will pop out from layer3 */
  evenPtr->evenEntity = (AorB + 1) % 2; /* event occurs at other entity */
  evenPtr->packetPtr = myPacketPtr;     /* save ptr to my copy of packet */
  /* finally, compute the arrival time of packet at the other end.
     medium can not reorder, so make sure packet arrives between 1 and 10
     time units after the latest arrival time of packets
     currently in the medium on their way to the destination */
  lasTime = time;
  /* for (q=evenList; q!=NULL && q->next!=NULL; q = q->next) */
  for (q = evenList; q != NULL; q = q->next)
    if ((q->evenType == FROM_LAYER3 && q->evenEntity == evenPtr->evenEntity))
      lasTime = q->evenTime;
  evenPtr->evenTime = lasTime + 1 + 9 * getRand();

  /* simulate corruption: */
  if (getRand() < corruptPro) {
    numCorrupt++;
    if ((x = getRand()) < .75){
      myPacketPtr->payload[0] = 'Z'; /* corrupt payload */
      // printf("toLayer3:: corrupt payload\n");
    }
    else if (x < .875){
      myPacketPtr->seqNum = 999999;
      // printf("toLayer3: corrupt seqNum\n");
    }
    else{
      myPacketPtr->ackNum = 999999;
      // printf("toLayer3: corrupt ackNum\n");
      }
    if (TRACE > 0)
      printf("          toLayer3: packet being corrupted\n");
  }

  if (TRACE > 2)
    printf("          toLayer3: scheduling arrival on other side\n");
  insertEvent(evenPtr);
}

void toLayer5(int AorB, char dataSent[20]) {
  int i;
  if (TRACE > 2) {
    printf("          toLayer5: data received: ");
    for (i = 0; i < 20; i++)
      printf("%c", dataSent[i]);
    printf("\n");
  }
}