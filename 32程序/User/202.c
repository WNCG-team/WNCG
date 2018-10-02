#include "202.h"
#include "bsp_usart1.h"

#define OS_FALSE 0
#define OS_TRUE 1
//全局变量定义
int16_t HUMI_vule,temp_vule=28;  //湿温值
int32_t R_Humideal_int;  //湿温电阻阻值
int8_t PwmTimeCnt = 0;

typedef struct
{
	u16  AITbl[10];
	int16_t AIEnable;
	int16_t AICnt;
		}HR202_ADC;



 HR202_ADC HR202_ADCTbl;

//求u16型数组[num]平均值，去掉最大最小值
u16 AdcConvertAverage(u16 *p,u8 num) 
{ 
    u8 i; 
    u16 ret=0; 
    u16 max_id,min_id,max_value,min_value;   
    for(i=0;i<num;i++) 
    {
      ret+=(*(p+i)); 
    }
   //找到最大和最小值索引  
   ret/=num; 
   max_id=min_id=1; 
   max_value=min_value=0; 
   for(i=0;i<num;i++) 
   { 
      if((*(p+i)) > ret) 
      { 
          if((*(p+i)) - ret > max_value) 
         { 
              max_value=(*(p+i)) - ret; 
              max_id=i; 
         } 
      } 
      else 
     { 
          if(ret - (*(p+i)) > min_value) 
         { 
             min_value=ret - (*(p+i)); 
             min_id=i; 
         } 
     } 
  } 
  
 //去掉第一个和最大最小值后的平均值 
   ret=0; 
   for(i=0;i<num;i++) 
  { 
     if((i!=min_id)&&(i!=max_id)) 
     ret+=(*(p+i));      
  }   
  if(min_id!=max_id) 
     ret = (ret+(num/2))/(num-2); 
  else   
     ret = (ret+(num/2))/(num-1);  
	
  return ret; 
}

/***************************************************************************
*** 函数名称 : HR202Dect_Init                                      
*** 创建人   : xx                                                
*** 创建日期 : xxxx                                           
*** 修改人者 :                                                           
*** 修改日期 :                                                           
*** 输入     : 
*** 返回     : 
*** 函数功能 : AI 初始化
***************************************************************************/
void HR202Dect_Init(void)
{
  
  
  //ADC1 初始化
 ADC_InitTypeDef ADC_InitStructure;
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);        
 GPIO_InitTypeDef  GPIO_InitStructure;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8;			//9为湿敏电阻	 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
 GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 			
	ADC_DeInit(ADC1);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode =DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

    //RH
  

  ADC_Cmd(ADC1, ENABLE);
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1) == SET);
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1) == SET);
  
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);  
	
	
	
	//Param 参数初始化
for(int8_t i=0;i<10;i++)
  {
  HR202_ADCTbl.AITbl[i] = 0;//采集AD数据 10个
  }
  HR202_ADCTbl.AIEnable = OS_FALSE;//采集是否完成
  HR202_ADCTbl.AICnt = 0;//采集数量
 
}
///ADC1 GetValue
int16_t HR202Dect_GetADValue(void)
{
  int16_t GetValue = 0;
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE); 
	 while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
  GetValue = ADC_GetConversionValue(ADC1);
  return GetValue;
}
/*****************************************
//湿敏电阻采集数据 返回湿敏阻值R_Humideal_int
*****************************************/
void  dataHM_colleck(void)
{
  int16_t ADValueTemp = 0;
  if(HR202_ADCTbl.AIEnable == OS_FALSE)
  {
    ADValueTemp = AdcConvertAverage(HR202_ADCTbl.AITbl,10);
    HR202_ADCTbl.AIEnable = OS_TRUE;
  }
  //获得实际湿敏电阻阻抗值（单位Ω）
	float temp=(float)ADValueTemp*3.3/4096;
//  R_Humideal_int = (51000*4096)/ADValueTemp-51000;//Rf = 51K (Rh+Rf)/Rmf=Vcc/Vh; 
	R_Humideal_int = 51000*temp/(3.3-temp);
//printf("%d",R_Humideal_int);
}

