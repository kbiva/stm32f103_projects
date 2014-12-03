#include "stm32f10x_conf.h"
#include "delay.h"
#include "hp1600.h"

extern uint16_t _pressedState;
extern uint16_t _releasedState;

void demo_small(void);
void demo_big(void);

volatile uint32_t millis;
char buf[16];

uint8_t openHeart[8] = {0x00,0x0A,0x15,0x11,0x0A,0x04,0x00,0x00};
uint8_t filledHeart[8] = {0x00,0x0A,0x1F,0x1F,0x0E,0x04,0x00,0x00};

void Int2Str(char* str, int32_t intnum) {

  uint32_t i,Div=1000000000,j=0,Status=0;

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

int main(void) {

  uint8_t mode=MODE_SMALL;

  SysTick_Config(SystemCoreClock/1000);

  DWT_Init();
  DWT_Delay(200000);

  GPIO_Configuration();

  HP1600_init(mode);

  HP1600_createChar(0, openHeart);
  HP1600_createChar(1, filledHeart);
  HP1600_clear();

  while(1) {
    if(mode==MODE_SMALL) {
      HP1600_printAt(0,0,"    HP Color    ");
      HP1600_printAt(0,1," LaserJet 1600  ");
      demo_small();
      mode=MODE_BIG;
    }
    else if(mode==MODE_BIG) {
      HP1600_printAt(0,0,"HPLaserJet 1600");
      demo_big();
      mode=MODE_SMALL;
    }
  }
}

void demo_small(void) {

  uint32_t mode=0,b1=0,b2=0,b3=0;

  while(1) {
    switch(mode) {
      case 0:
        if(HP1600_wasButtonPressed(HP1600_BUTTON1)) {
          HP1600_clear_home();
          HP1600_print("Button 1 pressed");
          HP1600_printAt(0,1,"LED On/off mode");
          b1=!b1;
          HP1600_leds(b1?HP1600_LED_GREEN:HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON2)) {
          HP1600_clear_home();
          HP1600_print("Button 2 pressed");
          HP1600_printAt(0,1,"LED On/off mode");
          b2=!b2;
          HP1600_leds(b2?HP1600_LED_YELLOW:HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON3)) {
          HP1600_clear_home();
          HP1600_print("Button 3 pressed");
          HP1600_printAt(0,1,"LED On/off mode");
          b3=!b3;
          HP1600_leds(b3?HP1600_LEDS_ON:HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON4)) {
          HP1600_clear_home();
          HP1600_print("Button 4");
          mode++;
          HP1600_printAt(0,1,"LED toggle mode");
          _pressedState = 0;
          _releasedState = 0;
        }
      break;
      case 1:
        if(HP1600_isButtonPressed(HP1600_BUTTON1)) {
          HP1600_clear_home();
          HP1600_print("Button 1 pressed");
          HP1600_printAt(0,1,"LED toggle mode");
          HP1600_leds(HP1600_LED_GREEN);
        }
        if(HP1600_isButtonPressed(HP1600_BUTTON2)) {
          HP1600_clear_home();
          HP1600_print("Button 2 pressed");
          HP1600_printAt(0,1,"LED toggle mode");
          HP1600_leds(HP1600_LED_YELLOW);
        }
        if(HP1600_isButtonPressed(HP1600_BUTTON3)) {
          HP1600_clear_home();
          HP1600_print("Button 3 pressed");
          HP1600_printAt(0,1,"LED toggle mode");
          b3=!b3;
          HP1600_leds(HP1600_LEDS_ON);
        }
        if(HP1600_wasButtonReleased(HP1600_BUTTON1)) {
          HP1600_clear_home();
          HP1600_print("Button1 released");
          HP1600_printAt(0,1,"LED toggle mode");
          HP1600_leds(HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonReleased(HP1600_BUTTON2)) {
          HP1600_clear_home();
          HP1600_print("Button2 released");
          HP1600_printAt(0,1,"LED toggle mode");
          HP1600_leds(HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonReleased(HP1600_BUTTON3)) {
          HP1600_clear_home();
          HP1600_print("Button3 released");
          HP1600_printAt(0,1,"LED toggle mode");
          HP1600_leds(HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON4)) {
          HP1600_clear_home();
          HP1600_print("Hello world!");
          mode++;
          _pressedState = 0;
          _releasedState = 0;
        }
      break;
      case 2:
        if(HP1600_wasButtonPressed(HP1600_BUTTON4)) {
          HP1600_init(MODE_BIG);
          HP1600_clear_home();
          return;
      }
      break;
    }

    if(mode==2) {
      Int2Str(buf,millis);
      HP1600_printAt(0,1,buf);
    }
    HP1600_displayCharAt(15,1,((millis % 1000) < 250) ? 1 : 0);
    DWT_Delay(50000);
  }
}

void demo_big(void) {

  uint32_t mode=0,b1=0,b2=0,b3=0;

  while(1) {
    switch(mode) {
      case 0:
        if(HP1600_wasButtonPressed(HP1600_BUTTON1)) {
          HP1600_clear_home();
          HP1600_print("Button1 pressed");
          b1=!b1;
          HP1600_leds(b1?HP1600_LED_GREEN:HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON2)) {
          HP1600_clear_home();
          HP1600_print("Button2 pressed");
          b2=!b2;
          HP1600_leds(b2?HP1600_LED_YELLOW:HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON3)) {
          HP1600_clear_home();
          HP1600_print("Button3 pressed");
          b3=!b3;
          HP1600_leds(b3?HP1600_LEDS_ON:HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON4)) {
          HP1600_clear_home();
          HP1600_print("Button 4");
          mode++;
          _pressedState = 0;
          _releasedState = 0;
        }
      break;
      case 1:
        if(HP1600_isButtonPressed(HP1600_BUTTON1)) {
          HP1600_clear_home();
          HP1600_print("Button1 pressed");
          HP1600_leds(HP1600_LED_GREEN);
        }
        if(HP1600_isButtonPressed(HP1600_BUTTON2)) {
          HP1600_clear_home();
          HP1600_print("Button2 pressed");
          HP1600_leds(HP1600_LED_YELLOW);
        }
        if(HP1600_isButtonPressed(HP1600_BUTTON3)) {
          HP1600_clear_home();
          HP1600_print("Button3 pressed");
          b3=!b3;
          HP1600_leds(HP1600_LEDS_ON);
        }
        if(HP1600_wasButtonReleased(HP1600_BUTTON1)) {
          HP1600_clear_home();
          HP1600_print("Button1released");
          HP1600_leds(HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonReleased(HP1600_BUTTON2)) {
          HP1600_clear_home();
          HP1600_print("Button2released");
          HP1600_leds(HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonReleased(HP1600_BUTTON3)) {
          HP1600_clear_home();
          HP1600_print("Button3released");
          HP1600_leds(HP1600_LEDS_OFF);
        }
        if(HP1600_wasButtonPressed(HP1600_BUTTON4)) {
          HP1600_clear_home();
          HP1600_printAt(7,0,"Hi world");
          mode++;
          _pressedState = 0;
          _releasedState = 0;
        }
      break;
      case 2:
        if(HP1600_wasButtonPressed(HP1600_BUTTON4)) {
          HP1600_init(MODE_SMALL);
          return;
      }
      break;
    }

    if(mode==2) {
      Int2Str(buf,millis);
      HP1600_printAt(0,0,buf);
    }
    HP1600_displayCharAt(15,0,((millis % 1000) < 250) ? 1 : 0);
    DWT_Delay(50000);
  }
}
