/*
 *  drx7375.h
 *
 *  Clarion DRX7375R face plate demo
 *
 *  Author: Kestutis Bivainis
 */

#ifndef DRX7375R_H
#define DRX7375R_H

#include "stm32f10x_conf.h"

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

#define DIM_Pin      GPIO_Pin_2             // DIM = PC[2]
#define DIM_Speed    GPIO_Speed_50MHz
#define DIM_Mode     GPIO_Mode_AIN
#define DIM_Port     GPIOC
#define DIM_Bus      RCC_APB2Periph_GPIOC

typedef struct _PIN {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Bus;
} PIN;

typedef struct _DATA_LC75854 {
  uint8_t ccb_address;
  uint8_t data[7];
} DATA_LC75854;

typedef struct _DATA_LC75824 {
  uint8_t ccb_address;
  uint8_t data[8];
} DATA_LC75824;

#define CMD_INPUT_LC75854 0x42
#define CMD_OUTPUT_LC75854 0xC2
#define CMD_INPUT_LC75824 0x82

#define SLEEP_CONTROL_DATA_LC75854 0x03
#define SLEEP_CONTROL_DATA_LC75824 0x04

#define SEGMENTS_ONOFF_CONTROL_DATA_LC75854 0x08
#define SEGMENTS_ONOFF_CONTROL_DATA_LC75824 0x08

#define BIAS_LC75854 0x04
#define BIAS_LC75824 0x10

#define KEY_SCAN_LC75854 0x40

#define DIGIT_CLEAR1 0x11
#define DIGIT_CLEAR1_HIGH (DIGIT_CLEAR1|0x0F)
#define DIGIT_CLEAR1_LOW  (DIGIT_CLEAR1|0xF0)
#define DIGIT_CLEAR2 0x01
#define DIGIT_CLEAR2_HIGH (DIGIT_CLEAR2|0x0F)
#define DIGIT_CLEAR2_LOW  (DIGIT_CLEAR2|0xF0)

#define TRUE 1
#define FALSE 0

#define MAX_POS 9

// 4 bit packet nr,4 bit byte nr in packet ,8 bit mask value in byte
#define SYMBOL_LC75854_RING1    0x0080
#define SYMBOL_LC75854_RING2    0x0040
#define SYMBOL_LC75854_RING3    0x0020
#define SYMBOL_LC75854_RING4    0x0010
#define SYMBOL_LC75854_RING5    0x0008
#define SYMBOL_LC75854_RING6    0x0004
#define SYMBOL_LC75854_RING7    0x0002
#define SYMBOL_LC75854_RING8    0x0001
#define SYMBOL_LC75854_RING9    0x0180
#define SYMBOL_LC75854_RING10   0x0140
#define SYMBOL_LC75854_RING11   0x0120
#define SYMBOL_LC75854_RING12   0x0110
#define SYMBOL_LC75854_RING13   0x0108
#define SYMBOL_LC75854_RING14   0x0104
#define SYMBOL_LC75854_RING15   0x0102
#define SYMBOL_LC75854_RING16   0x0101
#define SYMBOL_LC75854_B        0x0208
#define SYMBOL_LC75854_EQ       0x0308
#define SYMBOL_LC75854_U        0x0408
#define SYMBOL_LC75854_ISR      0x1180
#define SYMBOL_LC75854_AF       0x1280
#define SYMBOL_LC75854_REG      0x1380
#define SYMBOL_LC75854_TA       0x1480
#define SYMBOL_LC75854_TP       0x2080
#define SYMBOL_LC75854_PTY      0x2180
#define SYMBOL_LC75854_BRACKETS 0x2280
#define SYMBOL_LC75854_6        0x2380
#define SYMBOL_LC75854_5        0x2480
#define SYMBOL_LC75854_4        0x3080
#define SYMBOL_LC75854_3        0x3180
#define SYMBOL_LC75854_2        0x3280
#define SYMBOL_LC75854_1        0x3380

