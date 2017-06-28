
/*********************************************************************/
 /*                     USART_PRINTP for STM32    
                             
                             2008.12.21
                            BY LiuDaChuan                                  */
/*********************************************************************/
/*********************************************************************/

/* printp() ------------ a simply version of printf()                */

 /**********************************************************************/
/* with the help of printp() function, you can get the out format as below:
   printp("This first line outputs the format string.\n");
   printp("%s\n","This second line outputs a string.");
   printp( "Notice that the first %s\n","two output lines had CR/LF at the end.");
   printp( "%c%c%c%c line outputs 'This' as chars.\n",'T', 'h', 'i', 's');
   printp( "Dec values: %d  %d   Hex value: %x\n",dec_value, neg_value, hex_value);
   printp( "Long dec values: %ld  %ld\n",ldec_value,lneg_value);
   printp( "Output sized strings: %20s %-20s\n","Right string","Left string");
   printp( "Output sized values:  %20d %-20d\n",dec_value,neg_value);
   printp( "                      %20x %-20x\n",hex_value,hex_value);
   printp( "                      %20ld %-20ld\n",ldec_value,ldec_value);
where the variables can be as:
   int hex_value = 0x1234;
   int dec_value = 1234;
   int neg_value = -1234;
   long ldec_value = 12345678L;
   long lneg_value = -12345678L;


修改了部分printp()代码，
 打印时如果 %后面的x,d为大写，打印的数不区分正负数
                      为小写，区分正负数
                 加l为取长型变量。

例如打印如下语句：

	printp("\n here i is 0xF123ABCD, j is 0xF123");
	printp("\n 1 : %%lX i is: %lX",i);
	printp("\n 2 : %%lx i is: %lx",i);
	printp("\n 3 : %%lD i is: %lD",i);
	printp("\n 4 : %%ld i is: %ld",i);
	printp("\n 5 : %%8X i is: %8X",i);
	printp("\n 6 : %%8x i is: %8x",i);
	printp("\n 7 : %%8D i is: %8D",i);
	printp("\n 8 : %%8d i is: %8d",i);
	printp("\n 9 :  %%X j is: %X",j);
	printp("\n 10:  %%x j is: %x",j);
	printp("\n 11:  %%D j is: %D",j);
	printp("\n 12:  %%d j is: %d",j);
结果为：
	 here i is 0xF123ABCD, j is 0xF123
	 1 : %lX i is: F123ABCD
	 2 : %lx i is: -EDC5433
	 3 : %lD i is: 4045646797
	 4 : %ld i is: -249320499
	 5 : %8X i is:     F123
	 6 : %8x i is:     -EDD
	 7 : %8D i is:    61731
	 8 : %8d i is:    -3805
	 9 :  %X j is: F123
	 10:  %x j is: -EDD
	 11:  %D j is: 61731
	 12:  %d j is: -3805  */
 
/************************************************************************/


#include "common.h"