/*****************************************
//31K湿敏电阻获取湿度值
*****************************************/
void get_31HUMI_vule(void)
{
  //分段计算湿度值 
  if((R_Humideal_int/1000)>=2600)    HUMI_vule=(146000-10*(R_Humideal_int/1000))*10/4800; //20-25
  else if(1300<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<2600)  HUMI_vule=(91000-10*(R_Humideal_int/1000))*10/2600; //25-30
  else if(630<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<1300)  HUMI_vule=(53200-10*(R_Humideal_int/1000))*10/1340; //30-35
  else if(310<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<630)   HUMI_vule=(28700-10*(R_Humideal_int/1000))*10/640; //35-40  
  else if(160<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<310)  HUMI_vule=(15100-10*(R_Humideal_int/1000))*10/300; //40-45 
  else if(87<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<160)  HUMI_vule=(8170-10*(R_Humideal_int/1000))*10/146; //45-50
  else if(49<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<87)  HUMI_vule=(4670-10*(R_Humideal_int/1000))*10/76; //50-55
  else if(31<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<49)  HUMI_vule=(2470-10*(R_Humideal_int/1000))*10/36; //55-60     
  else if(20<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<31)  HUMI_vule=(1630-10*(R_Humideal_int/1000))*10/22; //60-65  
  else if(13<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<20)  HUMI_vule=(1110-10*(R_Humideal_int/1000))*10/14; //65-70
  else if(8<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<13)   HUMI_vule=(7740-100*(R_Humideal_int/1000))*10/92;//70-75
  else if(6<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<8)   HUMI_vule=(4890-100*(R_Humideal_int/1000))*10/54;//75-80
  else if(5<=(R_Humideal_int/1000)&(R_Humideal_int/1000)<6)  HUMI_vule=(2010-100*(R_Humideal_int/1000))*10/18;//80-85
  else if((R_Humideal_int/1000)<6)  HUMI_vule=(388-10*(R_Humideal_int/1000))*10/4;//85-90

  //温度补偿0-60℃
	if(HUMI_vule<=0)HUMI_vule=0;
  if(temp_vule<=25)  HUMI_vule= HUMI_vule+((25-temp_vule)*0.2);   //小于25一度加0.2
  if((25<temp_vule)&(temp_vule<=40))  HUMI_vule= HUMI_vule-((temp_vule-25)*0.2);   //大于25一度减0.2
  if(HUMI_vule>=999)  HUMI_vule=999;  //大于99  




}
static int8_t HR202_Step = 0; 
static int8_t HR202_Flag = 0;

void HR202_TMR2_ISR(void)//100us定时器中断调用函数
{
 if(HR202_Step == 0x00)
 {
   if(HR202_Flag == 0x00)
   {
  
     HR202_Flag = 0x10;//
   }
   else if(HR202_Flag == 0x10)
   {
     //PWM输出
     if(PwmTimeCnt > 0)
     {
      PwmTimeCnt--;
     }
     else
     {
      PwmTimeCnt = 9;
     }     
     if(PwmTimeCnt>=5)//1Khz 方波
     {
      GPIO_SetBits(GPIOC,GPIO_Pin_9);//Rh 1
      GPIO_ResetBits(GPIOC,GPIO_Pin_8);//Rf 0
			
     }
     else 
     {
      GPIO_ResetBits(GPIOC,GPIO_Pin_9);//Rh 0
      GPIO_SetBits(GPIOC,GPIO_Pin_8);;//Rf 1
     }
     if(PwmTimeCnt == 2)//在Rh 为高电平时的200us 进行AD采集
     {
       if(HR202_ADCTbl.AIEnable == OS_TRUE)//是否可以采集
       {
          HR202_ADCTbl.AITbl[HR202_ADCTbl.AICnt++] = HR202Dect_GetADValue();//采集当前AD值
          if(HR202_ADCTbl.AICnt == 10)//采集10次 取平均
          { 
            HR202_ADCTbl.AIEnable = OS_FALSE;//完成一组数据采集
            HR202_ADCTbl.AICnt = 0;
          }
       }
     }
   }
 } 
}
