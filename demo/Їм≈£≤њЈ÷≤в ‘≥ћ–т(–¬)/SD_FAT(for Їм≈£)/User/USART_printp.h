#ifndef _USART_PRINTP_H_
#define _USART_PRINTP_H_

#ifndef  TRUE
#define  TRUE 1
#endif
#ifndef  FALSE
#define  FALSE 0
#endif

#define PRINTP_EN  	TRUE		//���ӵ�printp()�����Ƿ�ʹ��
#define CR_as_CRLF	FALSE          // if true , you can use "\n" to act as CR/LF, 
                                    // if false , you have to use "\n\r",but can get a higher speed

extern char usart_putchar(char ch);     //����1��1���ֽ�
extern void prints(char* ctrl);         //���ڷ�һ���ַ���

#if PRINTP_EN == TRUE
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
void printp( char* ctrl, ...);
#endif


#endif  //_USART_PRINTP_H_

