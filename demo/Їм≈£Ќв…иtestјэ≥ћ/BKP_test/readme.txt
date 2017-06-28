1设计要求
	对开发板上STM32处理器的BKP进行操作。复位后检测BKP的数据寄存器是否被修改，如果未被
	修改则LED1亮，否则LED2量；通过开发板上的Tamper按钮向侵入检测引脚TAMPER输入一个信号，
	在侵入事件处理中检查BKP的数据寄存器是否被复位，如果被复位则LED3亮，否则LED4亮。

2 硬件电路设计
	在评估板上，LED1、LED2、LED3、LED4分别与PC6、PC7、PC8、PC9相连，按键Tamper与PB13相连，
	如图7-44所示。

3软件程序设计
	根据设计任务要求，程序内容主要包括：
	(1)	配置ANTI_TAMP低电平有效，同时允许侵入事件中断。
	(2)	启动后读取备份数据寄存器，将其数据送PC机超级终端显示，并同电源复位前写入的20个字
	节数据进行比较，如果相同则LED1亮，否则LED2亮。
	(3)	当ANTI_TAMP(PC13)检测到低电平时（TAMPER按钮被按下），备份数据寄存器被复位并且产
	生一个侵入中断。相应的ISR检测备份数据寄存器是否被复位。如果是则LED3亮，否则LED4灯亮。
	整个工程包含3个源文件：STM32F10x.s、stm32f10x_it.c和main.c，其中STM32F10x.s为启动代码，
	所有中断服务子程序均在stm32f10x_it.c中，其它函数则在main.c中。
	下面分别介绍相关的函数，具体程序清单见参考程序。
	函数WriteToBackupReg可修改备份寄存器的内容；
	函数CheckBackupReg用于检测备份寄存器的内容；
	函数IsBackupRegReset用于查看备份寄存器是否复位；
	函数PrintBackupReg用于将备份寄存器的内容向串口打印输出。
	函数TAMPER_IRQHandler用于处理TAMPER中断。
	其他，例如GPIO、RCC等的配置函数在此不再作冗述。


4 运行过程
(1)	使用Keil uVision3 通过ULINK 2仿真器连接实验板，打开实验例程目录BKP_TEST子
目录下的BKP.Uv2例程，编译链接工程；
(2)	使用STM32开发板附带的串口线，连接开发板上的COM0和PC机的串口；
(3)	在PC机上运行Windows自带的超级终端串口通信程序（波特率115200、1位停止位、
无校验位、无硬件流控制）；或者使用其它串口通信程序；
(4)	选择硬件调试模式，点击MDK的Debug菜单，选择Start/Stop Debug Session项或Ctrl+F5
键，远程连接目标板并下载调试代码到目标系统中；
(5)	下载完成后，按下Reset键，超级终端显示为：
Now the data in DRx are:
DR1 = 0xA53C	DR2 = 0xA596	DR3 = 0xA5D2	DR4 = 0xA677	DR5 = 0xA67D
DR6 = 0xA6F5	DR7 = 0xA7F4	DR8 = 0xA8A8	DR9 = 0xA8C6	DR10 = 0xA99A
Led_1: on.--------------------The datas are as their initial status
(6)	按下Tamper键后，系统将复位DRx寄存器，超级终端显示为：
The Tamper Key has been pressed.
Led_3: on.--------------------DRx have been reset.
Now the data in DRx are:
DR1 = 0x0000	DR2 = 0x0000	DR3 = 0x0000	DR4 = 0x0000	DR5 = 0x0000	
DR6 = 0x0000	DR7 = 0x0000	DR8 = 0x0000	DR9 = 0x0000	DR10= 0x0000
(7)	按下Reset键，超级终端显示为：
Now the data in DRx are:
DR1 = 0x0000	DR2 = 0x0000	DR3 = 0x0000	DR4 = 0x0000	DR5 = 0x0000	
DR6 = 0x0000	DR7 = 0x0000	DR8 = 0x0000	DR9 = 0x0000	DR10 = 0x0000
Led_2: on.--------------------The datas have been changed.
Recover the datas of DRx to their initial status.
Now the data in DRx are:
DR1 = 0xA53C	DR2 = 0xA596	DR3 = 0xA5D2	DR4 = 0xA677	DR5 = 0xA67D	
DR6 = 0xA6F5	DR7 = 0xA7F4	DR8 = 0xA8A8	DR9 = 0xA8C6	DR10= 0xA99A	
(8)	再次按下Reset键，超级终端显示为：
Now the data in DRx are:
DR1 = 0xA53C	DR2 = 0xA596	DR3 = 0xA5D2	DR4 = 0xA677	DR5 = 0xA67D	
DR6 = 0xA6F5	DR7 = 0xA7F4	DR8 = 0xA8A8	DR9 = 0xA8C6	DR10= 0xA99A	
Led_1: on.--------------------The datas are as their initial status.

(6)若想通过ITM Viewer来观测Printf数据，则需要加入#define DBG_ITM，并允许ITM跟踪，
详见RTX_Blinky例的说明。本例程中只要取消#define DBG_ITM前的注释符号即可。
