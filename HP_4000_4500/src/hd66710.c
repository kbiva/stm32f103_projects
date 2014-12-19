/*
 *  hd66710.c
 *
 *  Author: Kestutis Bivainis
 */

#include "hd66710.h"
#include "stm32f10x_conf.h"
#include "delay.h"

static PIN_SPI pins_spi[]={
  {{CS_Pin, CS_Speed, CS_Mode}, CS_Port, CS_Bus},
  {{SCLK_Pin,SCLK_Speed,SCLK_Mode},SCLK_Port,SCLK_Bus},
  {{MOSI_Pin,MOSI_Speed,MOSI_Mode},MOSI_Port,MOSI_Bus},
  {{MISO_Pin,MISO_Speed,MISO_Mode},MISO_Port,MISO_Bus},
};

uint8_t _displaycontrol;
uint8_t _displaymode;

uint8_t _digitalSwitchMask;

uint8_t _numlines;
uint8_t _currline;

uint8_t _useExtendedCharacters;
uint8_t _displayfunction;

volatile uint16_t inputValues;

// LED state.
uint8_t _currentLedValues;

// Button reading state.
unsigned long _lastReadMark;
uint16_t _currentButtonState;
uint16_t _pressedState;
uint16_t _releasedState;

extern volatile uint32_t millis;

/*
Description    20 pin ribbon connector
GND               19
3.3V              18
5V                20
SEL               14                        SS        PA4
CLK               15                        SCLK      PA5
DIN               17                        MOSI      PA7
DOUT              16                        MISO      PA6
*/

static void HD66710_sendByte(uint8_t value, uint8_t mode) {

  uint16_t wordValue = (mode) ? 0 : 0x80; // State of the RS (0 - Data, 1 - Instructions)
  wordValue |= (uint16_t)_digitalSwitchMask;
  wordValue |= ((uint16_t)(~value)<<8);

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI1,wordValue);

  GPIO_SetBits(CS_Port,CS_Pin);
  GPIO_ResetBits(CS_Port,CS_Pin);

  DWT_Delay(100);   // commands need > 37us to settle
}


static void HD66710_command(uint8_t value) {

  HD66710_sendByte(value, LOW);
}

