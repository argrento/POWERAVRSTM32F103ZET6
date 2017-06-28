
(1)	使用STM32开发板附带的串口线，连接开发板上的COM0和PC机的串口；
(2)	在PC机上运行Windows自带的超级终端串口通信程序（波特率115200、1位停止位、无校验位、
无硬件流控制）；或者使用其它串口通信程序；
(3)	选择硬件调试模式，点击MDK的Debug菜单，选择Start/Stop Debug Session项或Ctrl+F5键，
远程连接目标板并下载调试代码到目标系统中；
(4)	例程正常运行之后会在超级终端显示以下信息：
RTC not yet configured....
 RTC configured....
============TimeSettings===================
  Please Set Hours:
在PC机上依次输入时钟、分钟、秒钟之后每隔1秒在超级终端上显示一次时间：
  Please Set Hours:  12
  Please Set Minutes:  0
  Please Set Seconds:  0
Time: 12:00:00
同时开发板的LED1灯也会每隔1S闪烁一次。
(5)	程序正常运行并在开发外部电源保持的情况下，按下Reset按钮，PC超级终端上将继续显示正常时间：
External Reset occurred....
 No need to configure RTC....
Time: 12:03:09
(6)	程序正常运行时断开开发板外部电源，然后重新接上外部电源，PC超级终端上也将继续显示正常时间：
Power On Reset occurred....
 No need to configure RTC....
Time: 12:05:57

(7)	取下处理器板上的纽扣电池，并断开外部电源，然后重新接上外部电源，PC超级终端上将无法继续
正常显示时间，PC超级终端将出现第（4）步所出现内容。
(8)	也可采用软件调试模式，利用USART窗口来模拟实现COM0的输入和输出。
