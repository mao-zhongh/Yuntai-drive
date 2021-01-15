#include "st820.h"

u32 ST_TurnMeter_1 = 0,ST_TurnMeter_0 = 0;
u32 ST_lnterruptnum = 0;
u8 ST_Direction=0;		//����

/*
******************************************************
�������ܣ�ST�ܽų�ʼ��  
�����βΣ�None
��������ֵ��None
��ע��APB2---C10 C11 C12    B5 B6
���ߣ�hai 
���ڣ�
�汾��v0.0
******************************************************
*/
void ST820_Init(void)
{
	RCC->APB2ENR |= 0x7<<2;// A B C ��ʹ��
	
	GPIOA->CRL &= ~(0XF<<12);// PA3 PA5 PA7ͨ��������� 10MHZ
	GPIOA->CRL &= ~(0XF<<20);
	GPIOA->CRL |= 0X1<<12;
	GPIOA->CRL |= 0X1<<20;
	GPIOA->CRL &= ~(0XF<<28);
	GPIOA->CRL |= 0X1<<28;
	
	GPIOB->CRL &= ~(0xf<<4);// PB1 PB11 ͨ��������� 10MHZ
	GPIOB->CRH &= ~(0xf<<12);
	GPIOB->CRL |= 0X1<<4;
	GPIOB->CRH |= 0X1<<12;
	
	GPIOC->CRL &= ~(0XF<<20);//PC5 			ͨ��������� 10MHZ
	GPIOC->CRL |= 0X1<<20;
	
	
}



/*
****************************************
�������ܣ�ST820 �������ٲ������30BYJ-26��������
����������( Direction(�ƶ�����)��0��˳ʱ��    1����ʱ��  , �ƶ��Ľ������� )
��������ֵ��none   
��ע��		1/500 = 0.002s = 2ms    360��/7.5�� * 85step == 4080        16320 = 1/4
���ߣ�Haiiii
���ڣ�20.11.24
�汾��v0.0
****************************************
*/
unsigned char ST820_motor(unsigned char ST_Direc,unsigned int num)
{
	motor_disturb=1;
	ST_ENABLE = 1;    			//ʹ������
	ST_DIR = ST_Direc&0X01;

	TIM4->CR1 |= (0x1<<0);//��ʼ������ 	
 
	while(ST_lnterruptnum <= num)
	{		
			
			if( ST_TurnMeter_1 - ST_TurnMeter_0 == 0 || ST_TurnMeter_0 - ST_TurnMeter_1 == 0)
			{
				ST_TurnMeter_1 = 0; 
				ST_TurnMeter_0 = 0;				
			}
			if( Continue_disturb )    //��ͻȻ����ָ�����˳��������
			{
				TIM4->CR1 &= ~(0x1<<0);//ֹͣ������ 				
				ST_lnterruptnum = 0;
				Continue_disturb = 0;
				motor_disturb=0;
				ST_ENABLE = 0;    			//ʧ������
 
				//Delay2ms();
				return 1;
			}
			if(ST_Compare_TurnMeter() >= 65280 && ST_Direction != (ST_Direc&0X01) )//90
			{
				goto ST_out;
			}

		
	}
ST_out:	
	
	TIM4->CR1 &= ~(0x1<<0);//ֹͣ������ 
  motor_disturb=0;

	ST_lnterruptnum = 0;
	ST_ENABLE = 0;    			//ʧ������
	Delay_ms(10);		
	return 0;
}


/*ST���ص�ǰλ��ֵ*/
unsigned int ST_Compare_TurnMeter(void)
{

			if( ST_TurnMeter_1 > ST_TurnMeter_0 )//
			{
				ST_Direction=0;		//����
				return (ST_TurnMeter_1 - ST_TurnMeter_0);
			}
			else
			{
				ST_Direction=1;		//����
				return (ST_TurnMeter_0 - ST_TurnMeter_1);
			}	
}