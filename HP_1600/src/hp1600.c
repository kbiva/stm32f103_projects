/*
 *  hp1600.h
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "hp1600.h"
#include "delay.h"

volatile uint16_t inputValues;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _displayfunction;

uint16_t _currentButtonState;
uint16_t _pressedState;
uint16_t _releasedState;

static PIN_SPI pins_spi[]={
  {{CS_Pin, CS_Speed, CS_Mode}, CS_Port, CS_Bus},
  {{SCLK_Pin,SCLK_Speed,SCLK_Mode},SCLK_Port,SCLK_Bus, SCLK_AFIO_Bus},
  {{MOSI_Pin,MOSI_Speed,MOSI_Mode},MOSI_Port,MOSI_Bus, MOSI_AFIO_Bus},
  {{MISO_Pin,MISO_Speed,MISO_Mode},MISO_Port,MISO_Bus, MISO_AFIO_Bus},
};

static void HP1600_sendByte(uint8_t value, uint16_t mode) {

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  GPIO_ResetBits(CS_Port,CS_Pin);
  SPI_I2S_SendData(SPI1,mode|(value&0xF0));
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(CS_Port,CS_Pin);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  GPIO_ResetBits(CS_Port,CS_Pin);
  SPI_I2S_SendData(SPI1,mode|((value&0x0F)<<4));
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(CS_Port,CS_Pin);

  DWT_Delay(20);
}

static void HP1600_command(uint8_t value) {

  HP1600_sendByte(value, COMMAND);
}

void GPIO_Configuration(void) {

  uint32_t i;

  SPI_InitTypeDef  SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  for(i=0;i<sizeof(pins_spi)/sizeof(PIN_SPI);i++) {
    RCC_APB2PeriphClockCmd(pins_spi[i].GPIO_Bus,ENABLE);
    if(pins_spi[i].AFIO_Bus)
      RCC_APB2PeriphClockCmd(pins_spi[i].AFIO_Bus,ENABLE);
    GPIO_Init(pins_spi[i].GPIOx,&pins_spi[i].GPIO_InitStructure);
  }

  GPIO_SetBits(CS_Port,CS_Pin);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  SPI_Cmd(SPI1, ENABLE);
  SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void HP1600_init(uint8_t mode) {

  // Six magic initialization bytes
  HP1600_command(0x32);
  HP1600_command(0x29);
  HP1600_command(0x18);
  HP1600_command(0x5e);
  HP1600_command(0x75);
  HP1600_command(0x6c);

  HP1600_display(TRUE);

  if(mode==MODE_SMALL) {
    // 2x16 font 7x5
    _displayfunction = LCD_4BITMODE | LCD_2LINE;
  }
  else if(mode==MODE_BIG) {
    // 1x16 font 14x5 ( doubled )
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_FONT_BIG;
  }

  HP1600_command(LCD_FUNCTIONSET | _displayfunction);

}

void HP1600_clear(void) {

  HP1600_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  DWT_Delay(1000);
}

void HP1600_home(void) {

  HP1600_command(LCD_RETURNHOME);  // set cursor position to zero
  DWT_Delay(1000);
}

void HP1600_clear_home(void) {
  HP1600_clear();
  HP1600_home();
}

void HP1600_setCursor(uint8_t col, uint8_t row) {

  int row_offsets[] = { 0x00, 0x40 };
  HP1600_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}


void HP1600_print(char *str) {

  while (*str)
    HP1600_sendByte(*str++,DATA);
}

void HP1600_printAt(uint8_t col,uint8_t row,char *str) {

  HP1600_setCursor(col, row);
  HP1600_print(str);
}

void HP1600_display(uint8_t value) {

  if(value)
    _displaycontrol |= LCD_DISPLAYON;
  else
    _displaycontrol &= ~LCD_DISPLAYON;
  HP1600_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void HP1600_cursor(uint8_t value) {

  if(value)
    _displaycontrol |= LCD_CURSORON;
  else
    _displaycontrol &= ~LCD_CURSORON;
  HP1600_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void HP1600_blink(uint8_t value) {

  if(value)
    _displaycontrol |= LCD_BLINKON;
  else
    _displaycontrol &= ~LCD_BLINKON;
  HP1600_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// This is for text that flows Left to Right
void HP1600_leftToRight(void) {

  _displaymode |= LCD_ENTRYLEFT;
  HP1600_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void HP1600_rightToLeft(void) {

  _displaymode &= ~LCD_ENTRYLEFT;
  HP1600_command(LCD_ENTRYMODESET | _displaymode);
}

void HP1600_autoscroll(uint8_t value) {

  if(value)
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  else
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  HP1600_command(LCD_ENTRYMODESET | _displaymode);
}

// These commands scroll the display without changing the RAM
void HP1600_scrollDisplayLeft(void) {

  HP1600_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {

  HP1600_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void HP1600_createChar(uint8_t location, const uint8_t *charmap) {

  uint8_t i;
  location &= 0x07; // we only have 8 locations 0-7
  HP1600_command(LCD_SETCGRAMADDR | (location << 3));
  for (i=0; i<8; i++) {
    HP1600_sendByte(charmap[i],DATA);
  }
}

void HP1600_displayChar(uint8_t location) {

  location &= 0x07;
  HP1600_sendByte(location,DATA);
}

void HP1600_displayCharAt(uint8_t col,uint8_t row,uint8_t location) {

  HP1600_setCursor(col, row);
  HP1600_displayChar(location);
}

void HP1600_leds(HP1600_LEDS value) {

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  GPIO_ResetBits(CS_Port,CS_Pin);
  SPI_I2S_SendData(SPI1,0x9000|value);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  GPIO_SetBits(CS_Port,CS_Pin);

  DWT_Delay(50);
}

uint8_t HP1600_isButtonPressed(HP1600_BUTTONS buttonIndex) {

  uint16_t bitMask;
  HP1600_updateButtons();

  bitMask = (1 << buttonIndex);
  return (_currentButtonState & bitMask) != 0;
}

uint8_t HP1600_wasButtonPressed(HP1600_BUTTONS buttonIndex) {

  uint16_t bitMask;
  uint16_t pressedStateTmp;

  HP1600_updateButtons();

  bitMask = (1 << buttonIndex);
  pressedStateTmp = _pressedState;
  _pressedState &= ~bitMask;
  return (pressedStateTmp & bitMask) != 0;
}

uint8_t HP1600_wasButtonReleased(HP1600_BUTTONS buttonIndex) {

  uint16_t bitMask;
  uint16_t releasedStateTmp;

  HP1600_updateButtons();

  bitMask = (1 << buttonIndex);
  releasedStateTmp = _releasedState;
  _releasedState &= ~bitMask;
  return (releasedStateTmp & bitMask) != 0;
}

void HP1600_updateButtons() {

  uint16_t changedBits;
  uint16_t newButtonState;

  HP1600_sendByte(0,POLL);
  newButtonState = ~inputValues;

  if (newButtonState != _currentButtonState) {
    // Button state has changed.
    // Figure out pressed and released states.
    changedBits = newButtonState ^ _currentButtonState;
    _pressedState |= changedBits & newButtonState;
    _releasedState |= changedBits & _currentButtonState;

    // Save off new state.
    _currentButtonState = newButtonState;
  }
}
