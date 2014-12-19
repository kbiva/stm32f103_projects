/*
 *  m66004.c
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "delay.h"
#include "m66004.h"

static PIN_SPI pins_spi[]={
  {{CS_Pin, CS_Speed, CS_Mode}, CS_Port, CS_Bus},
  {{SCLK_Pin,SCLK_Speed,SCLK_Mode},SCLK_Port,SCLK_Bus},
  {{MOSI_Pin,MOSI_Speed,MOSI_Mode},MOSI_Port,MOSI_Bus},
  {{MISO_Pin,MISO_Speed,MISO_Mode},MISO_Port,MISO_Bus},
  {{LATCH_Pin, LATCH_Speed, LATCH_Mode}, LATCH_Port, LATCH_Bus},
};

uint8_t inputValues;
uint16_t inputVal;

static uint8_t _numColumns;

static void sendByte(uint8_t dataToWrite) {

  GPIO_ResetBits(CS_Port,CS_Pin);
  GPIO_SetBits(LATCH_Port,LATCH_Pin);

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,dataToWrite);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  inputValues = SPI_I2S_ReceiveData(SPI2);

  GPIO_ResetBits(LATCH_Port,LATCH_Pin);
  GPIO_SetBits(CS_Port,CS_Pin);

  inputVal=(inputValues==0x82)?0x100:(uint16_t)inputValues;
  DWT_Delay(50);
}

static void sendBytes(uint8_t dataToWrite) {

  GPIO_SetBits(LATCH_Port,LATCH_Pin);

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,dataToWrite);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  inputValues = SPI_I2S_ReceiveData(SPI2);

  GPIO_ResetBits(LATCH_Port,LATCH_Pin);

  inputVal=(inputValues==0x82)?0x100:(uint16_t)inputValues;
  DWT_Delay(50);
}

static void setCursor(uint8_t column) {

  uint8_t location = column;
  if (location >= _numColumns) {
    location = 0;
  }
  sendByte(SET_CURSOR_LOCATION | location);
}

static void write(uint8_t character) {

  if (character <= 0x0F) {
    character += USER_DEFINED;
  }

  if (character >= ' ') {// Ignore ASCII control codes.
    // Map anything hitting a hardware control code to draw a solid block.
    if ((character>=0x80 && character<=0x8F) || (character>=0xE0)) {
      character = 0x7F;
    }

    sendByte(character);
  }
}

void GPIO_Configuration(void) {

  uint32_t i;

  SPI_InitTypeDef  SPI_InitStructure;

  for(i=0;i<sizeof(pins_spi)/sizeof(PIN_SPI);i++) {
    RCC_APB2PeriphClockCmd(pins_spi[i].GPIO_Bus,ENABLE);
    GPIO_Init(pins_spi[i].GPIOx,&pins_spi[i].GPIO_InitStructure);
  }

  GPIO_SetBits(CS_Port,CS_Pin);
  //GPIO_SetBits(SCLK_Port,SCLK_Pin);
  //GPIO_SetBits(MOSI_Port,MOSI_Pin);
  //GPIO_SetBits(MISO_Port,MISO_Pin);
  GPIO_ResetBits(LATCH_Port,LATCH_Pin);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  SPI_Cmd(SPI2, ENABLE);
}

static const uint8_t PING_TEST_BYTE_0 = 103; // Just a random number.
static const uint8_t PING_TEST_BYTE_1 = 242; // Just another random number.

uint8_t HP_5N_attached(void) {

  uint8_t isAttached = TRUE;

  GPIO_ResetBits(CS_Port,CS_Pin);
  GPIO_SetBits(LATCH_Port,LATCH_Pin);

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,PING_TEST_BYTE_0);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  inputValues = SPI_I2S_ReceiveData(SPI2);
  GPIO_SetBits(CS_Port,CS_Pin);

  DWT_Delay(30);

  GPIO_ResetBits(CS_Port,CS_Pin);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,PING_TEST_BYTE_1);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  inputValues = SPI_I2S_ReceiveData(SPI2);
  GPIO_SetBits(CS_Port,CS_Pin);

  DWT_Delay(30);

  if (inputValues!=PING_TEST_BYTE_0) {
    isAttached = FALSE;
  }

  GPIO_ResetBits(CS_Port,CS_Pin);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){};
  SPI_I2S_SendData(SPI2,NOP);
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){};
  inputValues = SPI_I2S_ReceiveData(SPI2);
  GPIO_SetBits(CS_Port,CS_Pin);

  DWT_Delay(30);

  if (inputValues!=PING_TEST_BYTE_1) {
    isAttached = FALSE;
  }

  // Restore latch back to low.
  GPIO_ResetBits(LATCH_Port,LATCH_Pin);

  return isAttached;
}

void HP_5N_init(uint8_t numColumns) {

  _numColumns=numColumns;

  // Initialize the VFD back to our desired default state.
  sendByte(SET_DISPLAY_LENGTH | (numColumns - 9));
  M66004_dimmer(DIMMER_14_16); // Full brightness.
  sendByte(SET_REFRESH_FREQUENCY | REFRESH_128_fOSC); // Opt for fastest refresh rate.
  sendByte(SET_AUTO_INCREMENT | AUTOINCREMENT_ON);

  M66004_clear(1);
  M66004_display();
}

void M66004_clear(uint8_t val) {

  uint8_t location;

  sendByte(SET_CURSOR_LOCATION | 0);
  for (location = 0; location < _numColumns; location ++) {
    sendByte(' ');
  }
  if(val)
  for (location = 0; location < _numColumns; location ++) {
      sendByte(UNDERSCORE_OFF | location);
  }
}

void M66004_noDisplay(void) {

  sendByte(DISPLAY_ON | NO_DISPLAY);
}

void M66004_display(void) {

  sendByte(DISPLAY_ON | NORMAL_DISPLAY);
}

void M66004_allDisplay(void) {

  sendByte(DISPLAY_ON | ALL_DISPLAY);
}

void M66004_dimmer(uint8_t value){

  sendByte(SET_DIMMER | value);
}

void M66004_noUnderscore(uint8_t location) {

  sendByte(UNDERSCORE_OFF | location);
}

void M66004_underscore(uint8_t location) {

  sendByte(UNDERSCORE_ON | location);
}

void M66004_createChar(uint8_t characterIndex, uint8_t pixels[]) {

  uint8_t testBit = 0x10;
  uint8_t i,j;

  if (characterIndex <= 0x0F) {

    GPIO_ResetBits(CS_Port,CS_Pin);
    sendBytes(SET_USER_DEFINED_DATA);
    sendBytes(characterIndex);

    // We need to rotate the pixels.
    for (i=0; i<CHARACTER_PIXEL_WIDTH; i++) {

      uint8_t currentByte = 0;

      for (j=0; j<CHARACTER_PIXEL_PRETEND_HEIGHT; j++) {

        currentByte <<= 1;

        if (pixels[j] & testBit) {
          currentByte |= 1;
        }
      }

      sendBytes(currentByte);
      testBit >>= 1;
    }
    GPIO_SetBits(CS_Port,CS_Pin);
  }
}

void M66004_displayCharAt(uint8_t column,uint8_t character) {

  setCursor(column);
  write(character);
}

void M66004_printAt(uint8_t column,uint8_t clr,char *str) {

  uint8_t i=column;

  setCursor(column);
  while (*str) {
    write(*str++);
    i++;
  }
  if(clr) {
    while(i++<_numColumns) {
      write(' ');
    }
  }
}
