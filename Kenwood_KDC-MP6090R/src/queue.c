/*
 *  queue.c
 *
 *  Author: Kestutis Bivainis
 */

#include <string.h>
#include "queue.h"

IR_PACKET irbuf[QUEUE_SIZE];
int spos = 0;
int rpos = 0;

void qstore(IR_PACKET *q) {

  if(spos+1==rpos || (spos+1==QUEUE_SIZE && !rpos)) {
    return;
  }
  memcpy(&irbuf[spos],q,sizeof(IR_PACKET));
  spos++;
  if(spos==QUEUE_SIZE) spos = 0;
}

IR_PACKET* qretrieve(void) {

  if(rpos==QUEUE_SIZE) rpos = 0;
  if(rpos==spos) return 0;

  rpos++;
  return &irbuf[rpos-1];
}

int qsize(void) {

  return spos>=rpos?spos-rpos:QUEUE_SIZE-rpos+spos;
}
