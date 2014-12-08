/*
 *  mp6090r.h
 *
 *  Kenwood KDC-MP6090R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#ifndef MP6090R_H
#define MP6090R_H

#include "stm32f10x_conf.h"

#define DIM_Pin      GPIO_Pin_4             // DIM = PD[4]
#define DIM_Speed    GPIO_Speed_50MHz
#define DIM_Mode     GPIO_Mode_Out_PP
#define DIM_Port     GPIOD
#define DIM_Bus      RCC_APB2Periph_GPIOD

#define CLK_Pin      GPIO_Pin_13            // CLK = PB[13]
#define CLK_Speed    GPIO_Speed_50MHz
#define CLK_Mode     GPIO_Mode_AF_PP
#define CLK_Port     GPIOB
#define CLK_Bus      RCC_APB2Periph_GPIOB

#define DATAL_Pin    GPIO_Pin_14            // MISO = PB[14]
#define DATAL_Speed  GPIO_Speed_50MHz
#define DATAL_Mode   GPIO_Mode_AF_PP
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

#define REMO_Pin     GPIO_Pin_12            // REMO = PD[12]
#define REMO_Speed   GPIO_Speed_50MHz
#define REMO_Mode    GPIO_Mode_IN_FLOATING
#define REMO_Port    GPIOD
#define REMO_Bus     RCC_APB2Periph_GPIOD

typedef struct _PIN {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN;

#define CMD_INPUT 0x42
#define CMD_OUTPUT 0xC2

#define MODE_NORMAL 0x00
#define MODE_SLEEP  0x80

#define SEGMENT_ON   0x00
#define SEGMENT_OFF  0x01

// Display contrast adjustment circuit
#define CTC_ON   0x02
#define CTC_OFF  0x00

// Display contrast
typedef enum _CONTRAST {
  CT0=0x00,
  CT1=0x20,
  CT2=0x10,
  CT3=0x30,
  CT4=0x08,
  CT5=0x28,
  CT6=0x18,
  CT7=0x38,
  CT8=0x04,
  CT9=0x24,
  CT10=0x14
} CONTRAST;

#define CT_MAX CT1
#define CT_MIN CT11

// PC1 - ?
// PC2 - leds red
// PC3 - leds green
// PC4 - ?
#define LEDS_RED   0x01
#define LEDS_GREEN 0x80

#define MAX_SYMBOLS 12
#define MAX_WIDTH 5
#define MAX_HEIGHT 7

#define TRUE 1
#define FALSE 0

#define SYMBOL_CLOCK  0
#define SYMBOL_IN     1
#define SYMBOL_LINES1 2
#define SYMBOL_LINE1 3
#define SYMBOL_LINES_LOW 4
#define SYMBOL_LINE2 5
#define SYMBOL_LINE3 6
#define SYMBOL_LINE4 7
#define SYMBOL_LINE5 8
#define SYMBOL_LINE6 9
#define SYMBOL_LINES_MID 10
#define SYMBOL_LINES2 11
#define SYMBOL_LINE7 12
#define SYMBOL_LINE8 13
#define SYMBOL_LINES_HIGH 14
#define SYMBOL_LINE9 15
#define SYMBOL_LINE10 16
#define SYMBOL_LINE11 17
#define SYMBOL_LINE12 18
#define SYMBOL_LINE13 19
#define SYMBOL_LINE14 20
#define SYMBOL_LINES_QCONTROL 21
#define SYMBOL_ST 22
#define SYMBOL_ATT 23
#define SYMBOL_LOUD 24
#define SYMBOL_MTL 25
#define SYMBOL_NEWS 26
#define SYMBOL_DAB 27
#define SYMBOL_B 28
#define SYMBOL_PS 29
#define SYMBOL_NR 30
#define SYMBOL_AUTO 31
#define SYMBOL_PTY 32
#define SYMBOL_RDS 33
#define SYMBOL_TI 34
#define SYMBOL_1 35
#define SYMBOL_2 36
#define SYMBOL_LINES_EQ1 37

#define SYMBOL_6x9_6INCH 42
#define SYMBOL_5_4INCH 43
#define SYMBOL_OEM 44
#define SYMBOL_TRIANGLE_EQ1 45
#define SYMBOL_TRIANGLE_EQ2 46
#define SYMBOL_TRIANGLE_EQ3 47
#define SYMBOL_TRIANGLE_EQ4 48
#define SYMBOL_TRIANGLE_EQ5 49
#define SYMBOL_DIGIT_TOP_RIGHT 50
#define SYMBOL_DIGIT_TOP 51
#define SYMBOL_DIGIT_TOP_LEFT 52
#define SYMBOL_DIGIT_MIDDLE 53
#define SYMBOL_DIGIT_BOTTOM_RIGHT 54
#define SYMBOL_DIGIT_BOTTOM_LEFT 55
#define SYMBOL_DIGIT_BOTTOM 56
#define SYMBOL_CH 57
#define SYMBOL_TRIANGLE_EQ6 58
#define SYMBOL_LINES_EQ2 59

// byte 0
#define KEY_VOL_UP     0x40
#define KEY_VOL_DOWN   0x20
#define KEY_ATT        0x10
#define KEY_4          0x08
#define KEY_6          0x04
#define KEY_1          0x02
#define KEY_2          0x01

// byte 1
#define KEY_3          0x80
#define KEY_5          0x40
#define KEY_TI         0x20
#define KEY_SRC        0x10
#define KEY_SEEK_RIGHT 0x08
#define KEY_AM         0x04
#define KEY_FM         0x02
#define KEY_EJECT      0x01

// byte 2
#define KEY_DISP       0x80
#define KEY_SEEK_LEFT  0x40
#define KEY_Q          0x20
#define KEY_PLAY       0x10
#define KEY_MINUS      0x08

void GPIO_Configuration(void);
void MP6090R_init(CONTRAST val);
void MP6090R_dimmer(uint8_t val);
void MP6090R_send(void);
void MP6090R_receive(void);
void MP6090R_clear_buf(uint8_t flush);
void MP6090R_fill_buf(uint8_t flush);
void MP6090R_clear_text(uint8_t flush);
void MP6090R_clear_symbols(uint8_t flush);
void MP6090R_text(char *pString,uint8_t start,uint8_t pos,uint8_t clear,uint8_t flush);
void MP6090R_char(char chr,uint8_t start,uint8_t pos);
void MP6090R_symbol(uint8_t val,uint8_t on,uint8_t flush);
void MP6090R_scroll_left(uint8_t pos,uint8_t flush);
void MP6090R_scroll_right(uint8_t pos,uint8_t flush);
void MP6090R_scroll_up(uint8_t pos,uint8_t flush);
void MP6090R_scroll_down(uint8_t pos,uint8_t flush);
void MP6090R_digit(uint8_t val,uint8_t flush);
void MP6090R_digit_off(uint8_t flush);
void MP6090R_contrast(CONTRAST val);
void MP6090R_leds_red(void);
void MP6090R_leds_green(void);
void MP6090R_leds_off(void);

#endif
