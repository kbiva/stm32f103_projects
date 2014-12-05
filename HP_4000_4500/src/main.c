/*
 *  main.c
 *
 *  Author: Kestutis Bivainis
 */

#include "hd66710.h"
#include "delay.h"
#include "stm32f10x_conf.h"

uint8_t openHeart[8] = {0x00,0x0A,0x15,0x11,0x0A,0x04,0x00,0x00};
uint8_t filledHeart[8] = {0x00,0x0A,0x1F,0x1F,0x0E,0x04,0x00,0x00};

void printBits(uint16_t value, uint8_t bitCount) {

  uint16_t mask;
  for (mask = (1<<(uint16_t)(bitCount-1)); mask; mask >>= 1) {
    HD66710_displayChar((value & mask) ? 1 : 0);
  }
}


void Int2Str(char* str, int32_t intnum) {

  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++) {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0)) {
      j = 0;
    }
    else {
      Status++;
    }
  }
}

volatile uint32_t millis;

char buf[16];

extern volatile uint16_t inputValues;

int main(void) {

  DWT_Init();

  DWT_Delay(200000);
  SysTick_Config(SystemCoreClock/1000);
  GPIO_Configuration();
  HD66710_init();

  // LEDS off, backlight on
  HP4000_setLedState(HP4000_LED_GREEN1,LOW);
  HP4000_setLedState(HP4000_LED_GREEN2,LOW);
  HP4000_setLedState(HP4000_LED_ORANGE,LOW);
  HP4000_setLedState(HP4000_BACKLIGHT,LOW);

  HD66710_createChar(0, openHeart);
  HD66710_createChar(1, filledHeart);
  HD66710_clear();
  HD66710_display(TRUE);
  HD66710_print("hello world!");

  while(1){

    //HD66710_setCursor(0, 0);
    //print("In: ");
    //printBits(inputValues,16);
    //continue;
/*
    clear();
      home();
      display();
      extendedCharacters();
      for(j=0x00;j<0x10;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x10;j<0x20;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x20;j<0x30;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x30;j<0x40;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x40;j<0x50;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x50;j<0x60;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x60;j<0x70;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x70;j<0x80;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x80;j<0x90;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0x90;j<0xA0;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0xA0;j<0xB0;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0xB0;j<0xC0;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0xC0;j<0xD0;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0xD0;j<0xE0;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0xE0;j<0xF0;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
      for(j=0xF0;j<=0xFE;j++){write(j);};
      delay_ms(1000);
      clear();
      home();
      display();
*/



    if (HP4000_wasButtonPressed(HP4000_BUTTON_MENU_LEFT)) {
      // Toggle LED0.
      HP4000_setLedState(HP4000_LED_GREEN1, (HP4000_getLedState(HP4000_LED_GREEN1) == LOW) ? HIGH : LOW);
    }

    if (HP4000_wasButtonPressed(HP4000_BUTTON_ITEM_LEFT)) {
      // Toggle LED1.
      HP4000_setLedState(HP4000_LED_GREEN2, (HP4000_getLedState(HP4000_LED_GREEN2) == LOW) ? HIGH : LOW);
    }

    if (HP4000_wasButtonPressed(HP4000_BUTTON_VALUE_MINUS)) {
      // Toggle LED2.
      HP4000_setLedState(HP4000_LED_ORANGE, (HP4000_getLedState(HP4000_LED_ORANGE) == LOW) ? HIGH : LOW);
    }

    if (HP4000_isButtonPressed(HP4000_BUTTON_MENU_RIGHT)) {
      // Set LED0.
      HP4000_setLedState(HP4000_LED_GREEN1, HIGH);
    }

    if (HP4000_isButtonPressed(HP4000_BUTTON_ITEM_RIGHT)) {
      // Set LED1.
      HP4000_setLedState(HP4000_LED_GREEN2, HIGH);
    }

    if (HP4000_isButtonPressed(HP4000_BUTTON_VALUE_PLUS)) {
      // Set LED2.
      HP4000_setLedState(HP4000_LED_ORANGE, HIGH);
    }

    if (HP4000_wasButtonReleased(HP4000_BUTTON_MENU_RIGHT)) {
      // Clear LED0.
      HP4000_setLedState(HP4000_LED_GREEN1, LOW);
    }

    if (HP4000_wasButtonReleased(HP4000_BUTTON_ITEM_RIGHT)) {
      // Clear LED1.
      HP4000_setLedState(HP4000_LED_GREEN2, LOW);
    }

    if (HP4000_wasButtonReleased(HP4000_BUTTON_VALUE_PLUS)) {
      // Clear LED2.
      HP4000_setLedState(HP4000_LED_ORANGE, LOW);
    }

    if (HP4000_wasButtonPressed(HP4000_BUTTON_SELECT)) {
      // Toggle all LEDs.
      HP4000_setLedState(HP4000_LED_GREEN1, (HP4000_getLedState(HP4000_LED_GREEN1) == LOW) ? HIGH : LOW);
      HP4000_setLedState(HP4000_LED_GREEN2, (HP4000_getLedState(HP4000_LED_GREEN2) == LOW) ? HIGH : LOW);
      HP4000_setLedState(HP4000_LED_ORANGE, (HP4000_getLedState(HP4000_LED_ORANGE) == LOW) ? HIGH : LOW);
    }

    // Turn backlight on/off.
    if (HP4000_wasButtonPressed(HP4000_BUTTON_GO)) {
      HP4000_setLedState(HP4000_BACKLIGHT,LOW);
    }

    if (HP4000_wasButtonPressed(HP4000_BUTTON_CANCEL_JOB)) {
      HP4000_setLedState(HP4000_BACKLIGHT,HIGH);
    }

    HD66710_displayCharAt(15,1,((millis % 1000) < 250) ? 1 : 0);

    Int2Str(buf,millis);
    HD66710_printAt(0,1,buf);

  };
}
