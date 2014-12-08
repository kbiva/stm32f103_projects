/*
 *  cd354mp.h
 *
 *  Roadstar CD-354MP face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#ifndef CD354MP_H
#define CD354MP_H

#include "stm32f10x_conf.h"

#define CLK_Pin    GPIO_Pin_13            // CLK = PB[13]
#define CLK_Speed  GPIO_Speed_50MHz
#define CLK_Mode   GPIO_Mode_AF_PP
#define CLK_Port   GPIOB
#define CLK_Bus    RCC_APB2Periph_GPIOB

#define DO_Pin     GPIO_Pin_15            // MOSI (DO) = PB[15]
#define DO_Speed   GPIO_Speed_50MHz
#define DO_Mode    GPIO_Mode_AF_PP
#define DO_Port    GPIOB
#define DO_Bus     RCC_APB2Periph_GPIOB

#define CE_Pin     GPIO_Pin_12            // CE = PB[12]
#define CE_Speed   GPIO_Speed_50MHz
#define CE_Mode    GPIO_Mode_Out_PP
#define CE_Port    GPIOB
#define CE_Bus     RCC_APB2Periph_GPIOB

#define INH_Pin    GPIO_Pin_10            // INH = PC[10]
#define INH_Speed  GPIO_Speed_50MHz
#define INH_Mode   GPIO_Mode_Out_PP
#define INH_Port   GPIOC
#define INH_Bus    RCC_APB2Periph_GPIOC

#define KEY1_Pin      GPIO_Pin_2            // KEY-IN0 = PC[2]
#define KEY1_Speed    GPIO_Speed_50MHz
#define KEY1_Mode     GPIO_Mode_AIN
#define KEY1_Port     GPIOC
#define KEY1_Bus      RCC_APB2Periph_GPIOC

#define KEY2_Pin      GPIO_Pin_3            // KEY-IN0 = PC[3]
#define KEY2_Speed    GPIO_Speed_50MHz
#define KEY2_Mode     GPIO_Mode_AIN
#define KEY2_Port     GPIOC
#define KEY2_Bus      RCC_APB2Periph_GPIOC

typedef struct _PIN {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN;

#define CMD_INPUT 0x82

#define TRUE 1
#define FALSE 0

#define SYMBOL_STEREO_POS 0
#define SYMBOL_STEREO_BIT 0x80
#define SYMBOL_MP3_POS 1
#define SYMBOL_MP3_BIT 0x08
#define SYMBOL_AF_POS 3
#define SYMBOL_AF_BIT 0x10
#define SYMBOL_TA_POS 5
#define SYMBOL_TA_BIT 0x20
#define SYMBOL_TP_POS 7
#define SYMBOL_TP_BIT 0x40
#define SYMBOL_DIR_POS 9
#define SYMBOL_DIR_BIT 0x10
#define SYMBOL_POINT_POS 10
#define SYMBOL_POINT_BIT 0x01
#define SYMBOL_PTY_POS 11
#define SYMBOL_PTY_BIT 0x20
#define SYMBOL_CD1_POS 15
#define SYMBOL_CD1_BIT 0x10
#define SYMBOL_CD2_POS 15
#define SYMBOL_CD2_BIT 0x08
#define SYMBOL_CD3_POS 15
#define SYMBOL_CD3_BIT 0x04
#define SYMBOL_RING_LEFT1_POS 16
#define SYMBOL_RING_LEFT1_BIT 0x40
#define SYMBOL_RING_LEFT2_POS 16
#define SYMBOL_RING_LEFT2_BIT 0x20
#define SYMBOL_RING_LEFT3_POS 16
#define SYMBOL_RING_LEFT3_BIT 0x10
#define SYMBOL_RING_LEFT4_POS 16
#define SYMBOL_RING_LEFT4_BIT 0x08
#define SYMBOL_RING_LEFT5_POS 16
#define SYMBOL_RING_LEFT5_BIT 0x04
#define SYMBOL_RING_LEFT6_POS 16
#define SYMBOL_RING_LEFT6_BIT 0x02
#define SYMBOL_DSP_POS 17
#define SYMBOL_DSP_BIT 0x80
#define SYMBOL_RING_RIGHT1_POS 17
#define SYMBOL_RING_RIGHT1_BIT 0x40
#define SYMBOL_RING_RIGHT2_POS 17
#define SYMBOL_RING_RIGHT2_BIT 0x20
#define SYMBOL_RING_RIGHT3_POS 17
#define SYMBOL_RING_RIGHT3_BIT 0x10
#define SYMBOL_RING_RIGHT4_POS 17
#define SYMBOL_RING_RIGHT4_BIT 0x08
#define SYMBOL_RING_RIGHT5_POS 17
#define SYMBOL_RING_RIGHT5_BIT 0x04
#define SYMBOL_RING_RIGHT6_POS 17
#define SYMBOL_RING_RIGHT6_BIT 0x02
#define SYMBOL_BAR1_POS 18
#define SYMBOL_BAR1_BIT 0x40
#define SYMBOL_BAR2_POS 18
#define SYMBOL_BAR2_BIT 0x20
#define SYMBOL_BAR3_POS 18
#define SYMBOL_BAR3_BIT 0x08
#define SYMBOL_BAR4_POS 18
#define SYMBOL_BAR4_BIT 0x04
#define SYMBOL_ANTENNA_POS 18
#define SYMBOL_ANTENNA_BIT 0x01
#define SYMBOL_CDIN_POS 19
#define SYMBOL_CDIN_BIT 0x80

#define SYMBOL_DIGIT_TOP_RIGHT_POS 3
#define SYMBOL_DIGIT_TOP_RIGHT_BIT 0x02
#define SYMBOL_DIGIT_TOP_POS 1
#define SYMBOL_DIGIT_TOP_BIT 0x01
#define SYMBOL_DIGIT_TOP_LEFT_POS 14
#define SYMBOL_DIGIT_TOP_LEFT_BIT 0x04
#define SYMBOL_DIGIT_MIDDLE_POS 5
#define SYMBOL_DIGIT_MIDDLE_BIT 0x04
#define SYMBOL_DIGIT_BOTTOM_RIGHT_POS 7
#define SYMBOL_DIGIT_BOTTOM_RIGHT_BIT 0x08
#define SYMBOL_DIGIT_BOTTOM_LEFT_POS 13
#define SYMBOL_DIGIT_BOTTOM_LEFT_BIT 0x40
#define SYMBOL_DIGIT_BOTTOM_POS 12
#define SYMBOL_DIGIT_BOTTOM_BIT 0x02

void GPIO_Configuration(void);
void CD354MP_init(void);
void CD354MP_send(void);
void CD354MP_char(char val,uint8_t pos,uint8_t flush);
void CD354MP_text(char *pString,uint8_t start,uint8_t clear,uint8_t flush);
void CD354MP_clear_text(uint8_t flush);
void CD354MP_symbol(uint8_t val,uint8_t bit,uint8_t on,uint8_t flush);
void CD354MP_digit(uint8_t val,uint8_t flush);
void CD354MP_digit_off(uint8_t flush);

#endif
