/*----------------------------------------------------------------------------
 *      R T L  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.C
 *      Purpose: Configuration of RTX Kernel for Cortex-M
 *      Rev.:    V3.21
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>

/*----------------------------------------------------------------------------
 *      RTX User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Task Definitions
// ===================
//
//   <o>Number of concurrent running tasks <0-250>
//   <i> Define max. number of tasks that will run at the same time.
//   <i> Default: 6
#ifndef OS_TASKCNT
 #define OS_TASKCNT     10
#endif

//   <o>Number of tasks with user-provided stack <0-250>
//   <i> Define the number of tasks that will use a bigger stack.
//   <i> The memory space for the stack is provided by the user.
//   <i> Default: 0
#ifndef OS_PRIVCNT
 #define OS_PRIVCNT     0
#endif

//   <o>Task stack size [bytes] <20-4096:8><#/4>
//   <i> Set the stack size for tasks which is assigned by the system.
//   <i> Default: 200
#ifndef OS_STKSIZE
 #define OS_STKSIZE     50
#endif

// <q>Check for the stack overflow
// ===============================
// <i> Include the stack checking code for a stack overflow.
// <i> Note that additional code reduces the Kernel performance.
#ifndef OS_STKCHECK
 #define OS_STKCHECK    1
#endif

// <q>Run in privileged mode
// =========================
// <i> Run all Tasks in privileged mode.
// <i> Default: Unprivileged
#ifndef OS_RUNPRIV
 #define OS_RUNPRIV     0
#endif

//   <o>Number of user timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OS_TIMERCNT
 #define OS_TIMERCNT    0
#endif

// </h>
// <h>SysTick Timer Configuration
// =============================
//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Set the timer clock value for selected timer.
//   <i> Default: 6000000  (6MHz)
#ifndef OS_CLOCK
 #define OS_CLOCK       72000000
#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer.
//   <i> Default: 10000  (10ms)
#ifndef OS_TICK
 #define OS_TICK        10000
#endif

// </h>
// <e>Round-Robin Task switching
// =============================
// <i> Enable Round-Robin Task switching.
#ifndef OS_ROBIN
 #define OS_ROBIN       1
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Define how long a task will execute before a task switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
 #define OS_ROBINTOUT   5
#endif

// </e>

//------------- <<< end of configuration section >>> -----------------------

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/

#define MAGIC_WORD      0xE25A2EA5
#define OS_TRV          ((U32)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/
U16 const os_maxtaskrun = OS_TASKCNT;
U16 const os_stksize    = OS_STKSIZE;
U32 const os_trv        = OS_TRV;
U32 const os_magic_word = MAGIC_WORD;
U8  const os_flags      = OS_RUNPRIV;

/* Export following defines to uVision debugger. */
#pragma push
#pragma O0
U32 const os_stackinfo = (OS_STKCHECK<<24) | (OS_PRIVCNT<<16) | (OS_STKSIZE*4);
U32 const os_rrobin    = (OS_ROBIN << 16) | OS_ROBINTOUT;
U32 const os_clockrate =  OS_TICK;
U32 const os_timernum  =  OS_TIMERCNT;
#pragma pop

/* Memory pool for TCB allocation    */
_declare_box (mp_tcb, OS_TCB_SIZE, OS_TASKCNT);
U16 const mp_tcb_size = sizeof(mp_tcb);

/* Memory pool for System stack allocation (+ os_idle_demon). */
_declare_box8 (mp_stk, OS_STKSIZE*4, OS_TASKCNT-OS_PRIVCNT+1);
U32 const mp_stk_size = sizeof(mp_stk);

/* An array of Active task pointers. */
void *os_active_TCB[OS_TASKCNT];

#if (OS_TIMERCNT != 0)
 /* Memory pool for User Timer allocation */
 _declare_box (mp_tmr, OS_TMR_SIZE, OS_TIMERCNT);
 U32 const *m_tmr = &mp_tmr[0];
 U16 const mp_tmr_size = sizeof(mp_tmr);
#else
 U32 const *m_tmr = NULL;
 U16 const mp_tmr_size = 0;
#endif

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- os_idle_demon ---------------------------------*/

void os_idle_demon (void) __task {
   /* The idle demon is a system task. It is running when no other task is */
   /* ready to run (idle situation). It must not terminate. Therefore it   */
   /* should contain at least an endless loop.                             */

   for (;;) {
   /* HERE: include optional user code to be executed when no task runs.*/
   }
} /* end of os_idle_demon */


/*--------------------------- os_tmr_call -----------------------------------*/

void os_tmr_call (U16 info) {
   /* This function is called when the user timer has expired.             */
   /* Parameter "info" is the parameter defined when the timer was created.*/
   /* HERE: include here optional user code to be executed on timeout.     */

} /* end of os_tmr_call */

/*--------------------------- os_stk_overflow -------------------------------*/

void os_stk_overflow (OS_TID task_id) {
   /* This function is called when a stack overflow is detected.           */
   /*  'task_id'    holds a task id for this task                          */
   /* HERE: include here optional code to be executed on stack overflow.   */
   
   /* Use a uVision 'RTX Kernel' debug dialog page 'Active Tasks' to      */
   /* check which task has got a stack overflow and needs a bigger stack. */
   for (;;);
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

