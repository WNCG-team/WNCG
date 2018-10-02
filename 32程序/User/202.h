#ifndef __202_H
#define __202_H
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"


void get_31HUMI_vule(void);
void  dataHM_colleck(void);
void HR202Dect_Init(void);
void HR202_TMR2_ISR(void);
extern int16_t HUMI_vule;
#endif
