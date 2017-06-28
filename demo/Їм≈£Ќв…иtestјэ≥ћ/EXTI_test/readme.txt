1设计要求
	将Tamper按钮对应的PC13引脚配置为外部中断，当其上出现下降沿时产生一个中断，在中断服务
	子程序中会通过串口输出提示信息。
2 硬件电路设计
	在评估板上按键Tamper与PC.13相连。
3 软件程序设计
	根据设计要求，软件需实现以下任务：
	(1)	配置NVIC控制器、EXTI控制器，将PC13引脚配置为EXTI13，下降沿触发；
	(2)	通过软件触发一次EXTI13；
	(3)	在中断服务子程序中通过串口向外输出一个字符串。
	整个工程包含3个源文件：STM32F10x.s、stm32f10x_it.c和main.c，其中STM32F10x.s为启动代
	码，所有中断服务子程序均在stm32f10x_it.c中，其它函数则在main.c中。
4 运行过程
	(1)	用STM32V100评估板附带的串口线，连接实验板上的UART0和PC机的串口；
	(2)	在PC机上运行windows自带的超级终端串口通信程序（波特率115200、1位停止位、无校验位、
	无硬件流控制），或者使用其它串口通信程序；
	(3)	使用Keil uVision3 通过ULINK2仿真器连接评估板，打开实验例程EXTI_test子目录下的
	EXTI.Uv2例程，编译链接工程；
	(4)	点击MDK 的Debug菜单，选择Start/Stop Debug Session项或Ctrl+F5键，远程连接目标板并
	下载调试代码到目标系统RAM中；
	(5)	程序正常启动运行后，超级终端上将出现：
		System boot success!  
		The 13th external interrupt happend! 
	(6)	每次按下Tamper按钮，超级终端就打印一次字符串"The 13th external interrupt happend!"。
    (7)若想通过ITM Viewer来观测Printf数据，则需要加入#define DBG_ITM，并允许ITM跟踪，
详见RTX_Blinky例的说明。本例程中只要取消#define DBG_ITM前的注释符号即可。
	(8)	若无评估板，读者也可以使用软件仿真模式来完成程序运行。