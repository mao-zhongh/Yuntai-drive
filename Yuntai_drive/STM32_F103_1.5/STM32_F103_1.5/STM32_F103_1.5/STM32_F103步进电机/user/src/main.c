#include "main.h"



enum PELCO_D_CMD
{
	ZOOM_IN = 0,																																	/* �����ƶ�   0*/
	ZOOM_OUT ,																																		/* �����ƶ�   1*/
	Mount,                                                                        /* �����ƶ�   2*/
	Down,                                                                         /* �����ƶ�   3*/
	
	set_1,                                                                        /* Ԥ��λ1����       4*/
	call_1,                                                                       /* Ԥ��λ1����       5*/
	set_2,																																				/* Ԥ��λ2����       6*/
	call_2,                                                                       /* Ԥ��λ2����       7*/
	set_3,																																				/* Ԥ��λ3����       8*/
	call_3,                                                                       /* Ԥ��λ3����       9*/		
	set_4,																																				/* Ԥ��λ4����      10*/
	call_4,                                                                       /* Ԥ��λ4����      11*/			
	set_5,																																				/* Ԥ��λ5����      12*/
	call_5,                                                                       /* Ԥ��λ5����      13*/	

	
	STOP ,																																				/* ֹͣ	 						4*/
	reset ,																																				/* ����	 						5*/
};

/****************************************************************************
PELCO_D Э��˵����
���ݸ�ʽ��1λ��ʼλ��8λ���ݡ�1λֹͣλ����У��λ;
�����ʽ��|  �ֽ�1   |  �ֽ�2 |  �ֽ�3   |   �ֽ�4  |  �ֽ�5   |   �ֽ�6  |  �ֽ�7 |
					  ͬ���ֽ�   ��ַ��    ָ����1    ָ����2    ������1    ������2		У����

1.��Э����������ֵ��Ϊʮ��������
2.ͬ���ֽ�ʼ��ΪFFH
3.��ַ��Ϊ��������߼���ַ�ţ���ַ��Χ��00H~FFH
4.ָ�����ʾ��ͬ�Ķ���
5.������1��2�ֱ��ʾˮƽ����ֱ�����ٶ�(00~3FH),FFH��ʾ��turbo�� �ٶ�
6.У���� = MOD[(�ֽ�2+�ֽ�3+�ֽ�4+�ֽ�5+�ֽ�6)/100H]
			���µ�ַ��Ϊ0x01
****************************************************************************/

unsigned char PELCO_D[][7] = {
	/* ��ת ��ʱ��*/
	{0xff,0x01,0x00,0x04,0x0F,0x00,0x14},//ff 01 00 04 0f 00 14

	/* ��ת ˳ʱ�� */	
	{0xff,0x01,0x00,0x02,0x0F,0x00,0x12},//ff 01 00 02 0f 00 12

	/* ���� */
	{0xFF,0x01,0x00,0x08,0x00,0x1C,0x25},//FF 01 00 08 00 1C 25 
	
	/* ���� */
	{0xFF,0x01,0x00,0x10,0x00,0x1C,0x2D},//FF 01 00 10 00 1C 2D 	
	
	/* Ԥ��λ1���� */
	{0xff,0x01,0x00,0x03,0x00,0x01,0x05},//ff 01 00 03 00 01 05
	
	/* Ԥ��λ1���� */
	{0xff,0x01,0x00,0x07,0x00,0x01,0x09},//ff 01 00 07 00 01 09
	
	/* Ԥ��λ2���� */	
	{0xFF,0x01,0x00,0x03,0x00,0x02,0x06},//ff 01 00 03 00 02 06

	/* Ԥ��λ2���� */
	{0xFF,0x01,0x00,0x07,0x00,0x02,0x0A},//ff 01 00 07 00 02 0a	
	
	/* Ԥ��λ3���� */	
	{0xFF,0x01,0x00,0x03,0x00,0x03,0x07},//ff 01 00 03 00 03 07

	/* Ԥ��λ3���� */
	{0xFF,0x01,0x00,0x07,0x00,0x03,0x0B},//ff 01 00 07 00 03 0b		
	
	
	/* Ԥ��λ4���� */
	{0xff,0x01,0x00,0x03,0x00,0x04,0x08},//ff 01 00 03 00 04 08
	
	/* Ԥ��λ4���� */
	{0xff,0x01,0x00,0x07,0x00,0x04,0x0c},//ff 01 00 07 00 04 0c

	/* Ԥ��λ5���� */
	{0xff,0x01,0x00,0x03,0x00,0x05,0x09},//ff 01 00 03 00 05 09

	/* Ԥ��λ5���� */
	{0xff,0x01,0x00,0x07,0x00,0x05,0x0D},//ff 01 00 07 00 05 0d	
	
	
	

	/* ֹͣ */
	{0xff,0x01,0x00,0x00,0x00,0x00,0x01},//ff 01 00 00 00 00 01

	/* ���� �ع���ʼλ */
	{0xff,0x01,0x98,0x00,0x0f,0x11,0xb9},//FF 01 98 00 0F 11 B9 
};