#define SYMBOL_LC75824_LETTER4UP            0x0080
#define SYMBOL_LC75824_LETTER3DOWNBACKSLASH 0x0040
#define SYMBOL_LC75824_LETTER3DOWNRIGHTBAR  0x0020
#define SYMBOL_LC75824_LETTER4UPRIGHTBAR    0x0010
#define SYMBOL_LC75824_LETTER4MIDDLELINE    0x0008
#define SYMBOL_LC75824_LETTER3MIDDLEBAR     0x0004
#define SYMBOL_LC75824_LETTER3MIDDLELINE    0x0002
#define SYMBOL_LC75824_LETTER4UPLEFTBAR     0x0001
#define SYMBOL_LC75824_LETTER4DOWNLINE      0x0180
#define SYMBOL_LC75824_LETTER4DOWNLEFTBAR   0x0140
#define SYMBOL_LC75824_LETTER3UPRIGHTBAR    0x0120
#define SYMBOL_LC75824_LETTER4DOWNRIGHTBAR  0x0110
#define SYMBOL_LC75824_ADJ                  0x0108
#define SYMBOL_LC75824_LESS_MORE            0x0104
#define SYMBOL_LC75824_A_M                  0x0102
#define SYMBOL_LC75824_TITLE                0x0101
#define SYMBOL_LC75824_SRCH                 0x0280
#define SYMBOL_LC75824_LESS_MORE_FILLED     0x0240
#define SYMBOL_LC75824_VOL                  0x0220
#define SYMBOL_LC75824_UP_DOWN_FILLED       0x0210
#define SYMBOL_LC75824_LETTER1L             0x0208
#define SYMBOL_LC75824_LETTER1MIDDLEBAR     0x0204
#define SYMBOL_LC75824_LETTER1UPLINE        0x0202
#define SYMBOL_LC75824_DSP                  0x0201
#define SYMBOL_LC75824_DISC                 0x0310
#define SYMBOL_LC75824_S                    0x0301
#define SYMBOL_LC75824_DSP_2                0x0401
#define SYMBOL_LC75824_DSP_U                0x0510
#define SYMBOL_LC75824_TAPE                 0x0501
#define SYMBOL_LC75824_DSP_1                0x1010
#define SYMBOL_LC75824_DSP_B                0x1001
#define SYMBOL_LC75824_ST                   0x1110
#define SYMBOL_LC75824_BLS                  0x1210
#define SYMBOL_LC75824_LETTER2LEFTBAR       0x1208
#define SYMBOL_LC75824_LETTER3UPSLASH       0x1204
#define SYMBOL_LC75824_LETTER3UPLINE        0x1202
#define SYMBOL_LC75824_LETTER2MIDDLELINE    0x1201
#define SYMBOL_LC75824_LETTER2SLASHES       0x1380
#define SYMBOL_LC75824_LETTER3LEFTBAR       0x1340
#define SYMBOL_LC75824_LETTER3UPBASKSLASH   0x1320
#define SYMBOL_LC75824_LETTER2MIDDLEBAR     0x1310
#define SYMBOL_LC75824_LETTER2DOWNLINE      0x1301
#define SYMBOL_LC75824_LETTER2UPLINE        0x1480
#define SYMBOL_LC75824_LETTER3DOWNSLASH     0x1440
#define SYMBOL_LC75824_LETTER3DOWNLINE      0x1420
#define SYMBOL_LC75824_LETTER2RIGHTBAR      0x1410
#define SYMBOL_LC75824_B                    0x1401
#define SYMBOL_LC75824_C                    0x1510
#define SYMBOL_LC75824_NR                   0x1610
#define SYMBOL_LC75824_PERIODMIDDLEUP       0x2010
#define SYMBOL_LC75824_APERIOD              0x2001
#define SYMBOL_LC75824_LD                   0x2101
#define SYMBOL_LC75824_PERIODMIDDLEDOWN     0x2210
#define SYMBOL_LC75824_BRACKETS_DISKS       0x2201
#define SYMBOL_LC75824_DISKS                0x2310
#define SYMBOL_LC75824_TV                   0x2410
#define SYMBOL_LC75824_DIGITAL              0x2401
#define SYMBOL_LC75824_PERIODRIGHTUP        0x3010
#define SYMBOL_LC75824_DISC_FRAMED          0x3001
#define SYMBOL_LC75824_PERIODRIGHTDOWN      0x3101
#define SYMBOL_LC75824_SCN_FRAME            0x3210
#define SYMBOL_LC75824_SCN                  0x3201
#define SYMBOL_LC75824_RPT_FRAME            0x3301
#define SYMBOL_LC75824_RDM_FRAME            0x3410
#define SYMBOL_LC75824_RDM                  0x3401
#define SYMBOL_LC75824_RPT                  0x3501

