/******************************************************************************
* ����STM32��ģ���ʼ������
*******************************************************************************/
/* Private define ------------------------------------------------------------*/
//#define PRINTF_EN
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"

#ifdef PRINTF_EN
	#include <stdio.h>
#endif
/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);

void Init_Device(void);
/* Private functions ---------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
// ������ϵͳRCCģ��ĳ�ʼ�����ⲿ�ֱȽϸ���
// ĳЩ����ģ�飨��USART��RTC�ȣ������ʱ��ʹ�ܣ������ڸ��Ե�ģ���ʼ��������
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : ϵͳʱ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;

	//ʹ���ⲿ����
	RCC_HSEConfig(RCC_HSE_ON);
	//�ȴ��ⲿ�����ȶ�
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	//����ⲿ���������ɹ����������һ������
	if (HSEStartUpStatus==SUCCESS)
	{
		//����HCLK��AHBʱ�ӣ�=SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//PCLK1(APB1) = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//PCLK2(APB2) = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//FLASHʱ�����
		//�Ƽ�ֵ��SYSCLK = 0~24MHz   Latency=0
		//        SYSCLK = 24~48MHz  Latency=1
		//        SYSCLK = 48~72MHz  Latency=2
		FLASH_SetLatency(FLASH_Latency_2);
		//����FLASHԤȡָ����
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL���� SYSCLK/1 * 9 = 8*1*9 = 72MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		//����PLL
		RCC_PLLCmd(ENABLE);
		//�ȴ�PLL�ȶ�
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		//ϵͳʱ��SYSCLK����PLL���
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//�л�ʱ�Ӻ�ȴ�ϵͳʱ���ȶ�
		while (RCC_GetSYSCLKSource()!=0x08);
	}

	//�����Ǹ���ģ�鿪��ʱ��
	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC 
         | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG 
         | RCC_APB2Periph_AFIO, ENABLE);
	return;
}

////////////////////////////////////////////////////////////////////////////////
// �������ж�������ĳ�ʼ��
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
// GPIO����
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//PC��4567������GPIO��������� 2M
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//KEY2 KEY3 JOYKEY
	//λ��PD�ڵ�3 4 11-15�ţ�ʹ������Ϊ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_11 | GPIO_Pin_12 |\
								  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	return;
}


////////////////////////////////////////////////////////////////////////////////
//������USARTģ��ĳ�ʼ��
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : USART����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	//����USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//USART1_TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1ģ������
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);

#if USART_RXINT_EN
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
#endif

	USART_Cmd(USART1, ENABLE);
	return;
}




////////////////////////////////////////////////////////////////////////////////
// ������ϵͳ��ģ��ĳ�ʼ�����ܺ�������������������ĺ��������ϵͳ�ĳ�ʼ��
// main.c��ֻ����ñ���������
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : Init_Device
* Description    : Init all the system moudles
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Device(void)
{
	RCC_Configuration();
	
	GPIO_Configuration();
	USART_Configuration();

    NVIC_Configuration();
}