/************************************����Ԥ��λ*****************************************/
/*
�������ܣ����ݵ�ǰλ��ֵ�������Ԥ��λ��(current position)������ֵ
����������( step_A(������ڵ�λ��)   step_B(Ԥ��λ��λ��)    , count ��Ա )
��������ֵ��none   
��ע��		1/500 = 0
*/
unsigned int DRV_Current_Position(unsigned int step_A,unsigned int step_B,unsigned char count )      //DRV
{
		if( Drv_Direction == FLASH_DATA.DRVROM_Dir[count] )//����ͬһ����ʱ
		{
			if( step_A > step_B )
			return step_B = step_A - step_B;
			//FLASH_DATA.DRVROM_Dir != ROM_Dir;
			
			else //if(step_A < step_B )
			{
				FLASH_DATA.DRVROM_Dir[count] =! FLASH_DATA.DRVROM_Dir[count];					
				return step_B = step_B - step_A;								
			}			
		}
		else //if( Drv_Direction != ROM_Dir )								
		{
			FLASH_DATA.DRVROM_Dir[count] =! FLASH_DATA.DRVROM_Dir[count];
			return step_B = step_B + step_A;
		}
}
/*
�������ܣ����ݵ�ǰλ��ֵ�������Ԥ��λ��(current position)������ֵ
����������( step_A(������ڵ�λ��)   step_B(Ԥ��λ��λ��)    , count ��Ա )
��������ֵ��none   
��ע��		1/500 = 0
*/
unsigned int ST_Current_Position(unsigned int step_A,unsigned int step_B,unsigned char count )     //ST
{
		if( ST_Direction == FLASH_DATA.STROM_Dir[count] )//����ͬһ����ʱ
		{
			if( step_A > step_B )
			return step_B = step_A - step_B;
			
			else
			{
				FLASH_DATA.STROM_Dir[count] =! FLASH_DATA.STROM_Dir[count];					
				return step_B = step_B - step_A;								
			}			
		}
		else 							
		{
			FLASH_DATA.STROM_Dir[count] =! FLASH_DATA.STROM_Dir[count];
			return step_B = step_B + step_A;
		}
}
/************************************����Ԥ��λ*****************************************/
void Ser_Presetting_bit(u8 numbers,u32 addr_)
{
		FLASH_Unlock();                    								 //����	
		Delay_us(100);	
		STMFLASH_Read( addr_ );  //��������ֵ
		//ZOOM_IN,ZOOM_OUT,Mount,Down	
		FLASH_DATA.DRVROM_Step[numbers] = DRV_Compare_TurnMeter();    //�Ƶ�ǰλ����ԭ�㲽��
		FLASH_DATA.DRVROM_Dir[numbers] = Drv_Direction;
		FLASH_DATA.STROM_Step[numbers] = ST_Compare_TurnMeter();
		FLASH_DATA.STROM_Dir[numbers] = ST_Direction;										

		STMFLASH_Writ( addr_ ); //��һ������дֵ

		Delay_us(10);	
}
/************************************����Ԥ��λ*****************************************/

