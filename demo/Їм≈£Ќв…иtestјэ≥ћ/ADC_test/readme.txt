

(1)	在PC机上运行windows自带的超级终端串口通信程序（波特率115200、1位停止位、无校验位、无硬件流
控制）；或者使用其它串口通信程序；
(2)	点击MDK 的Debug菜单，点击Start/Stop Debug Session；
(3)	旋转电位器RV1，可以看到串口输出数值不断变化，正常显示结果如下所示。

usart1 print AD_value --------------------------
The current AD value = 0x0425
The current AD value = 0x0423
The current AD value = 0x0421
The current AD value = 0x0422
The current AD value = 0x0420
The current AD value = 0x0416
The current AD value = 0x03B6
The current AD value = 0x0841
The current AD value = 0x08C3
The current AD value = 0x08C0
The current AD value = 0x08BE
The current AD value = 0x09E9
The current AD value = 0x0A12
The current AD value = 0x0ACA
The current AD value = 0x0B0D
The current AD value = 0x0B10
The current AD value = 0x0B0E
....
....
(4)若想通过ITM Viewer来观测Printf数据，则需要加入#define DBG_ITM，并允许ITM跟踪，
详见RTX_Blinky例的说明。本例程中只要取消#define DBG_ITM前的注释符号即可。

