/*
 *  nec_decode.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef __NEC_DECODE_H
#define __NEC_DECODE_H

// NEC  {38.4k,564}<1,-1|1,-3>(16,-8,D:8,S:8,F:8,~F:8,1,-78,(16,-4,1,-173)*)
// NEC2 {38.4k,564}<1,-1|1,-3>(16,-8,D:8,S:8,F:8,~F:8,1,-78)+

#define IR_NEC_TIMEOUT_US        15000

#define IR_NEC_TIME_UNIT_US      564

#define IR_NEC1_HEADER_PULSE      (16*IR_NEC_TIME_UNIT_US)
#define IR_NEC1_HEADER_WIDTH      (16*IR_NEC_TIME_UNIT_US + 8*IR_NEC_TIME_UNIT_US)

#define IR_NEC2_REPEAT_PULSE      (16*IR_NEC_TIME_UNIT_US)
#define IR_NEC2_REPEAT_WIDTH      (16*IR_NEC_TIME_UNIT_US + 4*IR_NEC_TIME_UNIT_US)

#define IR_NEC_0_PULSE           IR_NEC_TIME_UNIT_US
#define IR_NEC_0_WIDTH           (2*IR_NEC_TIME_UNIT_US)

#define IR_NEC_1_PULSE           IR_NEC_TIME_UNIT_US
#define IR_NEC_1_WIDTH           (4*IR_NEC_TIME_UNIT_US)

#define IR_NEC_TOL_PCT           20

#endif /* __NEC_DECODE_H */