/************************************����Ԥ��λ*****************************************/
u8 Call_presetting_bit(u8 numbers,u32 addr)
{
	int dev_step_=0,st_step=0;
	u8 temp_res=0;

	
	dev_step_ = DRV_Compare_TurnMeter();    	//���ص�ǰλ��ֵ
	st_step  = ST_Compare_TurnMeter();
	/*���صĶ��Ǹ�����*/
	//dev_step_ = abs(dev_step_);					    //abs����:ȡ����ֵ; abs��-123��= 123��
	
	Read_DRV_EEPROM(addr,numbers); 					//DRV��һ��Ԥ��λֵ
  //ST��ַ��DRV��ַ������8�ֽ�
	Read_ST_EEPROM((addr+=8),numbers); 			//ST ��һ��Ԥ��λֵ        

	
//	temp_res = DRV_Drive(FLASH_DATA.DRVROM_Dir[numbers],DRV_Current_Position( dev_step,DRVROM_Step[numbers],numbers));
	dev_step_ = DRV_Current_Position( (unsigned int)dev_step_,(unsigned int)FLASH_DATA.DRVROM_Step[numbers],numbers);
	/* ���ݷ���ת��'-,+'���� */
	(FLASH_DATA.DRVROM_Dir[numbers] == 1) ? (dev_step_ = dev_step_) : ( dev_step_ = ~dev_step_ +1);
	temp_res = DRV_Drive(dev_step_);
	if(temp_res)      { return 1; }		 /*��;��������ָ��*/
	
	
	Delay_us(10);		
	
	
	st_step = ST_Current_Position( (unsigned int)st_step,(unsigned int)FLASH_DATA.STROM_Step[numbers],numbers);
	(FLASH_DATA.STROM_Dir[numbers] == 1) ? (st_step = st_step) : ( st_step = ~st_step +1);
	temp_res = ST820_Drive(st_step); 
	if(temp_res)      { return 1; }		

	return 0;	
}
/***************************��ȡflash�д洢����ǰ��λ��,���ص�ԭ��,���ȼ����****************************************/
void Restart_MOTOR(void)
{
/*                       flash����                          */
	USART1->CR1 &= ~(0X1<<13);//USART1ʧ��;
	restart = 1;

//	FLASH_DATA.DRVROM_Dir[0] = 0;
//	FLASH_DATA.DRVROM_Step[0] =0;
//	FLASH_DATA.STROM_Dir[0] =0;
//	FLASH_DATA.STROM_Step[0] =0;
	

	Read_DRV_EEPROM(FLASH_Presetting_DRV,0);//��
	Read_ST_EEPROM(FLASH_Presetting_ST,0); //��

	Delay_ms(500);		Delay_ms(500);	
	
//	DRV_Drive(1);
	((FLASH_DATA.STROM_Dir[0]&0x01)  == st_right) ? (ST820_Drive(FLASH_DATA.STROM_Step[0])) : ( ST820_Drive((-FLASH_DATA.STROM_Step[0])) );	
	((FLASH_DATA.DRVROM_Dir[0]&0x01) == drv_down) ? (DRV_Drive(FLASH_DATA.DRVROM_Step[0]) ) : ( DRV_Drive((-FLASH_DATA.DRVROM_Step[0])) );	


//	printf("FLASH_DATA.DRVROM_Dir[0] == %d\r\n",FLASH_DATA.DRVROM_Dir[0]);
//	printf("dev_step == %d\r\n",FLASH_DATA.DRVROM_Step[0]);
//	printf("FLASH_DATA.STROM_Dir[0] == %d\r\n",FLASH_DATA.STROM_Dir[0]);
//	printf("STROM_Step == %d\r\n",FLASH_DATA.STROM_Step[0]);		
	
	/*��ֹһֱ���ص�Դ*/
	STMFLASH_ErasePage(FLASH_Presetting);   //����	
	Write_EEPROM(0,FLASH_Presetting_DRV,0); //�ϵ縴λ��¼��д0		
	Write_EEPROM(0,FLASH_Presetting_ST,0);  //�ϵ縴λ��¼��д0		

	st_spta.TurnMeter = 0; 
	drv_spta.TurnMeter = 0;	
	restart = 0;
	USART1->CR1 |= 0X1<<13;//USART1ʹ��;
/*                       flash����                          */	
}