static void updateButtons(void) {

  uint16_t changedBits;
  uint16_t newButtonState;

  HD66710_command(0);
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

void GPIO_Configuration(void) {

  uint32_t i;

  SPI_InitTypeDef  SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  for(i=0;i<sizeof(pins_spi)/sizeof(PIN_SPI);i++) {
    RCC_APB2PeriphClockCmd(pins_spi[i].GPIO_Bus,ENABLE);
    GPIO_Init(pins_spi[i].GPIOx,&pins_spi[i].GPIO_InitStructure);
  }

  GPIO_SetBits(CS_Port,CS_Pin);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  // HP LaserJet 4000/4050/4100 - SPI_CPHA_1Edge
  // HP LaserJet 4500 - SPI_CPHA_2Edge
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  // HP LaserJet 4000/4050/4100 - SPI_BaudRatePrescaler_256
  // HP LaserJet 4500 - SPI_BaudRatePrescaler_128
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
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


void HD66710_init(void) {

  _useExtendedCharacters = 0;

  // LEDS off, backlight off
  _currentLedValues =  (HP4000_LED_GREEN1 |
                        HP4000_LED_GREEN2 |
                        HP4000_LED_ORANGE) &
                       ~HP4000_BACKLIGHT;

  _digitalSwitchMask = _currentLedValues;

  // Buttons are not pressed
  _currentButtonState = HP4000_BUTTON_MENU_LEFT   |
                        HP4000_BUTTON_MENU_RIGHT  |
                        HP4000_BUTTON_ITEM_LEFT   |
                        HP4000_BUTTON_ITEM_RIGHT  |
                        HP4000_BUTTON_VALUE_MINUS |
                        HP4000_BUTTON_VALUE_PLUS  |
                        HP4000_BUTTON_SELECT      |
                        HP4000_BUTTON_GO          |
                        HP4000_BUTTON_CANCEL_JOB;

  _pressedState = 0;
  _releasedState = 0;

  _numlines = 16;
  _currline = 0;

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
  DWT_Delay(50000);

  //put the LCD into 4 bit mode

  // this is according to the hitachi HD44780 datasheet
  // page 45 figure 23

  _displayfunction = LCD_4BITMODE | LCD_2LINE;

  // Send function set command sequence
  HD66710_command(LCD_FUNCTIONSET | _displayfunction);
  DWT_Delay(5000);  // wait more than 4.1ms

  // second try
  HD66710_command(LCD_FUNCTIONSET | _displayfunction);
  DWT_Delay(50000);

  // third go
  HD66710_command(LCD_FUNCTIONSET | _displayfunction);

  // Need to pop in and out of extended function for contrast to be correct.
  // Not sure why.
  HD66710_command(LCD_FUNCTIONSET | LCD_REGISTER_EXTENSION | LCD_REGISTER_EXTENSION );
  HD66710_command(LCD_FUNCTIONSET | LCD_REGISTER_EXTENSION);

  // turn the display on with no cursor or blinking default
  _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  HD66710_display(TRUE);

  // clear it off
  HD66710_clear();

  // Initialize to default text direction (for romance languages)
  _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  HD66710_command(LCD_ENTRYMODESET | _displaymode);

  // Clear extension function set.
  HD66710_extendedCommand(LCD_EXTENSION_FUNCTION_SET | 0);

  // Zero any extended addresses.
  HD66710_extendedCommand(LCD_EXTENSION_SETSEGRAMADDR | 0);
}

void HD66710_extendedCommand(uint8_t c) {

  HD66710_command(LCD_FUNCTIONSET | _displayfunction | LCD_REGISTER_EXTENSION);
  HD66710_command(c);
  HD66710_command(LCD_FUNCTIONSET | _displayfunction);
}

void HD66710_clear(void) {

  HD66710_command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
  DWT_Delay(2000);  // this command takes a long time!
}

void HD66710_home(void) {

  HD66710_command(LCD_RETURNHOME);  // set cursor position to zero
  DWT_Delay(2000);  // this command takes a long time!
}

void HD66710_setCursor(uint8_t col, uint8_t row) {

  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if ( row > _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }
  HD66710_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void HD66710_display(uint8_t value) {

  if(value)
    _displaycontrol |= LCD_DISPLAYON;
  else
    _displaycontrol &= ~LCD_DISPLAYON;
  HD66710_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void HD66710_cursor(uint8_t value) {

  if(value)
    _displaycontrol |= LCD_CURSORON;
  else
    _displaycontrol &= ~LCD_CURSORON;
  HD66710_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void HD66710_blink(uint8_t value) {

  if(value)
    _displaycontrol |= LCD_BLINKON;
  else
    _displaycontrol &= ~LCD_BLINKON;
  HD66710_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void HD66710_scrollDisplayLeft(void) {

  HD66710_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void HD66710_scrollDisplayRight(void) {

  HD66710_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void HD66710_leftToRight(void) {

  _displaymode |= LCD_ENTRYLEFT;
  HD66710_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void HD66710_rightToLeft(void) {

  _displaymode &= ~LCD_ENTRYLEFT;
  HD66710_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right/left justify' text from the cursor
void HD66710_autoscroll(uint8_t value) {

  if(value)
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  else
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  HD66710_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void HD66710_createChar(uint8_t location, const uint8_t *charmap) {

  uint8_t i;
  location &= 0x07; // we only have 8 locations 0-7
  HD66710_command(LCD_SETCGRAMADDR | (location << 3));
  for (i=0; i<8; i++) {
    HD66710_sendByte(charmap[i], HIGH);
  }
}

void HD66710_extendedCharacters(uint8_t value) {

  if(value)
    _useExtendedCharacters = 1;
  else
    _useExtendedCharacters = 0;
}

void HD66710_print(char *str) {

  char c;

  while (*str) {

    c=*str++;
    if (!_useExtendedCharacters) {
      if ((c>='a') && (c<='z'))  {
        c -= 'a' - 'A';
      }
    }
    HD66710_sendByte(c, HIGH);
  }
}

void HD66710_printAt(uint8_t col,uint8_t row,char *str) {

  HD66710_setCursor(col, row);
  HD66710_print(str);
}

void HD66710_displayChar(uint8_t location) {

  location &= 0x07;
  HD66710_sendByte(location,HIGH);
}

void HD66710_displayCharAt(uint8_t col,uint8_t row,uint8_t location) {

  HD66710_setCursor(col, row);
  HD66710_displayChar(location);
}

uint8_t HP4000_isButtonPressed(HP4000_BUTTONS button) {

  updateButtons();
  return (_currentButtonState & button) != 0;
}

uint8_t HP4000_wasButtonPressed(HP4000_BUTTONS button) {

  uint16_t pressedStateTmp;

  updateButtons();

  pressedStateTmp = _pressedState;
  _pressedState &= ~button;
  return (pressedStateTmp & button) != 0;
}

uint8_t HP4000_wasButtonReleased(HP4000_BUTTONS button) {

  uint16_t releasedStateTmp;

  updateButtons();

  releasedStateTmp = _releasedState;
  _releasedState &= ~button;
  return (releasedStateTmp & button) != 0;
}

uint8_t HP4000_getLedState(HP4000_LEDS led) {

  return (led & _currentLedValues) ? LOW : HIGH; // Led on/off is inverted.
}

void HP4000_setLedState(HP4000_LEDS led, uint8_t level) {
  uint8_t currentLedValuesTmp = _currentLedValues;

  if (level == LOW) {
    // Led on/off is inverted.
    currentLedValuesTmp |= led;
  }
  else {
    // Led on/off is inverted.
    currentLedValuesTmp &= ~led;
  }

  if (currentLedValuesTmp != _currentLedValues) {
    _currentLedValues = currentLedValuesTmp;
    _digitalSwitchMask &= ~0x39;
    _digitalSwitchMask |= _currentLedValues;
    // Send a NOP command which will also set the bits.
    HD66710_command(0);
  }
}
