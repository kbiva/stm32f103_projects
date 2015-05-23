/*
 *  KDC_5060R.h
 *
 *  Kenwood KDC-KDC_5060R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#ifndef _KDC_5060R_H
#define _KDC_5060R_H

#include "stm32f10x_conf.h"

#define CLK_Pin      GPIO_Pin_13            // CLK = PB[13]
#define CLK_Speed    GPIO_Speed_50MHz
#define CLK_Mode     GPIO_Mode_AF_PP
#define CLK_Port     GPIOB
#define CLK_Bus      RCC_APB2Periph_GPIOB

#define DATAL_Pin    GPIO_Pin_14            // MISO = PB[14]
#define DATAL_Speed  GPIO_Speed_50MHz
#define DATAL_Mode   GPIO_Mode_IN_FLOATING
#define DATAL_Port   GPIOB
#define DATAL_Bus    RCC_APB2Periph_GPIOB

#define DATAS_Pin    GPIO_Pin_15            // MOSI = PB[15]
#define DATAS_Speed  GPIO_Speed_50MHz
#define DATAS_Mode   GPIO_Mode_AF_PP
#define DATAS_Port   GPIOB
#define DATAS_Bus    RCC_APB2Periph_GPIOB

#define RESET_Pin    GPIO_Pin_10            // Reset = PC[10]
#define RESET_Speed  GPIO_Speed_50MHz
#define RESET_Mode   GPIO_Mode_Out_PP
#define RESET_Port   GPIOC
#define RESET_Bus    RCC_APB2Periph_GPIOC

#define CE_Pin       GPIO_Pin_12            // CE = PB[12]
#define CE_Speed     GPIO_Speed_50MHz
#define CE_Mode      GPIO_Mode_Out_PP
#define CE_Port      GPIOB
#define CE_Bus       RCC_APB2Periph_GPIOB

typedef struct _PIN {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN;

#define CMD_INPUT 0x42
#define CMD_OUTPUT 0x43

#define MAX_POSITIONS 8
#define MAX_SEGMENTS 14

#define TRUE 1
#define FALSE 0

#define MAX_SYMBOLS 43

// name bit_position
#define LEDS_GREEN 6
#define LEDS_RED 9
#define SYMBOL_DAB 12
#define SYMBOL_AUTO 13
#define SYMBOL_2 14
#define SYMBOL_CLOCK 15
#define SYMBOL_RDS 16
#define SYMBOL_PTY 17
#define SYMBOL_B_NR 18
#define SYMBOL_TP 19
#define SYMBOL_REG 20
#define SYMBOL_DIGIT_TOP_RIGHT 21
#define SYMBOL_DIGIT_BOTTOM_RIGHT 22
#define SYMBOL_CH 23
#define SYMBOL_DIGIT_TOP 24
#define SYMBOL_DIGIT_MIDDLE 25
#define SYMBOL_DIGIT_BOTTOM 26
#define SYMBOL_DIGIT_TOP_LEFT 27
#define SYMBOL_DIGIT_BOTTOM_LEFT 28
#define SYMBOL_ATT 29
#define SYMBOL_TI 30
#define SYMBOL_LOUD 31
#define SYMBOL_ST 32
#define POS8_BOTTOM 33
#define POS8_BOTTOM_RIGHT 34
#define POS8_TOP_RIGHT 35
#define POS8_BOTTOM_RIGHT_LEANED 36
#define POS8_MIDDLE_RIGHT 37
#define POS8_TOP_RIGHT_LEANED 38
#define POS8_BOTTOM_MIDDLE 39
#define POS8_TOP_MIDDLE 40
#define POS8_TOP 41
#define POS8_BOTTOM_LEFT_LEANED 42
#define POS8_MIDDLE_LEFT 43
#define POS8_TOP_LEFT_LEANED 44
#define SYMBOL_1 45
#define POS8_BOTTOM_LEFT 46
#define POS8_TOP_LEFT 47
#define POS7_BOTTOM 48
#define POS7_BOTTOM_RIGHT 49
#define POS7_TOP_RIGHT 50
#define POS7_BOTTOM_RIGHT_LEANED 51
#define POS7_MIDDLE_RIGHT 52
#define POS7_TOP_RIGHT_LEANED 53
#define POS7_BOTTOM_MIDDLE 54
#define POS7_TOP_MIDDLE 55
#define POS7_TOP 56
#define POS7_BOTTOM_LEFT_LEANED 72
#define POS7_MIDDLE_LEFT 73
#define POS7_TOP_LEFT_LEANED 74
#define SYMBOL_DOUBLE_COLON_2 75
#define POS7_BOTTOM_LEFT 76
#define POS7_TOP_LEFT 77
#define SYMBOL_TA 78
#define SYMBOL_1_LEFT 79
#define SYMBOL_1_V 80
#define SYMBOL_1_BOTTOM 81
#define SYMBOL_1_RIGHT 82
#define SYMBOL_1_TOP_MIDDLE 83
#define SYMBOL_2_V_BOTTOM 84
#define SYMBOL_2_LEFT_RIGHT 85
#define SYMBOL_2_V_TOP 86
#define SYMBOL_TRIANGLE1 87
#define SYMBOL_II 88
#define SYMBOL_I 89
#define SYMBOL_IN 90
#define POS1_BOTTOM_LEFT 91
#define POS1_TOP_LEFT 92
#define POS1_BOTTOM_LEFT_LEANED 93
#define POS1_MIDDLE_LEFT 94
#define POS1_TOP_LEFT_LEANED 95
#define POS1_BOTTOM_MIDDLE 96
#define POS1_TOP_MIDDLE 97
#define POS1_TOP 98
#define POS1_BOTTOM_RIGHT_LEANED 99
#define POS1_MIDDLE_RIGHT 100
#define POS1_TOP_RIGHT_LEANED 101
#define POS1_BOTTOM 102
#define POS1_BOTTOM_RIGHT 103
#define POS1_TOP_RIGHT 104
#define SYMBOL_TRIANGLE2 105
#define POS2_BOTTOM_LEFT 106
#define POS2_TOP_LEFT 107
#define POS2_BOTTOM_LEFT_LEANED 108
#define POS2_MIDDLE_LEFT 109
#define POS2_TOP_LEFT_LEANED 110
#define POS2_BOTTOM_MIDDLE 111
#define POS2_TOP_MIDDLE 112
#define POS2_TOP 113
#define POS2_BOTTOM_RIGHT_LEANED 114
#define POS2_MIDDLE_RIGHT 115
#define POS2_TOP_RIGHT_LEANED 116
#define POS2_BOTTOM 117
#define POS2_BOTTOM_RIGHT 118
#define POS2_TOP_RIGHT 119
#define SYMBOL_MTL 120
#define POS3_BOTTOM_LEFT 121
#define POS3_TOP_LEFT 122
#define POS3_BOTTOM_LEFT_LEANED 123
#define POS3_MIDDLE_LEFT 124
#define POS3_TOP_LEFT_LEANED 125
#define POS3_BOTTOM_MIDDLE 126
#define POS3_TOP_MIDDLE 127
#define POS3_TOP 128
#define POS3_BOTTOM_RIGHT_LEANED 144
#define POS3_MIDDLE_RIGHT 145
#define POS3_TOP_RIGHT_LEANED 146
#define POS3_BOTTOM 147
#define POS3_BOTTOM_RIGHT 148
#define POS3_TOP_RIGHT 149
#define SYMBOL_NEWS 150
#define POS4_BOTTOM_LEFT 151
#define POS4_TOP_LEFT 152
#define POS4_BOTTOM_LEFT_LEANED 153
#define POS4_MIDDLE_LEFT 154
#define POS4_TOP_LEFT_LEANED 155
#define POS4_BOTTOM_MIDDLE 156
#define POS4_TOP_MIDDLE 157
#define POS4_TOP 158
#define POS4_BOTTOM_RIGHT_LEANED 159
#define POS4_MIDDLE_RIGHT 160
#define POS4_TOP_RIGHT_LEANED 161
#define POS4_BOTTOM 162
#define POS4_BOTTOM_RIGHT 163
#define POS4_TOP_RIGHT 164
#define SYMBOL_DUAL 165
#define SYMBOL_DOUBLE_COLON_1 166
#define SYMBOL_MINUS 167
#define POS5_BOTTOM_LEFT 169
#define POS5_TOP_LEFT 170
#define POS5_BOTTOM_LEFT_LEANED 171
#define POS5_MIDDLE_LEFT 172
#define POS5_TOP_LEFT_LEANED 173
#define POS5_BOTTOM_MIDDLE 174
#define POS5_TOP_MIDDLE 175
#define POS5_TOP 176
#define POS5_BOTTOM_RIGHT_LEANED 177
#define POS5_MIDDLE_RIGHT 178
#define POS5_TOP_RIGHT_LEANED 179
#define POS5_BOTTOM 180
#define POS5_BOTTOM_RIGHT 181
#define POS5_TOP_RIGHT 182
#define SYMBOL_CHANGER 183
#define POS6_BOTTOM_LEFT 184
#define POS6_TOP_LEFT 185
#define POS6_BOTTOM_LEFT_LEANED 186
#define POS6_MIDDLE_LEFT 187
#define POS6_TOP_LEFT_LEANED 188
#define POS6_BOTTOM_MIDDLE 189
#define POS6_TOP_MIDDLE 190
#define POS6_TOP 191
#define POS6_BOTTOM_RIGHT_LEANED 192
#define POS6_MIDDLE_RIGHT 193
#define POS6_TOP_RIGHT_LEANED 194
#define POS6_BOTTOM 195
#define POS6_BOTTOM_RIGHT 196
#define POS6_TOP_RIGHT 197

// byte 0
#define KEY_3          0x40
#define KEY_2          0x80

// byte 1
#define KEY_1          0x01
#define KEY_4          0x02
#define KEY_VOL_UP     0x08
#define KEY_VOL_DOWN   0x10
#define KEY_ATT        0x20
#define KEY_5          0x40
#define KEY_6          0x80

// byte 2
#define KEY_PLAY       0x01
#define KEY_SRC        0x02
#define KEY_SEEK_RIGHT 0x04
#define KEY_DISP       0x08
#define KEY_SEEK_LEFT  0x20
#define KEY_FM         0x40
#define KEY_AM         0x80

// byte 3
#define KEY_AUDIO      0x01
#define KEY_TI         0x02

void KDC_5060R_init(void);
void KDC_5060R_send(void);
void KDC_5060R_receive(void);
void KDC_5060R_clear_text(uint8_t flush);
void KDC_5060R_text(char *pString,int8_t pos,uint8_t clear,uint8_t flush);
void KDC_5060R_char(char chr,int8_t pos);
void KDC_5060R_symbol(uint8_t val,uint8_t on,uint8_t flush);
void KDC_5060R_digit(uint8_t val,uint8_t flush);
void KDC_5060R_digit_off(uint8_t flush);
void KDC_5060R_leds_red(void);
void KDC_5060R_leds_green(void);
void KDC_5060R_leds_yellow(void);
void KDC_5060R_leds_off(void);
void KDC_5060R_char_all(uint8_t pos,uint8_t on,uint8_t flush);
void KDC_5060R_text_all(uint8_t on,uint8_t flush);
#endif
