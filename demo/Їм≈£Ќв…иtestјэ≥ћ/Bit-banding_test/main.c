/*******************************************************************************
* File Name          : main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 08/08/2008
* Description        : Main program body
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
u32 uart_input_u32(void) ;
void bitbanding_test(u32,u32,u32);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{  u8  select;
   u32 region_address;
   u32 alia_address;
   u32 region_bit;
  int arr[]={6,7,8,9,10};   
  int *ptr=arr; 
  *(ptr++)+=123;   
#ifdef DEBUG
  debug();
#endif

  /* Configure the system clocks */
  RCC_Configuration();
    
  /* NVIC Configuration */
  NVIC_Configuration();

  /* Configure the GPIOs */
  GPIO_Configuration();
  
  /* Configure the USART1 */
  USART_Configuration();
  

  printf("%d,%d",*ptr,*(++ptr));

  printf("\n");
  printf("\n boot success! \n");
  printf("\n /** 0 maens test bit-banding of SRAM         **/\n");
  printf("\n /** 1 maens test bit-banding of Peripherals  **/\n");
  printf("\n /** 2 maens to exit!                         **/\n");
  while(1)
  {
	printf("\n Please input your choice 0/1/2:");
    while( USART_GetFlagStatus(USART1,USART_IT_RXNE)==RESET) ;
    select=USART_ReceiveData(USART1);
	printf("%c\n",select);
    switch(select)
    {
      case '0':
	  {
	    printf("\n Please input the address of bit-banding region(SRAM)(20000000--200FFFFF):0x");
	    region_address=uart_input_u32(); 
	 	//Get the region address of the test area from the hypertrm.
		while( region_address<0x20000000|| region_address>0x200FFFFF)
		 {
		   printf("\n Input ERROR!Please input again!\n") ;
		   printf("\n Please input the address of bit-banding region(SRAM)(20000000--200FFFFF):0x");
		   region_address=uart_input_u32();
		 }
	    printf("\n Please input the bit of bit-banding region(SRAM)(0--7):0x");
	    region_bit=uart_input_u32(); 
		 //Get the region bit of the test area from the hypertrm.
		while( region_bit>7)
		  {
		    printf("\n Input ERROR!Please input again!\n") ;
			printf("\n Please input the bit of bit-banding region(SRAM)(0--7):0x");
		    region_bit=uart_input_u32();
		  }
	    alia_address=0x22000000+(region_address-0x20000000)*32+region_bit*4;
		printf(" alia_address = 0x%X\n",alia_address);
	    bitbanding_test(region_address,region_bit,alia_address);
		//test the bit-banding. 
	    break;
	  }
      case '1':
      {
	    printf("You must give clock to your test peripheral area firstly!\n");
	 /* printf("\n Please input the address of bit-banding region(Peripherals)(40000000--400FFFFF):0x");
	    region_address=uart_input_u32();
		while( region_address<0x40000000|| region_address>0x400FFFFF)
		 {
		   printf("\n Input ERROR!Please input again!\n") ;
		   printf("\n Please input the address of bit-banding region(Peripherals)(40000000--400FFFFF):0x");
		   region_address=uart_input_u32();
		 }
	    printf("\n Please input the bit of bit-banding region(Peripherals)(0--7):0x");
	    region_bit=uart_input_u32();
		while( region_bit>7)
		  {
		    printf("\n Input ERROR!Please input again!\n") ;
			printf("\n Please input the bit of bit-banding region(Peripherals)(0--7):0x");
		    region_bit=uart_input_u32();
		  }
	    alia_address=0x42000000+(region_address-0x40000000)*32+region_bit*4;
		printf("alia_address = 0x%X\n",alia_address);
	    bitbanding_test(region_address,region_bit,alia_address);
	 */
	    break;
      }
	  case '2':
	  {
	   printf("\n TEST OVER! \n");
	   while(1);
      } 
	  default : 
	  {
	   printf("\n Inpur ERROR!Please input again!\n") ;
	   break;
	  }
    }
  }
} 

/*******************************************************************************
* Function Name  : bitbanding_test
* Description    : Test the bit-banding
* Input          : region_address: address of region area
*                  region_bit    : bit of the tested byte in region area
*                  alia_address  : address of alia area  
* Output         : Output the result of this test
* Return         : None
*******************************************************************************/
void bitbanding_test(u32 region_address,u32 region_bit,u32 alia_address)
{
  int  result=1	; // Used to judge the result of testing.
  *(u8 *)(region_address)=(*(u8 *)(region_address)) | (1<<region_bit);
  //Write 1 to the bit of tested byte inregion area
  if(*(u32 *)(alia_address)==0) 
  {
    result=0;
    printf("\n test fail!\n");
    return ;
  }  
  *(u8 *)(region_address)=(*(u8 *)(region_address)) & ~(1<<region_bit);
   //Write 0 to the bit of tested byte in region area
  if(*(u32 *)(alia_address)==1) 
  {
    result=0;
    printf("\n test fail!\n");
    return ;
  } 
  *(u32 *)(alia_address)=1;
  //Write 1 to the tested word  in alia area
  if( *(u8 *)(region_address) & 1<<region_bit ==0)
  {
    result=1;
    printf("\n test fail!\n");
    return ;
  } 
  *(u32 *)(alia_address)=0;
  //Write 0 to the tested word in alia area
  if( *(u8 *)(region_address) & 1<<region_bit ==1)
  {
    result=1;
    printf("test fail!\n");
    return ;
  } 
 if(result==1) printf("\n test successful!\n");
}

/*******************************************************************************
* Function Name  : uart_input_u32
* Description    : Get an u32 hex number from hypertrm and end with a '\n'
* Input          : None 
* Output         : Output a clew when input an illegal char
* Return         : Return the number inputed from the hypertrm
*******************************************************************************/
u32 uart_input_u32(void)
{
  u32 sum=0;
  int  num=0;
  u32 digital[9];
  u32 temp1,temp2;
  int  i,j;
  while(1)
  {
    if(USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET)
	{
	  temp1=USART_ReceiveData(USART1);
	  printf("%c",temp1&0xFF);	   /* print the input char */
	  if(temp1==13)
	   { num--;
	     for( j=0;num>=0;num--,j++ )
		 {
		   for(temp2=1,i=0;i<num;i++)
		     {  temp2=temp2*16; }
		   if(digital[j]>='0'&&digital[j]<='9')
		      sum=sum+(digital[j]-48)*temp2;
		   else if(digital[j]>='a'&&digital[j]<='f')
		      sum=sum+(digital[j]-87)*temp2;
		   else if (digital[j]>='A'&&digital[j]<='F')
		      sum=sum+(digital[j]-55)*temp2;
		   else	 ; 
		   // translate the char to number    		    
		 }  
		 return  sum;
		 //Return the number inputed from the hypertrm
	   }
	  else if((temp1>='0'&&temp1<='9')|| (temp1>='a'&&temp1<='f') ||(temp1>='A'&&temp1<='F'))
	  {
	    digital[num]=temp1;
		num++;
	  }
	  else  { printf("\n Input ERROR!Please input again\n") ;  continue;}
   }
 }
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
void assert_failed (u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number */

  printf("\n\r Wrong parameter value detected on\r\n");
  printf("       file  %s\r\n", file);
  printf("       line  %d\r\n", line);
    
  /* Infinite loop */
  /* while (1)
  {
  } */
}
#endif

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
   
  /* Enable USART1 and GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
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
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  }

  return ch;
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