/*******************************************************************************
* Function Name  : u8 putchar(u8 ch)
* Description    : 定义putchar，供printp使用
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
char usart_putchar(char ch)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART1->DR = (u8)ch;

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

	return ch;
}

/*******************************************************************************
* Function Name  : void prints(char* ctrl)
* Description    : 定义putchar，供printp使用
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void prints(char* ctrl)
{

    for(;*ctrl;ctrl++)
    {

        usart_putchar(*ctrl);

    }

}


#if PRINTP_EN==TRUE


static int do_padding;
static int left_flag;
static int len;
static int num1;
static int num2;
static char pad_character;

/*---------------------------------------------------*/
/*                                                   */
/* This routine puts pad characters into the output  */
/* buffer.                                           */
/*                                                   */
static void padding( const int l_flag)
{
   int i;

   if (do_padding && l_flag && (len < num1))
      for (i=len; i<num1; i++)
          usart_putchar( pad_character);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a string to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
static void outs( char* lp)
{
   /* pad on left if needed                          */
   len = strlen( lp);
   padding( !left_flag);

   /* Move string to the buffer                      */
   while (*lp && num2--)
      usart_putchar( *lp++);

   /* Pad on right if needed                         */
   len = strlen( lp);
   padding( left_flag);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a number to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */


  
   
	
static void reoutnum(unsigned long num, unsigned int negative, const long base ) 
{
   char* cp;
   char outbuf[32];
   const char digits[] = "0123456789ABCDEF";
   
   /* Build number (backwards) in outbuf             */
   cp = outbuf;
   do {
      *cp++ = digits[(int)(num % base)];
      } while ((num /= base) > 0);
   if (negative)
      *cp++ = '-';
   *cp-- = 0;

   /* Move the converted number to the buffer and    */
   /* add in the padding where needed.               */
   len = strlen(outbuf);
   padding( !left_flag);
   while (cp >= outbuf)
      usart_putchar( *cp--);
   padding( left_flag);
}


static void outnum(long num, const long base ,unsigned char sign)//1, signed  0 unsigned
{   unsigned int negative;

   if ( (num < 0L) && sign ) 
   {  negative=1;
      num = -num;
      }
   else negative=0;
   
   reoutnum(num,negative,base);  
} 
/*---------------------------------------------------*/
/*                                                   */
/* This routine gets a number from the format        */
/* string.                                           */
/*                                                   */
static int getnum( char** linep)
{
   int n;
   char* cp;

   n = 0;
   cp = *linep;
   while (isdigit(*cp))
      n = n*10 + ((*cp++) - '0');
   *linep = cp;
   return(n);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine operates just like a printf/sprintf  */
/* routine. It outputs a set of data under the       */
/* control of a formatting string. Not all of the    */
/* standard C format control are supported. The ones */
/* provided are primarily those needed for embedded  */
/* systems work. Primarily the floaing point         */
/* routines are omitted. Other formats could be      */
/* added easily by following the examples shown for  */
/* the supported formats.                            */
/*                                                   */

void printp( char* ctrl, ...)
{

   int long_flag;
   int dot_flag;

   char ch;
   va_list argp;

   va_start( argp, ctrl);

   for ( ; *ctrl; ctrl++) {

      /* move format string chars to buffer until a  */
      /* format control is found.                    */
      if (*ctrl != '%') {
         usart_putchar(*ctrl);
#if CR_as_CRLF==TRUE         
         if(*ctrl=='\n') usart_putchar('\r');
#endif         
         continue;
         }

      /* initialize all the flags for this format.   */
      dot_flag   =
      long_flag  =
      left_flag  =
      do_padding = 0;
      pad_character = ' ';
      num2=32767;

try_next:
      ch = *(++ctrl);

      if (isdigit(ch)) {
         if (dot_flag)
            num2 = getnum(&ctrl);
         else {
            if (ch == '0')
               pad_character = '0';

            num1 = getnum(&ctrl);
            do_padding = 1;
         }
         ctrl--;
         goto try_next;
      }
      
      switch (tolower(ch)) {
         case '%':
              usart_putchar( '%');
              continue;

         case '-':
              left_flag = 1;
              break;

         case '.':
              dot_flag = 1;
              break;

         case 'l':
              long_flag = 1;
              break;

         case 'd':
              if (long_flag ==1 ) {
              		if(ch == 'D')                {outnum( va_arg(argp, unsigned long), 10L , 0);continue;}
              	        else  /* ch == 'd' */        {outnum( va_arg(argp, long), 10L,1);continue;}
                 }
              else {
              		if(ch == 'D')                {outnum( va_arg(argp, unsigned int),10L,0);continue;}
              		else  /* ch == 'd' */        {outnum( va_arg(argp, int), 10L,1);continue;}
                }
                
         case 'x':    // X 无符号 ， x  有符号
              if (long_flag ==1 )  {
              	        if(ch == 'X')                {outnum( va_arg(argp, unsigned long), 16L,0);continue;}
              	        else  /* ch == 'x' */        {outnum( va_arg(argp, long), 16L,1);continue;} 
                 }
              else {
              		if(ch == 'X')                {outnum( va_arg(argp, unsigned int), 16L,0);continue;}
              		else  /* ch == 'x' */        {outnum( va_arg(argp, int), 16L,1);continue;}
                 } //如果按照16进制打印，将全部按照无符号数进行
              continue;

         case 's':
              outs( va_arg( argp, char*));
              continue;

         case 'c':
              usart_putchar( va_arg( argp, int));
              continue;

         default:
              continue;
         }
      goto try_next;
      }
   va_end( argp);
   }

#endif  /*#if PRINTP_EN==TRUE结束*/

