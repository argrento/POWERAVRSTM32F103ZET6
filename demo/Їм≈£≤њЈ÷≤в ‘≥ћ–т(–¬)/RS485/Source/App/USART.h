#ifndef __USART_H
#define __USART_H

extern void USART1_SendByte(u16 dat);
extern void USART1Write(u8* data,u16 len);
extern void USART3_SendByte(u16 dat);
extern void USART3Write(u8* data,u16 len);

void USART1_Configuration(void);
void USART3_Configuration(void);

void Delay(vu32 nCount);

#define DIR485_H  GPIOF->BSRR=1<<11
#define DIR485_L  GPIOF->BRR=1<<11 


#endif
