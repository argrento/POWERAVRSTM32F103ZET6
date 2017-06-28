/*******************************************************************************
* File Name          : Blinky.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 08/08/2008
* Description        : Main program body
********************************************************************************/
/*----------------------------------------------------------------------------
 *      R T L   K e r n e l   E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    BLINKY.C
 *      Purpose: RTX example program
 *      Rev.:    V3.20
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <stm32f10x_lib.h>              /* STM32F10x Library Definitions     */
#include <stdio.h>
#include "STM32_Init.h"                 /* STM32 Initialization              */
#include "LCD.h"

extern void SetupADC  (void);
extern void SetupLED(void);
extern unsigned short int ADC_ConvertedValue;
extern void lcd_putint(int v);

int AD_value;
int AD_scaled;

OS_TID t_phaseA;                        /* assigned task id of task: phase_a */
OS_TID t_phaseB;                        /* assigned task id of task: phase_b */
OS_TID t_phaseC;                        /* assigned task id of task: phase_c */
OS_TID t_phaseD;                        /* assigned task id of task: phase_d */
OS_TID t_lcd;                           /* assigned task id of task: lcd     */

#define LED_A   0x01
#define LED_B   0x02
#define LED_C   0x04
#define LED_D   0x08

#define LED_On(led)     GPIOF->ODR |=  (led << 6)
#define LED_Off(led)    GPIOF->ODR &= ~(led << 6)


/*----------------------------------------------------------------------------
 *        Function 'signal_func' called from multiple tasks
 *---------------------------------------------------------------------------*/
void signal_func (OS_TID task)  {
  
  AD_value  = ADC_ConvertedValue;        /* Read AD value                    */
  AD_scaled = AD_value/20 ;              /* AD value scaled to               */
  os_dly_wait (AD_scaled);   
  os_evt_set (0x0001, task);             /* send event to task 'task'        */
  os_dly_wait (AD_scaled);               /* delay AD_scaled clock ticks      */
}

/*----------------------------------------------------------------------------
 *        Task 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
void phaseA (void) __task {
  for (;;) {
    os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
    LED_On (LED_A);
    signal_func (t_phaseB);              /* call common signal function      */
    LED_Off(LED_A);
  }
}

/*----------------------------------------------------------------------------
 *        Task 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
void phaseB (void) __task {
  for (;;) {
    os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
    LED_On (LED_B);
    signal_func (t_phaseC);              /* call common signal function      */
    LED_Off(LED_B);
  }
}

/*----------------------------------------------------------------------------
 *        Task 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
void phaseC (void) __task {
  for (;;) {
    os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
    LED_On (LED_C);
    signal_func (t_phaseD);              /* call common signal function      */
    LED_Off(LED_C);
  }
}

/*----------------------------------------------------------------------------
 *        Task 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
void phaseD (void) __task {
  for (;;) {
    os_evt_wait_and (0x0001, 0xffff);    /* wait for an event flag 0x0001    */
    LED_On (LED_D);
    signal_func (t_phaseA);              /* call common signal function      */
    LED_Off(LED_D);
  }
}

/*----------------------------------------------------------------------------
 *        Task 8 'init': Initialize
 *---------------------------------------------------------------------------*/
void init (void) __task {

  t_phaseA = os_tsk_create (phaseA, 0);  /* start task phaseA                */
  t_phaseB = os_tsk_create (phaseB, 0);  /* start task phaseB                */
  t_phaseC = os_tsk_create (phaseC, 0);  /* start task phaseC                */
  t_phaseD = os_tsk_create (phaseD, 0);  /* start task phaseD                */

  os_evt_set (0x0001, t_phaseA);         /* send signal event to task phaseA */
  os_tsk_delete_self ();
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {

  stm32_Init ();                         /* STM32 setup (must be called first*/
  SetupLED();
  SetupADC(); 
  printf("Hello, ITM Viewer!");                   //ITM Viewer窗口显示信息    
  os_sys_init (init);                    /* Initialize RTX and start init    */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

