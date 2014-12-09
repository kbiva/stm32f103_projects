/*
 *  ir_decode.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef __IR_DECODE_H
#define __IR_DECODE_H

#include "stm32f10x.h"

#define IR_TIM                 TIM4                     /*!< Timer used for IR decoding */
#define TIM_PRESCALER          (72-1)                     /* !< TIM prescaler */
#define IR_TIM_CLK             RCC_APB1Periph_TIM4      /*!< Clock of the used timer */
#define IR_TIM_IRQn            TIM4_IRQn             /*!< IR TIM IRQ */
#define IR_TIM_Channel         TIM_Channel_1            /*!< IR TIM Channel */

typedef struct
{
  uint16_t type;
  uint16_t lastc;
  uint32_t data;
} IR_PACKET;

enum {
  REC_Idle = 0,
  REC_Recording,
  REC_Captured,
  REC_Decoded
};

enum {
  PROTOCOL_SIRC = 1,
  PROTOCOL_NEC,
  PROTOCOL_PANASONIC,
  PROTOCOL_SAMSUNG,
  PROTOCOL_RC5,
  PROTOCOL_ADAPTIVE,
  PROTOCOL_MCE
};

#define IT_TIMEOUT_MAX_US  15000

#define checkVal(var,val,tol) (var>(val*(100-tol)/100) && var<(val*(100+tol)/100))

void IR_DeInit(void);
void IR_Init(void);
void IR_Decode(IR_PACKET *irp);

#endif
