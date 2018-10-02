#include "timer.h"

#include "202.h"


void TIM2_Int_Init(u16 arr,u16 psc)

{

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	NVIC_InitTypeDef NVIC_InitStructure;

 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	

	//定时器TIM2初始化

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	

	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

 

	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断，允许更新中断

 

	//中断优先级NVIC设置

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 

	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

 

 

	TIM_Cmd(TIM2, ENABLE);  //使能TIM2					 

}

//定时器3中断服务程序

void TIM2_IRQHandler(void)   //TIM2中断

{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生否

	{

		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIM2更新中断标志

	   HR202_TMR2_ISR();
	}

	

}
