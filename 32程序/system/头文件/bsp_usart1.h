#ifndef __USART1_H
#define	__USART1_H
#include "stdio.h"
#include "stm32f10x.h"




/**************************USART参数定义********************************/
#define             macUSART_BAUD_RATE                       115200

#define             macUSARTx                                USART1
#define             macUSART_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define             macUSART_CLK                             RCC_APB2Periph_USART1
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        RCC_APB2Periph_GPIOA     
#define             macUSART_TX_PORT                         GPIOA   
#define             macUSART_TX_PIN                          GPIO_Pin_9
#define             macUSART_RX_PORT                         GPIOA 
#define             macUSART_RX_PIN                          GPIO_Pin_10
#define             macUSART_IRQ                             USART1_IRQn
#define             macUSART_INT_FUN                         USART1_IRQHandler



void                USARTx_Config                           ( void );
extern u8 TEMP_data[20],Receive_ok;
typedef struct __FILE FILE;
void tem_com(void);
void send_out(int16_t *data,uint8_t length,uint8_t send);

#endif /* __USART1_H */
