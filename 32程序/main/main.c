#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "esp8266.h"
#include "bsp_SysTick.h"
#include "oled.h"
 #include "delay.h"
#include "bmp.h"
#include "202.h"
#include "common.h"
#include "timer.h"
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{ 
	char tem[10],tem1[10],tem2[10];

	float TO=0,TA=0;
  u8 sum=0,i=0;
	NVIC_Configuration();
	HR202Dect_Init();
	TIM2_Int_Init(99,71);
	//SysTick_Init();
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级 	
	OLED_Init();			//初始化OLED  
	OLED_Clear(); 
	USARTx_Config ();                                                              //初始化串口1
           

	while(1)
	{ 
		delay_ms(2);
		dataHM_colleck();
		delay_ms(2);
		get_31HUMI_vule();
			tem_com();
	float 	HUMI= (float)HUMI_vule/10;
		
		if(Receive_ok)//串口接收完毕
		{ 	
			for(sum=0,i=0;i<(TEMP_data[3]+4);i++)//TEMP_data[3]=4
			sum+=TEMP_data[i];
			if(sum==TEMP_data[i])//校验和判断
			{
				TO =(float)((TEMP_data[4]<<8)|TEMP_data[5])/100;//得到真实温度
				//data[0]=(TEMP_data[4]<<8)|TEMP_data[5];//放大100倍的物体温度
				TA =(float)((TEMP_data[6]<<8)|TEMP_data[7])/100;//得到真实温度
				//data[1]=(TEMP_data[6]<<8)|TEMP_data[7];//放大100倍的环境温度
				//printf("真实温度:%f \r\n" ,TO);
				//printf("环境温度:%f \r\n",TA);
				
				sprintf(tem2,"RH:%.1f",HUMI);
				
				sprintf(tem,":%.3f",TO);
				sprintf(tem1,":%.3f",TA);
				OLED_ShowCHinese(0,0,0);
				OLED_ShowCHinese(16,0,1);
				OLED_ShowCHinese(32,0,2);
				OLED_ShowCHinese(48,0,3);
				OLED_ShowString(64,0 ,tem);
				OLED_ShowCHinese(0,2,4);
				OLED_ShowCHinese(16,2,5);
				OLED_ShowCHinese(32,2,6);
				OLED_ShowCHinese(48,2,7);
				OLED_ShowString(64,2 ,tem1);
				OLED_ShowString(0,4 ,tem2);
			}
			Receive_ok=0;//处理数据完毕标志
		}
		delay_ms(250);
		esp8266send(TO,HUMI,TO);     
	}

}



/*********************************************END OF FILE**********************/
