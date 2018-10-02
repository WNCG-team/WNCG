 #ifndef __ESP8266_H
 #define	__ESP8266_H
 #include "common.h"
#include "bsp_usart1.h"
 #define esp8266_send USART_printf


 void esp8266send(float tmp,float RH,float tmp2);
 
 
#endif 
