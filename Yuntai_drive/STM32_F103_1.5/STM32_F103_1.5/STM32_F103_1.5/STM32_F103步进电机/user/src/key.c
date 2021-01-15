#include "key.h"



/*
******************************************************
�������ܣ�Key�ܽų�ʼ��
�����βΣ�None
��������ֵ��None
��ע��KEY1 - PA0      KEY2 - PA8   Ĭ�ϸߵ�ƽ
								APB2

���ߣ�HaI
���ڣ�2020��09��24��
�汾��v0.0
******************************************************
*/
void Key_Lib_Pin_Init(void)
{
	RCC->APB2ENR |= 0x1<<2;//��Aʱ��;
	
	GPIOA ->CRL &= ~(0xf<<0);//PA0
	GPIOA ->CRL |= 0x4<<0;//��������;
	GPIOA ->CRH &= ~(0xf<<0);//PA8
	GPIOA ->CRH |= 0x4<<0;//��������;
	
	AFIO->EXTICR[0] &= ~(0XF<<0);//��PA0���õ�EXTI0��;
	AFIO->EXTICR[2] &= ~(0XF<<0);//��PA8���õ�EXTI8��;
	
	
	EXTI->IMR |= 0x1<<0;//��������0 / 8 ���ϵ��ж�����
	EXTI->IMR |= 0x1<<8;
	
//	EXTI->EMR &= ~(0x1<<0);//��0 / 8 �����¼�������
//	EXTI->EMR &= ~(0x1<<8);
//	EXTI->SWIER &= ~(0x101<<0);//�ر�����ж�
	EXTI->EMR = 0;		 //�ر��¼�����
	EXTI->SWIER = 0;	 //�ر�����ж�
	
	EXTI->FTSR |= 0x1<<0;//�½��ش���
	EXTI->FTSR |= 0x1<<8;

	EXTI->IMR |= 0x1<<0;//��������0 / 8 ���ϵ��ж�����
	EXTI->IMR |= 0x1<<8;
	
//����EXTI0���ж����ȼ�
	NVIC_SetPriority( EXTI0_IRQn, NVIC_EncodePriority( 7-2,2,2 ) );
	NVIC_SetPriority( EXTI9_5_IRQn, NVIC_EncodePriority( 7-2,2,3 ) );	
//ʹ��NVIC��ӦEXTI0���ж����� 	
	NVIC_EnableIRQ ( EXTI0_IRQn );
	NVIC_EnableIRQ ( EXTI9_5_IRQn );	
}


/*
******************************************************
�������ܣ�Key�ж�ɨ�躯��
�����βΣ�None
��������ֵ��0���ް�������   ��������������
��ע��KEY1 - PA0   PA8
							APB2
      ��������ǰ�ߵ�ƽ  �������°��º�͵�ƽ   
���ߣ�HaI
���ڣ�2020��09��24��
�汾��v0.0
******************************************************
*/

//void EXTI0_IRQHandler(void)
//{
//	if(EXTI->PR & 0x1<<0)//����PA0����
//	{
//		dir_mode=1;
//		EXTI->PR |= 0x1<<0;		
////		LED_ON ;
//		TIM4->CR1 |= (0x1<<0);//ʹ�ܼ�����
//	}
////	Delay_ms(50);
//}
//void EXTI9_5_IRQHandler(void)
//{
//	if(EXTI->PR & 0x1<<8)//����PA8����
//	{
//		dir_mode=0;
//		EXTI->PR |= 0x1<<8;		
//	//	LED_OFF ;
//		TIM4->CR1 |= (0x1<<0);//ʹ�ܼ�����
//	}
////	Delay_ms(50);
//}



/*
******************************************************
�������ܣ�LED�ܽų�ʼ��
�����βΣ�None
��������ֵ��None
��ע��
     �������
	    APB2  PD2 -LED1     PA8-LED1 
���ߣ�HAI
���ڣ�2020��09��24��
�汾��v0.0
******************************************************
*/
void LED_Iint(void)
{
	RCC->APB2ENR |= 0x1<<5;//D ��ʱ�ӿ���;
	RCC->APB2ENR |= 0X1<<2;//A ��ʱ�ӿ���
	
	GPIOD ->CRL &= ~(0xf<<8);//PD2�������2MHZ
	GPIOD ->CRL |= 0x2<<8;

	GPIOA ->CRH &= ~(0xf<<0);//PA8�������2MHZ
	GPIOA ->CRH |= 0x2<<0;	

	//Ĭ�ϸߵ�ƽ
	
//	GPIOA->ODR &= ~(0X1<<8);
	
//	GPIOD->ODR &= ~(0X1<<2);
	GPIOA->ODR |= 0X1<<8;
	GPIOD->ODR |= (0X1<<2);


}
