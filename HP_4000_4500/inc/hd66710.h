/*
 *  hd66710.h
 *
 *  Author: Kestutis Bivainis
 */

#ifndef HD66710_H
#define HD66710_H

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
  HP4000_BUTTON_MENU_LEFT   = 0x0080,
  HP4000_BUTTON_MENU_RIGHT  = 0x0100,
  HP4000_BUTTON_ITEM_LEFT   = 0x0200,
  HP4000_BUTTON_ITEM_RIGHT  = 0x0400,
  HP4000_BUTTON_VALUE_MINUS = 0x0800,
  HP4000_BUTTON_VALUE_PLUS  = 0x1000,
  HP4000_BUTTON_SELECT      = 0x2000,
  HP4000_BUTTON_GO          = 0x4000,
  HP4000_BUTTON_CANCEL_JOB  = 0x8000,
} HP4000_BUTTONS;

typedef enum _LEDS {
  HP4000_BACKLIGHT  = 0x01,
  HP4000_LED_GREEN1 = 0x08,
  HP4000_LED_GREEN2 = 0x10,
  HP4000_LED_ORANGE = 0x20,
} HP4000_LEDS;


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
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
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
#define LCD_REGISTER_EXTENSION 0x04
#define LCD_BLINK_ENABLE 0x02
#define LCD_LOW_POWER_MODE 0x01

#define LOW 0
#define HIGH 1

#define FALSE 0
#define TRUE 1

void GPIO_Configuration(void);
void HD66710_init(void);

void HD66710_clear(void);
void HD66710_home(void);

void HD66710_display(uint8_t value);
void HD66710_blink(uint8_t value);
void HD66710_cursor(uint8_t value);
void HD66710_scrollDisplayLeft(void);
void HD66710_scrollDisplayRight(void);
void HD66710_leftToRight(void);
void HD66710_rightToLeft(void);
void HD66710_autoscroll(uint8_t value);

void HD66710_createChar(uint8_t code, const uint8_t *charmap);
void HD66710_setCursor(uint8_t column, uint8_t row);

void HD66710_print(char *str);
void HD66710_printAt(uint8_t col,uint8_t row,char *str);
void HD66710_displayChar(uint8_t location);
void HD66710_displayCharAt(uint8_t col,uint8_t row,uint8_t location);

  // Commands new over what is in LiquidCrystal.
void HD66710_extendedCharacters(uint8_t value);
void HD66710_extendedCommand(uint8_t c);

uint8_t HP4000_isButtonPressed(HP4000_BUTTONS button);
uint8_t HP4000_wasButtonPressed(HP4000_BUTTONS button); // Latched, clears state for button when read.
uint8_t HP4000_wasButtonReleased(HP4000_BUTTONS button); // Latched, clears state for button when read.

uint8_t HP4000_getLedState(HP4000_LEDS led);
void HP4000_setLedState(HP4000_LEDS led, uint8_t level);

#endif
