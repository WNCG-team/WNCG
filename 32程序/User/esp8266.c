#include "esp8266.h"
#include "delay.h"
/*本程序将是实现将有传感器接收到的数据上传到服务器*/
#include "bsp_usart1.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 void esp8266send(float tmp,float RH,float tmp2)
	 {
	   if(tmp>35&&RH>80)
			 {
			 //5为编号，order为数据类型：尿检或者体温
		 printf("5:order1检测到\r\n");
		 }
		
		 printf("5:order2%.3f\r\n",tmp2);

	
}
 
