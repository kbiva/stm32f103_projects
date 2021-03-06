/*
 *  adc.c
 *
 *  Author: Kestutis Bivainis
 */

#include "stm32f10x_conf.h"
#include "adc.h"

volatile uint16_t ADCConvertedValue[1];

void ADC_DMA_Configuration() {

  DMA_InitTypeDef DMA_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_ADCCLKConfig(RCC_PCLK2_Div6);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC,&GPIO_InitStructure);

  //==Configure DMA1 - Channel1==
  DMA_DeInit(DMA1_Channel1); //Set DMA registers to default values
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address; //Address of peripheral the DMA must map to
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & ADCConvertedValue; //Variable to which ADC values will be stored
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1; //Buffer size (1 because we using one channel)
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel1, &DMA_InitStructure); //Initialise the DMA
  DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1; //We using one channel

  ADC_Init(ADC1, &ADC_InitStructure); //Initialise ADC1

  //Setup order in which the Channels are sampled....
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);

  ADC_DMACmd(ADC1, ENABLE); //Enable ADC1 DMA
  ADC_Cmd(ADC1, ENABLE); //Enable ADC1

  //==Calibrate ADC1==
  ADC_ResetCalibration(ADC1);//Enable ADC1 reset calibaration register
  while (ADC_GetResetCalibrationStatus(ADC1)); //Check the end of ADC1 reset calibration register

  ADC_StartCalibration(ADC1);//Start ADC1 calibaration
  while (ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration
}