// byte 0
#define KEY_DSP       0x01
#define KEY_EQ        0x02

// byte 1
#define KEY_RIGHT     0x01
#define KEY_LEFT      0x02
#define KEY_DOWN      0x04
#define KEY_UP        0x08
#define KEY_ENTER     0x10
#define KEY_ADJ       0x80

// byte 2
#define KEY_4         0x01
#define KEY_RPT       0x02
#define KEY_RDM       0x04
#define KEY_DISP      0x08
#define KEY_TA        0x10
#define KEY_BAND      0x20
#define KEY_AM        0x40
#define KEY_TITLE     0x80

// byte 3
#define KEY_3         0x02
#define KEY_2         0x04
#define KEY_1         0x08
#define KEY_SCN       0x10
#define KEY_6         0x40
#define KEY_5         0x80

#define RING_1        10
#define RING_2        9
#define RING_3        13
#define RING_4        14
#define RING_5        15
#define RING_6        16
#define RING_7        12
#define RING_8        11
#define RING_9        7
#define RING_10       8
#define RING_11       4
#define RING_12       3
#define RING_13       2
#define RING_14       1
#define RING_15       5
#define RING_16       6

void GPIO_Configuration(void);
void DRX7375R_init(void);

void DRX7375R_send(void);
void DRX7375R_send_LC75854(void);
void DRX7375R_send_LC75824(void);

void DRX7375R_receive_LC75854(void);

void DRX7375R_fill(uint32_t flush);
void DRX7375R_fill_LC75854(uint32_t flush);
void DRX7375R_fill_LC75824(uint32_t flush);

void DRX7375R_clear(uint32_t flush);
void DRX7375R_clear_LC75854(uint32_t flush);
void DRX7375R_clear_LC75824(uint32_t flush);

void DRX7375R_clear_symbols(uint32_t flush);
void DRX7375R_clear_symbols_LC75854(uint32_t flush);
void DRX7375R_clear_symbols_LC75824(uint32_t flush);

void DRX7375R_fill_symbols(uint32_t flush);
void DRX7375R_fill_symbols_LC75854(uint32_t flush);
void DRX7375R_fill_symbols_LC75824(uint32_t flush);

void DRX7375R_fill_symbol_val_LC75854(uint32_t val,uint32_t on,uint32_t flush);
void DRX7375R_fill_symbol_val_LC75824(uint32_t val,uint32_t on,uint32_t flush);

void DRX7375R_fill_symbol_nr_LC75854(uint32_t nr,uint32_t on,uint32_t flush);
void DRX7375R_fill_symbol_nr_LC75824(uint32_t nr,uint32_t on,uint32_t flush);

void DRX7375R_fill_dots(uint32_t column,uint8_t mask,uint32_t flush);
void DRX7375R_clear_dots(uint32_t column,uint8_t mask,uint32_t flush);
void DRX7375R_fill_all_dots(uint8_t mask,uint32_t flush);
void DRX7375R_clear_all_dots(uint8_t mask,uint32_t flush);

void DRX7375R_char(char chr,int pos);
void DRX7375R_clear_char(int pos);
void DRX7375R_text(char *pString,int pos,int clear,uint32_t flush);
void DRX7375R_clear_text(uint32_t flush);

void DRX7375R_ring(uint16_t on,uint32_t flush);

void DRX7375R_segments(uint32_t on);
void DRX7375R_sleep(uint32_t on);

#endif
