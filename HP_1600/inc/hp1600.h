/*
 *  hp1600.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef HP1600_H
#define HP1600_H

#include "stm32f10x_conf.h"

#define CS_Pin        GPIO_Pin_4            // CS = PA[4]
#define CS_Speed      GPIO_Speed_50MHz
#define CS_Mode       GPIO_Mode_Out_PP
#define CS_Port       GPIOA
#define CS_Bus        RCC_APB2Periph_GPIOA

#define SCLK_Pin      GPIO_Pin_5            // SCLK = PA[5]
#define SCLK_Speed    GPIO_Speed_50MHz
#define SCLK_Mode     GPIO_Mode_AF_PP
#define SCLK_Port     GPIOA
#define SCLK_Bus      RCC_APB2Periph_GPIOA

#define MOSI_Pin      GPIO_Pin_7            // MOSI = PA[7]
#define MOSI_Speed    GPIO_Speed_50MHz
#define MOSI_Mode     GPIO_Mode_AF_PP
#define MOSI_Port     GPIOA
#define MOSI_Bus      RCC_APB2Periph_GPIOA

#define MISO_Pin      GPIO_Pin_6            // MISO = PA[6]
#define MISO_Speed    GPIO_Speed_50MHz
#define MISO_Mode     GPIO_Mode_IN_FLOATING
#define MISO_Port     GPIOA
#define MISO_Bus      RCC_APB2Periph_GPIOA

typedef struct _PIN_SPI {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN_SPI;

typedef enum _BUTTONS {
  HP1600_BUTTON1=0,
  HP1600_BUTTON2=1,
  HP1600_BUTTON3=2,
  HP1600_BUTTON4=3,
} HP1600_BUTTONS;

typedef enum _LEDS {
  HP1600_LEDS_OFF=0,
  HP1600_LED_GREEN=0x10,
  HP1600_LED_YELLOW=0x20,
  HP1600_LEDS_ON=HP1600_LED_GREEN|HP1600_LED_YELLOW,
} HP1600_LEDS;

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_EXTENSION_FUNCTION_SET 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80
#define LCD_EXTENSION_SETSEGRAMADDR 0x80

// flags for display entry mode 0x04
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control 0x08
#define LCD_DISPLAYON 0x04
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x0
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift 0x10
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set 0x20
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_FONT_BIG 0x04
#define LCD_FONT_SMALL 0x00
#define LCD_BLINK_ENABLE 0x02
#define LCD_LOW_POWER_MODE 0x01

#define POLL    0x0700
#define COMMAND 0x2700
#define DATA    0x6700

#define FALSE 0
#define TRUE 1

#define MODE_SMALL 0
#define MODE_BIG 1

void GPIO_Configuration(void);
void HP1600_init(uint8_t mode);
void HP1600_createChar(uint8_t location, const uint8_t *charmap);
void HP1600_displayChar(uint8_t location);
void HP1600_displayCharAt(uint8_t col,uint8_t row,uint8_t location);
void HP1600_leds(HP1600_LEDS value);
void HP1600_print(char *str);
void HP1600_printAt(uint8_t col,uint8_t row,char *str);
void HP1600_setCursor(uint8_t col,uint8_t row);
void HP1600_clear(void);
void HP1600_home(void);
void HP1600_clear_home(void);
void HP1600_display(uint8_t value);
void HP1600_cursor(uint8_t value);
void HP1600_blink(uint8_t value);

void HP1600_scrollDisplayLeft(void);
void HP1600_scrollDisplayRight(void);
void HP1600_leftToRight(void);
void HP1600_rightToLeft(void);
void HP1600_autoscroll(uint8_t value);
uint8_t HP1600_isButtonPressed(HP1600_BUTTONS buttonIndex);
uint8_t HP1600_wasButtonPressed(HP1600_BUTTONS buttonIndex);
uint8_t HP1600_wasButtonReleased(HP1600_BUTTONS buttonIndex);
void HP1600_updateButtons(void);

#endif
