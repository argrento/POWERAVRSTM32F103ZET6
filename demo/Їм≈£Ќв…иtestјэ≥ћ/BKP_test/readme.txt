1���Ҫ��
	�Կ�������STM32��������BKP���в�������λ����BKP�����ݼĴ����Ƿ��޸ģ����δ��
	�޸���LED1��������LED2����ͨ���������ϵ�Tamper��ť������������TAMPER����һ���źţ�
	�������¼������м��BKP�����ݼĴ����Ƿ񱻸�λ���������λ��LED3��������LED4����

2 Ӳ����·���
	���������ϣ�LED1��LED2��LED3��LED4�ֱ���PC6��PC7��PC8��PC9����������Tamper��PB13������
	��ͼ7-44��ʾ��

3����������
	�����������Ҫ�󣬳���������Ҫ������
	(1)	����ANTI_TAMP�͵�ƽ��Ч��ͬʱ���������¼��жϡ�
	(2)	�������ȡ�������ݼĴ���������������PC�������ն���ʾ����ͬ��Դ��λǰд���20����
	�����ݽ��бȽϣ������ͬ��LED1��������LED2����
	(3)	��ANTI_TAMP(PC13)��⵽�͵�ƽʱ��TAMPER��ť�����£����������ݼĴ�������λ���Ҳ�
	��һ�������жϡ���Ӧ��ISR��ⱸ�����ݼĴ����Ƿ񱻸�λ���������LED3��������LED4������
	�������̰���3��Դ�ļ���STM32F10x.s��stm32f10x_it.c��main.c������STM32F10x.sΪ�������룬
	�����жϷ����ӳ������stm32f10x_it.c�У�������������main.c�С�
	����ֱ������صĺ�������������嵥���ο�����
	����WriteToBackupReg���޸ı��ݼĴ��������ݣ�
	����CheckBackupReg���ڼ�ⱸ�ݼĴ��������ݣ�
	����IsBackupRegReset���ڲ鿴���ݼĴ����Ƿ�λ��
	����PrintBackupReg���ڽ����ݼĴ����������򴮿ڴ�ӡ�����
	����TAMPER_IRQHandler���ڴ���TAMPER�жϡ�
	����������GPIO��RCC�ȵ����ú����ڴ˲�����������


4 ���й���
(1)	ʹ��Keil uVision3 ͨ��ULINK 2����������ʵ��壬��ʵ������Ŀ¼BKP_TEST��
Ŀ¼�µ�BKP.Uv2���̣��������ӹ��̣�
(2)	ʹ��STM32�����帽���Ĵ����ߣ����ӿ������ϵ�COM0��PC���Ĵ��ڣ�
(3)	��PC��������Windows�Դ��ĳ����ն˴���ͨ�ų��򣨲�����115200��1λֹͣλ��
��У��λ����Ӳ�������ƣ�������ʹ����������ͨ�ų���
(4)	ѡ��Ӳ������ģʽ�����MDK��Debug�˵���ѡ��Start/Stop Debug Session���Ctrl+F5
����Զ������Ŀ��岢���ص��Դ��뵽Ŀ��ϵͳ�У�
(5)	������ɺ󣬰���Reset���������ն���ʾΪ��
Now the data in DRx are:
DR1 = 0xA53C	DR2 = 0xA596	DR3 = 0xA5D2	DR4 = 0xA677	DR5 = 0xA67D
DR6 = 0xA6F5	DR7 = 0xA7F4	DR8 = 0xA8A8	DR9 = 0xA8C6	DR10 = 0xA99A
Led_1: on.--------------------The datas are as their initial status
(6)	����Tamper����ϵͳ����λDRx�Ĵ����������ն���ʾΪ��
The Tamper Key has been pressed.
Led_3: on.--------------------DRx have been reset.
Now the data in DRx are:
DR1 = 0x0000	DR2 = 0x0000	DR3 = 0x0000	DR4 = 0x0000	DR5 = 0x0000	
DR6 = 0x0000	DR7 = 0x0000	DR8 = 0x0000	DR9 = 0x0000	DR10= 0x0000
(7)	����Reset���������ն���ʾΪ��
Now the data in DRx are:
DR1 = 0x0000	DR2 = 0x0000	DR3 = 0x0000	DR4 = 0x0000	DR5 = 0x0000	
DR6 = 0x0000	DR7 = 0x0000	DR8 = 0x0000	DR9 = 0x0000	DR10 = 0x0000
Led_2: on.--------------------The datas have been changed.
Recover the datas of DRx to their initial status.
Now the data in DRx are:
DR1 = 0xA53C	DR2 = 0xA596	DR3 = 0xA5D2	DR4 = 0xA677	DR5 = 0xA67D	
DR6 = 0xA6F5	DR7 = 0xA7F4	DR8 = 0xA8A8	DR9 = 0xA8C6	DR10= 0xA99A	
(8)	�ٴΰ���Reset���������ն���ʾΪ��
Now the data in DRx are:
DR1 = 0xA53C	DR2 = 0xA596	DR3 = 0xA5D2	DR4 = 0xA677	DR5 = 0xA67D	
DR6 = 0xA6F5	DR7 = 0xA7F4	DR8 = 0xA8A8	DR9 = 0xA8C6	DR10= 0xA99A	
Led_1: on.--------------------The datas are as their initial status.

(6)����ͨ��ITM Viewer���۲�Printf���ݣ�����Ҫ����#define DBG_ITM��������ITM���٣�
���RTX_Blinky����˵������������ֻҪȡ��#define DBG_ITMǰ��ע�ͷ��ż��ɡ�
