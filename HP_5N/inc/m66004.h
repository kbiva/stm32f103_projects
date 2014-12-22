/*
 *  m66004.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef M66004_H
#define M66004_H

#include "stm32f10x_conf.h"

#define CS_Pin        GPIO_Pin_12            // CS = PB[12]
#define CS_Speed      GPIO_Speed_50MHz
#define CS_Mode       GPIO_Mode_Out_PP
#define CS_Port       GPIOB
#define CS_Bus        RCC_APB2Periph_GPIOB

#define SCLK_Pin      GPIO_Pin_13            // SCLK = PB[13]
#define SCLK_Speed    GPIO_Speed_50MHz
#define SCLK_Mode     GPIO_Mode_AF_PP
#define SCLK_Port     GPIOB
#define SCLK_Bus      RCC_APB2Periph_GPIOB

#define MISO_Pin      GPIO_Pin_14            // MISO = PB[14]
#define MISO_Speed    GPIO_Speed_50MHz
#define MISO_Mode     GPIO_Mode_IN_FLOATING
#define MISO_Port     GPIOB
#define MISO_Bus      RCC_APB2Periph_GPIOB

#define MOSI_Pin      GPIO_Pin_15            // MOSI = PB[15]
#define MOSI_Speed    GPIO_Speed_50MHz
#define MOSI_Mode     GPIO_Mode_AF_PP
#define MOSI_Port     GPIOB
#define MOSI_Bus      RCC_APB2Periph_GPIOB

#define LATCH_Pin     GPIO_Pin_10            // Latch = PC[10]
#define LATCH_Speed   GPIO_Speed_50MHz
#define LATCH_Mode    GPIO_Mode_Out_PP
#define LATCH_Port    GPIOC
#define LATCH_Bus     RCC_APB2Periph_GPIOC

typedef struct _PIN_SPI {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN_SPI;

typedef enum _BUTTONS {
  HP5N_BUTTON_MENU_LEFT   = 0x80,
  HP5N_BUTTON_MENU_RIGHT  = 0x02,
  HP5N_BUTTON_ITEM_LEFT   = 0x20,
  HP5N_BUTTON_ITEM_RIGHT  = 0x04,
  HP5N_BUTTON_VALUE_MINUS = 0x10,
  HP5N_BUTTON_VALUE_PLUS  = 0x01,
  HP5N_BUTTON_SELECT      = 0x08,
  HP5N_BUTTON_GO          = 0x40,
  HP5N_BUTTON_JOB_CANCEL  = 0x100,
} HP5N_BUTTONS;

// commands
#define SET_DISPLAY_LENGTH 0x00
#define SET_DIMMER 0x08
#define UNDERSCORE_ON 0x10
#define UNDERSCORE_OFF 0x80
#define USER_DEFINED 0x90
#define SET_CURSOR_LOCATION 0xE0
#define DISPLAY_ON 0xF0
#define SET_AUTO_INCREMENT 0xF4
#define SET_REFRESH_FREQUENCY 0xF6
#define SET_OUTPUT_PORT_CHARACTER 0xF8
#define SET_USER_DEFINED_DATA 0xFC

// flags for DISPLAY_ON 0xF0
#define NO_DISPLAY 0x00
#define NORMAL_DISPLAY 0x01
#define ALL_DISPLAY 0x03

// flags for SET_DIMMER 0x08
#define DIMMER_1_16 0x00
#define DIMMER_2_16 0x01
#define DIMMER_4_16 0x02
#define DIMMER_6_16 0x03
#define DIMMER_8_16 0x04
#define DIMMER_10_16 0x05
#define DIMMER_12_16 0x06
#define DIMMER_14_16 0x07

// flags for SET_REFRESH_FREQUENCY 0xF6
#define REFRESH_128_fOSC 0x00
#define REFRESH_256_fOSC 0x01

// flags for SET_AUTO_INCREMENT 0xF4
#define AUTOINCREMENT_OFF 0x00
#define AUTOINCREMENT_ON 0x01

#define NOP (DISPLAY_ON|NORMAL_DISPLAY)

#define CHARACTER_PIXEL_WIDTH 5
#define CHARACTER_PIXEL_PRETEND_HEIGHT 8
#define CHARACTER_PIXEL_ACTUAL_HEIGHT 7

#define FALSE 0
#define TRUE 1

uint8_t HP_5N_attached(void);
void HP_5N_init(uint8_t numColumns);
void GPIO_Configuration(void);

void M66004_clear(uint8_t val);

void M66004_noDisplay(void);
void M66004_display(void);
void M66004_allDisplay(void); //Forces all pixels to be set. Cancel with noDisplay() or display()

void M66004_dimmer(uint8_t value);
void M66004_noUnderscore(uint8_t column);
void M66004_underscore(uint8_t column);

void M66004_createChar(uint8_t characterIndex, uint8_t pixels[] /* 8 bytes, 1 for each row */);

void M66004_displayCharAt(uint8_t column,uint8_t character);
void M66004_printAt(uint8_t column,uint8_t clr,char *str);

#endif
