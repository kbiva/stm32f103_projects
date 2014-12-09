/*
 *  queue.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdint.h>
#include "ir_decode.h"

#define QUEUE_SIZE 100
#define COMMAND_SIZE 24

void qstore(IR_PACKET *q);
IR_PACKET* qretrieve(void);
int qsize(void);

#endif
