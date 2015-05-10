/*
 *  mcx80.h
 *
 *  Scott MCX80 face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#ifndef MCX80_H
#define MCX80_H

#include "stm32f10x_conf.h"

#define CLK_Pin    GPIO_Pin_13            // CLK = PB[13]
#define CLK_Speed  GPIO_Speed_50MHz
#define CLK_Mode   GPIO_Mode_AF_PP
#define CLK_Port   GPIOB
#define CLK_Bus    RCC_APB2Periph_GPIOB

#define DI_Pin     GPIO_Pin_15            // MOSI (DI) = PB[15]
#define DI_Speed   GPIO_Speed_50MHz
#define DI_Mode    GPIO_Mode_AF_PP
#define DI_Port    GPIOB
#define DI_Bus     RCC_APB2Periph_GPIOB

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

#define KEY1_Pin      GPIO_Pin_2            // KEY1 = PC[2]
#define KEY1_Speed    GPIO_Speed_50MHz
#define KEY1_Mode     GPIO_Mode_AIN
#define KEY1_Port     GPIOC
#define KEY1_Bus      RCC_APB2Periph_GPIOC

#define KEY2_Pin      GPIO_Pin_3            // KEY2 = PC[3]
#define KEY2_Speed    GPIO_Speed_50MHz
#define KEY2_Mode     GPIO_Mode_AIN
#define KEY2_Port     GPIOC
#define KEY2_Bus      RCC_APB2Periph_GPIOC

#define VOLMINUS_Pin    GPIO_Pin_6            // VOL- = PB[6]
#define VOLMINUS_Speed  GPIO_Speed_50MHz
#define VOLMINUS_Mode   GPIO_Mode_IN_FLOATING
#define VOLMINUS_Port   GPIOB
#define VOLMINUS_Bus    RCC_APB2Periph_GPIOB

#define VOLPLUS_Pin     GPIO_Pin_7            // VOL+ = PB[7]
#define VOLPLUS_Speed   GPIO_Speed_50MHz
#define VOLPLUS_Mode    GPIO_Mode_IN_FLOATING
#define VOLPLUS_Port    GPIOB
#define VOLPLUS_Bus     RCC_APB2Periph_GPIOB

typedef struct _PIN {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN;

typedef enum {
  FORWARD,
  BACKWARD
} Volume_Direction;

#define CMD_INPUT 0x41

#define MAX_POSITIONS 8
#define MAX_SEGMENTS 13

#define TRUE 1
#define FALSE 0

// name bit_position
#define SYMBOL_DISC1 0
#define SYMBOL_DISC2 1
#define POS1 2
#define SYMBOL_DISC3 15
#define SYMBOL_DISC4 16
#define POS2 17
#define SYMBOL_MUTE 30
#define SYMBOL_INT 31
#define POS3 32
#define SYMBOL_BLUETOOTH 45
#define SYMBOL_TRACK 46
#define POS4 47
#define SYMBOL_DISC 60
#define SYMBOL_RDM 61
#define POS5 62
#define SYMBOL_RPT 75
#define SYMBOL_MP3 76
#define POS6 77
#define SYMBOL_FOLDER 90
#define SYMBOL_MUSIC 91
#define POS7 92
#define SYMBOL_NOTE 105
#define SYMBOL_BOOK 106
#define POS8 107
#define SYMBOL_LOUD 120
#define SYMBOL_PEOPLE 121
#define SYMBOL_2RINGS 122
#define SYMBOL_PTY 123
#define SYMBOL_AF 124
#define SYMBOL_POINT 125
#define SYMBOL_EON 126
#define SYMBOL_TA 127
#define SYMBOL_TP 128
#define SYMBOL_EQ 129
#define SYMBOL_POP 130
#define SYMBOL_DIGIT_DEC 131
#define SYMBOL_FLAT 132
#define SYMBOL_EQ1 133
#define SYMBOL_ROCK 134
#define SYMBOL_CLAS 135
#define SYMBOL_DIGIT_TOP_LEFT 136
#define SYMBOL_DIGIT_BOTTOM_LEFT 137
#define SYMBOL_DIGIT_TOP 138
#define SYMBOL_DIGIT_MIDDLE 139
#define SYMBOL_DIGIT_BOTTOM 140
#define SYMBOL_DIGIT_TOP_RIGHT 141
#define SYMBOL_DIGIT_BOTTOM_RIGHT 142
#define SYMBOL_DOTS1 143
#define SYMBOL_DOTS2 144
#define SYMBOL_DOTS3 145
#define SYMBOL_DOTS4 146
#define SYMBOL_DOTS5 147
#define SYMBOL_DOTS6 148
#define SYMBOL_DOTS7 149

void MCX80_init(void);
void MCX80_send(void);
void MCX80_char(char val,int8_t pos);
void MCX80_text(char *pString,int8_t pos,uint8_t clear,uint8_t flush);
void MCX80_clear_text(uint8_t flush);
void MCX80_symbol(uint8_t val,uint8_t on,uint8_t flush);
void MCX80_digit(uint8_t val,uint8_t flush);
void MCX80_digit_off(uint8_t flush);

#endif
