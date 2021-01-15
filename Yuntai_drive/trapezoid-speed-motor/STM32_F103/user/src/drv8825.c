#include "drv8825.h"

u32 Drv_lnterruptnum =0;
u32 TurnMeter_1=0,TurnMeter_0=0;
u8 Drv_Direction=0;
/*
******************************************************
�������ܣ�drv8825�ܽų�ʼ��   PB9_CH4
�����βΣ�None
��������ֵ��None
��ע��APB2---C10 C11 C12    B5 B6
���ߣ�hai 
���ڣ�
�汾��v0.0
******************************************************
*/

void DRV8825_Init(void)
{
	RCC->APB2ENR |= 0x7<<2;// A B C ��ʹ��

	GPIOA->CRH &= ~(0XF<<16); // PA12 ͨ��������� 10MHZ
	GPIOA->CRH |= 0X1<<16;	
	
	
	GPIOB->CRL &= ~(0xf<<28);//PB3 5 7 ͨ��������� 10MHZ
	GPIOB->CRL |= 0x01<<28;		
	GPIOB->CRL &= ~(0xf<<20);
  GPIOB->CRL |= 0x01<<20;		
	GPIOB->CRL &= ~(0xf<<12);
  GPIOB->CRL |= 0x01<<12;	


	GPIOC->CRH &= ~(0XF<<8);// pc10 pc12  ͨ��������� 10MHZ
	GPIOC->CRH |= 0x1<<8;
	GPIOC->CRH &= ~(0XF<<16);
	GPIOC->CRH |= 0x1<<16;
}



/*���ص�ǰλ��ֵ*/
unsigned int Compare_TurnMeter(void)
{
	
			if( TurnMeter_1 > TurnMeter_0 )//
			{
				Drv_Direction=0;		//����
				return (TurnMeter_1 - TurnMeter_0);
			}
			else
			{
				Drv_Direction=1;		//����
				return (TurnMeter_0 - TurnMeter_1);
			}	
		

}
