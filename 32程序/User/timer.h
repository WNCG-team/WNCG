#ifndef __timer_H
#define __timer_H
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_it.h"
void TIM2_Int_Init(u16 arr,u16 psc);
#endif
