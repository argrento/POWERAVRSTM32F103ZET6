/*******************************************************************************
* File Name          : main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 08/08/2008
* Description        : Main program body
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include <stdio.h>
//#include "main.h"

/************ 用于定义ITM Viewer相关的ITM激励寄存器端口 ************************/
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

/*用于定义是否使用ITM Viewer*/
//#define DBG_ITM   

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;


/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);
void PrintBackupReg(void); 
void WriteToBackupReg(u16 FirstBackupData);
u8 CheckBackupReg(u16 FirstBackupData);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
  debug();
#endif

  /* System Clocks Configuration */
  RCC_Configuration();
  
  /* NVIC configuration */
  NVIC_Configuration();
    
  /* GPIO configuration */
  GPIO_Configuration();

  /* Configure the USART1 */
  USART_Configuration();

  printf("\n\r\n");
  PrintBackupReg();

  /* Enable PWR and BKP clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Enable write access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Clear Tamper pin Event(TE) pending flag */
  BKP_ClearFlag();

  /* Tamper pin active on low level */
  BKP_TamperPinLevelConfig(BKP_TamperPinLevel_Low);

  /* Enable Tamper interrupt */
  BKP_ITConfig(ENABLE);

  /* Enable Tamper pin */
  BKP_TamperPinCmd(ENABLE);
   
  /* Write data to Backup DRx registers */
//  WriteToBackupReg(0xA53C);

  /* Check if the written data are correct */


  if(CheckBackupReg(0xA53C) == 0x00)
  {
    /* Turn on led connected to PC.08 */
    GPIO_Write(GPIOC, GPIO_Pin_6);
	printf("Led_1: on.--------------------The datas are as their initial status\r\n");
  }
  else
  {
    /* Turn on led connected to PC.09 */
    GPIO_Write(GPIOC, GPIO_Pin_7);
	printf("Led_2: on.--------------------The datas have been changed.\r\n");

    /* Clear Tamper pin Event(TE) pending flag */
    BKP_ClearFlag();


    /* Write data to Backup DRx registers */
	WriteToBackupReg(0xA53C);
	printf("Recover the datas of DRx to their initial status.");
	PrintBackupReg();

  }
        
  while(1)
  {    
  }
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

  /* Enable GPIOC clock,USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO| RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);


}

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


  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  /* Configure USART1 Rx (PA.10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* Configure PC.06, PC.07, PC.08 and PC.09 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Enable TAMPER IRQChannel */
  NVIC_InitStructure.NVIC_IRQChannel = TAMPER_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


 /*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef  USART_ClockInitStructure;
/* USART1 configuration ------------------------------------------------------*/
  /* USART1 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
/* Configure the USART1 synchronous paramters */
USART_ClockInit(USART1, &USART_ClockInitStructure);

USART_InitStructure.USART_BaudRate = 115200;
USART_InitStructure.USART_WordLength = USART_WordLength_8b;
USART_InitStructure.USART_StopBits = USART_StopBits_1;
USART_InitStructure.USART_Parity = USART_Parity_No ;
USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;


USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
/* Configure USART1 basic and asynchronous paramters */
USART_Init(USART1, &USART_InitStructure);
    
  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);
}


 /*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART or ITM Viewer.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

int fputc(int ch, FILE *f)
{
#ifdef DBG_ITM
/* 将Printf内容发往ITM激励寄存器端口  */
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
#else  
/* 将Printf内容发往串口 */
  USART_SendData(USART1, (unsigned char) ch);
  while (!(USART1->SR & USART_FLAG_TXE));
#endif  
  return (ch);
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*******************************************************************************
* Function Name  : WriteToBackupReg
* Description    : Writes data Backup DRx registers.
* Input          : FirstBackupData: data to be written to Backup data registers.
* Output         : None
* Return         : None
*******************************************************************************/
void WriteToBackupReg(u16 FirstBackupData)
{
  BKP_WriteBackupRegister(BKP_DR1, FirstBackupData);
  BKP_WriteBackupRegister(BKP_DR2, BKP->DR1 + 0x5A);
  BKP_WriteBackupRegister(BKP_DR3, BKP->DR2 + 0x3C);
  BKP_WriteBackupRegister(BKP_DR4, BKP->DR3 + 0xA5);
  BKP_WriteBackupRegister(BKP_DR5, BKP->DR4 + 0x06);
  BKP_WriteBackupRegister(BKP_DR6, BKP->DR5 + 0x78);
  BKP_WriteBackupRegister(BKP_DR7, BKP->DR6 + 0xFF);
  BKP_WriteBackupRegister(BKP_DR8, BKP->DR7 + 0xB4);
  BKP_WriteBackupRegister(BKP_DR9, BKP->DR8 + 0x1E);
  BKP_WriteBackupRegister(BKP_DR10, BKP->DR9 + 0xD4);
}

