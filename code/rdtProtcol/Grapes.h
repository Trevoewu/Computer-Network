#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define BIDIRECTIONAL 0 /* change to 1 if you're doing extra credit  and write a routine called    */
struct msg {
  char data[20];
};
struct pkt {
  int seqNum;
  int ackNum;
  int checksum;
  char payload[20];
};
struct event {
  float evenTime;        /* event time */
  int evenType;          /* event type code */
  int evenEntity;        /* entity where event occurs */
  struct pkt *packetPtr; /* ptr to packet (if any) assoc w/ this event */
  struct event *preEven;
  struct event *next;
};
struct event *evenList = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_LAYER5 1
#define FROM_LAYER3 2
#define WINDOWS_SIZE 5
#define BUFFER_SIZE 20
#define OFF 0
#define ON 1
#define A 0
#define B 1

int TRACE = 1;   /* for my debugging */
int numMsgs = 0; /* number of messages from 5 to 4 so far */
int maxMsg = 0;  /* number of msgs to generate, then stop */
float time = (float)0.000;
float lossPro;     /* probability that a packet is dropped  */
float corruptPro;  /* probability that one bit is packet is flipped */
float arrivalRate; /* arrival rate of messages from layer 5 */
int numOfToLayer3; /* number sent into layer 3 */
int numOfLost;     /* number lost in media */
int numCorrupt;    /* number corrupted by media*/
int seqNum = 0;
void init();
void generate_next_arrival();
void insertEvent(struct event *p);
float getRand();
void toLayer3(int AorB, struct pkt packet);
void toLayer5(int AorB, char dataSent[20]);
void startTimer(int AorB, float increment);
void stopTimer(int AorB);
bool notCorrupt(struct pkt packet);
struct pkt copy;
int base = 0;
int next = 0;
int bufferNext = 0;
int winLen;
struct msg sendWin[WINDOWS_SIZE];