int main(void)
{

	unsigned char reverse = STOP;								//���ָ��	
	unsigned char temp_IN =0;										//��Ӧ�жϷ�              
	int dev_step=0;    //�����ʱram�ռ䲽��
	int st_step=0;
	
//	����Ĵ���ֻ��ִ��һ��  ��ʼ������
	NVIC_SetPriorityGrouping(7-2);  //2��ռ(0-3)   2��Ӧ(0-3)	
	
	

	/*LED��ʼ��*/
	LED_Iint();
	
	/*���ڳ�ʼ��*/
	Usart1__Init(115200);
	
	/*���IO��ʼ��*/
	DRV8825_Init();
	ST820_Init();

	/*TIM4->PWM3_4ͨ����ʼ��*/
	TIM4_Tint_PWM();/*	 70us����  14.287Khz  DRV */
	
 /* ����� 7.5��/85  [Full_stepʱ11.333(step)Ϊ1�㣻4079.999(step)��360��] */
 /* [8159 1/2]  [16320 1/4]  [32640 1/8]   [65280 1/16]   [130560 1/32]   [522240 1/128]  [1044480 1/256]���н����� */	
 /* ϸ�������� */
	drv_32_step;
	st_256_step;
	
	/*ϵͳʱ�ӳ�ʼ��->����1ms�ж�*/	
	Systick_Init(); 	Iwdg_Init();  //   �������Ź� ->1s��⣻	
	
//	FLASH_SetLatency(FLASH_Latency_2);       //FLASH�����ӳ� 72Mhz������ʱ����
	Delay_ms(500);	
	/*���ݵ���ǰλִ�и�λԭ��*/
	Restart_MOTOR();


		
	Delay_ms(100);
	printf("��λ��\r\n");

	while(1)
	{		

#if 1
Interrupt_to_bothe:			
			if(UART1_Overflow_Flag)						//����������־		�� UART1_Recv_Buf[] �Ѵ���˿��ԱȽϵ����ݣ�
			{ 	
					/* �䱶�� (�Ŵ�+) 0*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[ZOOM_IN][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[ZOOM_IN][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[ZOOM_IN][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[ZOOM_IN][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[ZOOM_IN][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[ZOOM_IN][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[ZOOM_IN][6]))
						{
								reverse = ZOOM_IN;   		//��ת Dir1
						}

						/* �䱶�� ��С- 	  1*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[ZOOM_OUT][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[ZOOM_OUT][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[ZOOM_OUT][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[ZOOM_OUT][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[ZOOM_OUT][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[ZOOM_OUT][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[ZOOM_OUT][6]))
						{
							reverse = ZOOM_OUT;   		//��ת Dir0
						}
						
		 if(	 	 (UART1_Recv_Buf[0] == PELCO_D[Mount][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[Mount][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[Mount][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[Mount][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[Mount][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[Mount][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[Mount][6]))
						{
							reverse = Mount;   		//���� Dir0
						}					
						

		 if(	 	 (UART1_Recv_Buf[0] == PELCO_D[Down][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[Down][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[Down][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[Down][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[Down][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[Down][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[Down][6]))
						{
							reverse = Down;   		//���� Dir1
						}							

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_1][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_1][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_1][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_1][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_1][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_1][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_1][6]))
						{
							reverse = set_1;   				//Ԥ��λ1����
						}						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_1][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_1][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_1][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_1][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_1][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_1][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_1][6]))
						{
							reverse = call_1;   				//Ԥ��λ1����
						}									
						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_2][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_2][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_2][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_2][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_2][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_2][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_2][6]))
						{
							reverse = set_2;   				//Ԥ��λ2����
						}						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_2][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_2][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_2][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_2][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_2][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_2][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_2][6]))
						{
							reverse = call_2;   				//Ԥ��λ2����
						}		
												
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_3][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_3][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_3][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_3][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_3][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_3][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_3][6]))
						{
							reverse = set_3;   				//Ԥ��λ3����
						}						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_3][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_3][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_3][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_3][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_3][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_3][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_3][6]))
						{
							reverse = call_3;   				//Ԥ��λ3����
						}								

						
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_4][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_4][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_4][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_4][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_4][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_4][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_4][6]))
						{
							reverse = set_4;   				//Ԥ��λ4����
						}		


					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_4][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_4][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_4][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_4][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_4][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_4][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_4][6]))
						{
							reverse = call_4;   				//Ԥ��λ4����
						}								
						
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_5][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_5][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_5][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_5][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_5][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_5][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_5][6]))
						{
							reverse = set_5;   				//Ԥ��λ5����
						}		


					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_5][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_5][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_5][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_5][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_5][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_5][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_5][6]))
						{
							reverse = call_5;   				//Ԥ��λ5����
						}	
						
						/* ֹͣ	 						2*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[STOP][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[STOP][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[STOP][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[STOP][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[STOP][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[STOP][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[STOP][6]))
						{
							reverse = STOP;   				//ֹͣ�ƶ�							
						}

					  /* ���� �ع�ԭλ 3*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[reset][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[reset][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[reset][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[reset][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[reset][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[reset][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[reset][6]))
						{
							reverse = reset;   				//��λָ��		
						}						


						
						
						
					UART1_Flag_count = 0;  	 			//����ͳ�ƽ��մ���
					UART1_Overflow_Flag = 0;			//�����ڼ����������� 

						
						
						
			}

		switch(reverse)
			{		
				
				case ZOOM_IN : //LED_1 =! LED_1;
											st_step = ST_Compare_TurnMeter();
											if(st_step >= st_scope && ST_Direction == st_right ) { st_step = 0; break; }//���ص����ת�����־ 65280��45��ȵ�    
											
											temp_IN = ST820_Drive(-130560);// ��˳  45��
											if( temp_IN ) { goto Interrupt_to_bothe; }			/* reverse = STOP; */	
											Delay_us(100);											
											break;

				case ZOOM_OUT://LED_1 =! LED_1;
											st_step = ST_Compare_TurnMeter();
											if(st_step >= st_scope && ST_Direction == st_left) { st_step = 0; break; }//���ص����ת�����־   ����goto �����־��
											temp_IN = ST820_Drive(130560);//  ����45��
											if( temp_IN ) { goto Interrupt_to_bothe; }
											Delay_us(100);
											break;

				case Mount	 ://LED_1 =! LED_1;

											dev_step = DRV_Compare_TurnMeter();       //������λ,�˳�
											if(dev_step >= drv_scope ) { dev_step = 0; break; };
											temp_IN = DRV_Drive(16320);//8160 ��˳ 45���ƶ�  
											if( temp_IN ) { goto Interrupt_to_bothe; }
											Delay_us(100);
											break;
				

				case Down		 :		//LED_1 =! LED_1;		
											dev_step = DRV_Compare_TurnMeter();
											if(dev_step == drv_up ) { dev_step = 0; break; }//������λ,�˳� 
											
											temp_IN = DRV_Drive(-16320);//8160 ���� 45���ƶ�
											if( temp_IN ) { goto Interrupt_to_bothe; }				
											Delay_us(100);											
											break;		
				

											
				case set_1   :	/* 0123 һ������Ԥ��λ  */
											Ser_Presetting_bit(0,FLASH_start_ADDRESS);
											Delay_us(100);
											break;					

				
				case call_1	 :					
											temp_IN = Call_presetting_bit(0,one_ADDRESS); 
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//˵����Ӧ�ж�
											Delay_us(100);
											break;

				case set_2   :	

											Ser_Presetting_bit(1,FLASH_start_ADDRESS);			
											Delay_us(100);
											break;
				
				
				case call_2	 :

											temp_IN = Call_presetting_bit(1,two_ADDRESS );
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//˵����Ӧ�ж�				
											Delay_us(100);
											break;
		/******************************/	
				case set_3   :	

											Ser_Presetting_bit(2,FLASH_start_ADDRESS);											
											Delay_us(100);
											break;
				
				
				case call_3	 :

											temp_IN = Call_presetting_bit( 2,three_ADDRESS );	
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//˵����Ӧ�ж�						
											Delay_us(100);
											break;
	
				case set_4   :	

											Ser_Presetting_bit(3,FLASH_start_ADDRESS);											
											Delay_us(100);
											break;
				
				case call_4	 :
					
											temp_IN = Call_presetting_bit( 3,foue_ADDRESS );
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//˵����Ӧ�ж�						
											Delay_us(100);				
											break;				
				
				
				case set_5   :	
					
											Ser_Presetting_bit(4,FLASH_start_ADDRESS);
											Delay_us(100);
											break;

				case call_5	 :
		
											temp_IN = Call_presetting_bit(4,five_ADDRESS);
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//˵����Ӧ�ж�						
											Delay_us(100);				
											break;					
				
				
				
				
				
				case STOP    :
											motor_disturb = 0;	
											TIM4->CR1 &= ~(0x1<<0);			/*  ʧ�ܼ�����  */	
											ENABLE = 1;									/* ��ʱʧ��ֹ�� */
											ST_ENABLE = 0;              

											/*��¼ֹͣǰλ��*/
											STMFLASH_ErasePage(FLASH_Presetting);//����		//Erase current sector  ������	
											Write_EEPROM(Drv_Direction,FLASH_Presetting_DRV,DRV_Compare_TurnMeter());
											Write_EEPROM(ST_Direction,FLASH_Presetting_ST,ST_Compare_TurnMeter());	
											FLASH_Lock();																			//����
				
											drv_spta.steps_taken = 0;     //�����еĲ���
											st_spta.steps_taken = 0; 			
											Delay_ms(100);			


											break;				

				case reset   :/* ��λ->������ȼ�ָ�� */

											USART1->CR1 &= ~(0X1<<13);//USART1ʧ��;
				
											/*DRV ����λ�������� ��λֵֻΪ����*/
											dev_step = DRV_Compare_TurnMeter();
											DRV_Drive(~dev_step+1);
				
											/*ST_DirectionΪ��0��ʱ,������,��ֵ������������Ϊ������*/
											st_step = ST_Compare_TurnMeter();
											(ST_Direction == st_right) ? (ST820_Drive(st_step)) : (ST820_Drive(-st_step));			

											drv_spta.TurnMeter = 0;	
											st_spta.TurnMeter  = 0;
											
											USART1->CR1 |= 0X1<<13;//USART1ʹ��;	
							//				printf("dev_step %d",(~dev_step+1));
	
											Delay_ms(100);
											break;												
				default: 			
											break;

			}
			reverse = STOP;//Ĭ��ֹͣ		
#endif		
	
			
	}
}