/*******************************************************************************
* Function Name  : CheckBackupReg
* Description    : Checks if the Backup DRx registers values are correct or not.
* Input          : FirstBackupData: data to be compared with Backup data registers.
* Output         : None
* Return         : - 0: All Backup DRx registers values are correct
*                  - Value different from 0: Number of the first Backup register
*                    which value is not correct
*******************************************************************************/
u8 CheckBackupReg(u16 FirstBackupData)
{
  if(BKP_ReadBackupRegister(BKP_DR1) != FirstBackupData) return 1;
  if(BKP_ReadBackupRegister(BKP_DR2) != (BKP->DR1 + 0x5A)) return 2;
  if(BKP_ReadBackupRegister(BKP_DR3) != (BKP->DR2 + 0x3C)) return 3;
  if(BKP_ReadBackupRegister(BKP_DR4) != (BKP->DR3 + 0xA5)) return 4;
  if(BKP_ReadBackupRegister(BKP_DR5) != (BKP->DR4 + 0x06)) return 5;
  if(BKP_ReadBackupRegister(BKP_DR6) != (BKP->DR5 + 0x78)) return 6;
  if(BKP_ReadBackupRegister(BKP_DR7) != (BKP->DR6 + 0xFF)) return 7;
  if(BKP_ReadBackupRegister(BKP_DR8) != (BKP->DR7 + 0xB4)) return 8;
  if(BKP_ReadBackupRegister(BKP_DR9) != (BKP->DR8 + 0x1E)) return 9;
  if(BKP_ReadBackupRegister(BKP_DR10) != (BKP->DR9 + 0xD4)) return 10;

  return 0;
}

/*******************************************************************************
* Function Name  : IsBackupRegReset
* Description    : Checks if the Backup DRx registers are reset or not.
* Input          : None
* Output         : None
* Return         : - 0: All Backup DRx registers are reset
*                  - Value different from 0: Number of the first Backup register
*                    not reset
*******************************************************************************/
u8 IsBackupRegReset(void)
{
  if(BKP_ReadBackupRegister(BKP_DR1) != 0x0000) return 1;
  if(BKP_ReadBackupRegister(BKP_DR2) != 0x0000) return 2;
  if(BKP_ReadBackupRegister(BKP_DR3) != 0x0000) return 3;
  if(BKP_ReadBackupRegister(BKP_DR4) != 0x0000) return 4;
  if(BKP_ReadBackupRegister(BKP_DR5) != 0x0000) return 5;
  if(BKP_ReadBackupRegister(BKP_DR6) != 0x0000) return 6;
  if(BKP_ReadBackupRegister(BKP_DR7) != 0x0000) return 7;
  if(BKP_ReadBackupRegister(BKP_DR8) != 0x0000) return 8;
  if(BKP_ReadBackupRegister(BKP_DR9) != 0x0000) return 9;
  if(BKP_ReadBackupRegister(BKP_DR10) != 0x0000) return 10;

  return 0;
}

/*******************************************************************************
* Function Name  : PrintBackupReg
* Description    : Print the Backup DRx registers values.
* Input          : 
* Output         : None
* Return         : 
*                  
*                    
*******************************************************************************/
void PrintBackupReg(void)
{
  //u16 e;
  //e = BKP_DR1;
  //printf("e  = 0x%x\t",e);
  printf("\nNow the data in DRx are:\n");
  printf("DR1  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR1)); 
  printf("DR2  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR2));
  printf("DR3  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR3));
  printf("DR4  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR4));
  printf("DR5  = 0x%04X\t\n",BKP_ReadBackupRegister(BKP_DR5));
  printf("DR6  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR6));
  printf("DR7  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR7));
  printf("DR8  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR8));
  printf("DR9  = 0x%04X\t",BKP_ReadBackupRegister(BKP_DR9));
  printf("DR10 = 0x%04X\t\n",BKP_ReadBackupRegister(BKP_DR10));
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
