/*
 *  mdx7900r.h
 *
 *  Sony MDX-C7900R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#ifndef MDX7900R_H
#define MDX7900R_H

#include "stm32f10x_conf.h"

#define CLK_Pin         GPIO_Pin_13            // CLK = PB[13]
#define CLK_Speed       GPIO_Speed_50MHz
#define CLK_Mode        GPIO_Mode_AF_PP
#define CLK_Port        GPIOB
#define CLK_Bus         RCC_APB2Periph_GPIOB

#define DO_Pin          GPIO_Pin_15            // MOSI = PB[15]
#define DO_Speed        GPIO_Speed_50MHz
#define DO_Mode         GPIO_Mode_AF_PP
#define DO_Port         GPIOB
#define DO_Bus          RCC_APB2Periph_GPIOB

#define CE_Pin          GPIO_Pin_12            // CE = PB[12]
#define CE_Speed        GPIO_Speed_50MHz
#define CE_Mode         GPIO_Mode_Out_PP
#define CE_Port         GPIOB
#define CE_Bus          RCC_APB2Periph_GPIOB

#define KEYIN0_Pin      GPIO_Pin_2            // KEY-IN0 = PC[2]
#define KEYIN0_Speed    GPIO_Speed_50MHz
#define KEYIN0_Mode     GPIO_Mode_AIN
#define KEYIN0_Port     GPIOC
#define KEYIN0_Bus      RCC_APB2Periph_GPIOC

#define KEYIN1_Pin      GPIO_Pin_3            // KEY-IN0 = PC[3]
#define KEYIN1_Speed    GPIO_Speed_50MHz
#define KEYIN1_Mode     GPIO_Mode_AIN
#define KEYIN1_Port     GPIOC
#define KEYIN1_Bus      RCC_APB2Periph_GPIOC

#define DBASS_Pin       GPIO_Pin_1            // D-BASS = PC[1]
#define DBASS_Speed     GPIO_Speed_50MHz
#define DBASS_Mode      GPIO_Mode_AIN
#define DBASS_Port      GPIOC
#define DBASS_Bus       RCC_APB2Periph_GPIOC

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

#define CMD_DISPLAY_SETTING 0x00
#define CMD_DISPLAY_SETTING_1_8_DUTY 0x00
#define CMD_DISPLAY_SETTING_1_15_DUTY 0x01
#define CMD_DISPLAY_SETTING_MASTER 0x00
#define CMD_DISPLAY_SETTING_SLAVE 0x02
#define CMD_DISPLAY_SETTING_VOLTAGE_EXTERNAL 0x00
#define CMD_DISPLAY_SETTING_VOLTAGE_INTERNAL 0x04

#define CMD_DATA_SETTING 0x40
#define CMD_DATA_SETTING_WRITE_DISPLAY_DATA_RAM 0x00
#define CMD_DATA_SETTING_WRITE_CHARACTER_DISPLAY 0x01
#define CMD_DATA_SETTING_WRITE_CGRAM 0x02
#define CMD_DATA_SETTING_WRITE_LED_OUTPUT_LATCH 0x03
#define CMD_DATA_SETTING_READ_KEY_DATA 0x04
#define CMD_DATA_SETTING_ADDRESS_INCREMENT 0x00
#define CMD_DATA_SETTING_ADDRESS_FIXED 0x08

#define CMD_ADDRESS_SETTING 0x80

#define CMD_STATUS 0xC0
#define CMD_STATUS_LCD_OFF 0x00
#define CMD_STATUS_LCD_ON 0x02
#define CMD_STATUS_LED_OFF 0x00
#define CMD_STATUS_LED_ON 0x04
#define CMD_STATUS_KEY_SCAN_CONTROL_STOP 0x00
#define CMD_STATUS_KEY_SCAN_CONTROL_OPERATION 0x08
#define CMD_STATUS_STANDBY_OFF 0x00
#define CMD_STATUS_STANDBY_ON 0x10
#define CMD_STATUS_TEST_OFF 0x00
#define CMD_STATUS_TEST_ON 0x20

typedef enum _BACKLIGHT {
  LEDS_OFF=0x00,
  LEDS_GREEN=0x01,
  LEDS_AMBER=0x02,
  LEDS_YELLOW=LEDS_GREEN|LEDS_AMBER
} BACKLIGHT;

// 8bits - PICTOGRAPH DISPLAY RAM ADDRESS, 8bits - bits values
// PICTOGRAPH DISPLAY RAM ADDRESS 0
#define SYMBOL_DOUBLE_COLON 0x0004
#define SYMBOL_1 0x0079
#define SYMBOL_2 0x00AA
// PICTOGRAPH DISPLAY RAM ADDRESS 1
#define SYMBOL_BIG1 0x0101
#define SYMBOL_REG 0x0102
#define SYMBOL_ST 0x0104
#define SYMBOL_EON 0x0108
#define SYMBOL_TP 0x0110
#define SYMBOL_TA 0x0120
#define SYMBOL_LCL 0x0140
#define SYMBOL_AF 0x0180
// PICTOGRAPH DISPLAY RAM ADDRESS 2
#define SYMBOL_ARROW_LEFT 0x0208
#define SYMBOL_BANK 0x0210
#define SYMBOL_DISC 0x0240
// PICTOGRAPH DISPLAY RAM ADDRESS 3
#define SYMBOL_PLAYMODE 0x0320
#define SYMBOL_SETUP 0x0340
// PICTOGRAPH DISPLAY RAM ADDRESS 4
#define SYMBOL_2REP 0x0401
#define SYMBOL_1REP 0x0402
#define SYMBOL_REP_TOP 0x0404
#define SYMBOL_MONO 0x0408
#define SYMBOL_TRACK 0x0440
#define SYMBOL_ARROW_RIGHT 0x0480
// PICTOGRAPH DISPLAY RAM ADDRESS 5
#define SYMBOL_ALL 0x0508
#define SYMBOL_1SHUF 0x0510
#define SYMBOL_SHUF_TOP 0x0520
// PICTOGRAPH DISPLAY RAM ADDRESS 6
#define SYMBOL_REP_BOTTOM 0x0604
#define SYMBOL_ENTER 0x0640
// PICTOGRAPH DISPLAY RAM ADDRESS 7
#define SYMBOL_DBASS 0x0701
#define SYMBOL_DIGITAL 0x0702
#define SYMBOL_SHUF_BOTTOM 0x0710

void GPIO_Configuration(void);
void MDX7900R_init(void);
void MDX7900R_backlight(BACKLIGHT val);
void MDX7900R_send(uint8_t length);
void MDX7900R_text(uint8_t pos,char* text,uint8_t clear);
void MDX7900R_clear_text(uint8_t start);
void MDX7900R_clear_symbols(void);
void MDX7900R_fill_symbols(void);
void MDX7900R_symbol(uint16_t symbol,uint8_t on);
void MDX7900R_symbol_1(void);
void MDX7900R_symbol_2(void);
void MDX7900R_create_char(uint8_t pos,uint8_t* character);

#endif
