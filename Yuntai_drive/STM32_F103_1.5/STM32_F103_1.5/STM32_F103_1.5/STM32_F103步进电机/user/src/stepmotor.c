#include "stepmotor.h"

/*
******************************************************
�������ܣ����������ʼ��
�����βΣ�None
��������ֵ��None
��ע��PA4~PA7    B   --  A         APB2_72Mhz       TIM4_APB1_36*2
���ߣ�HaI
���ڣ�2020��09��23��
�汾��v0.0
******************************************************
*/

//void StepMotor_Lib_Init(void)
//{
//	RCC->APB2ENR |= 0X1<<2;//��A��ʱ��;
//	
//	GPIOA ->CRL &= ~(0xff<<16);
//	GPIOA ->CRL |= 0x1<<16;//PA4�������10MHz
//	GPIOA ->CRL |= 0x1<<20;//PA5�������10MHz
//	
//	GPIOA ->CRL &= ~(0xff<<24);		
//	GPIOA ->CRL |= 0x1<<24;//PA6�������10MHz
//	GPIOA ->CRL |= 0x1<<28;//PA7�������10MHz
//	
//	GPIOA ->ODR &= ~(0xf<<4);//����ODR�Ĵ���Ĭ�ϵ͵�ƽ
//	
//	
//	
//	RCC->APB1ENR |= 0x1<<2;//��TIM4ʱ��;
//	
//	TIM4 ->CR1 =0;
//	TIM4 ->CR1 |=0x1<<2; 
//	/*
//	��������
//	û��Ӱ��
//	���������/����Ų��������ж�
//	*/
//	
//	TIM4 ->CNT =0;
//	TIM4 ->PSC =72-1;//1us����һ��
//	TIM4 ->ARR =800;//�涨���ܵ���800us
//	
//	TIM4 ->DIER |=0x1<<0;//��������ж�
//	
//	NVIC_SetPriority(TIM4_IRQn, NVIC_EncodePriority (7-2, 1, 1));//����TIM4���ж����ȼ�
//	NVIC_EnableIRQ(TIM4_IRQn);//��Ӧ��ʱ��4���ж�����
//	
////	TIM4 ->CR1 |= 0x1<<0;//ʹ�ܼ�����;
//}



///*
//******************************************************
//�������ܣ����Ʋ����������
//�����βΣ�ת��  
//��������ֵ��None
//��ע��None
//���ߣ�HaI
//���ڣ�2020��09��23��
//�汾��v0.0
//******************************************************
//*/
//void Control_StepMotor(u8 dir)
//{
//	static u8 cnt = 0;

//	cnt++;
//	if(cnt >= 5)  //1:  1 2 3 4     2 3 4    2 3 4      2 3 4   
//	{
//		cnt = 0;    //ѡ0��ȷ:   /  ֮ǰcnt=1һֱ���Y  1 2 3 4    1 2 3 4   1 2 3 4
//	}
//	
//	if(dir == DIR_LEFT)
//	{
//		switch(cnt)
//		{
//			case 1: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0x9 << 4);
//					break;
//			case 2: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0x6 << 4);
//					break;
//			case 3: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0x5 << 4);
//					break;
//			case 4: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0xa << 4);
//					break;
//		}
//	}else if(dir == DIR_RIGHT)
//	{
//		switch(cnt)
//		{
//			case 4: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0x9 << 4);
//					break;
//			case 3: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0x6 << 4);
//					break;
//			case 2: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0x5 << 4);
//					break;
//			case 1: GPIOA->ODR &= ~(0xf << 4);
//					GPIOA->ODR |= (0xa << 4);
//					break;
//		}
//	}
//	
//	
//}

u8 dir_mode;//�����ƶ�����


///*TIM4�жϷ�����*/
//void TIM4_IRQHandler(void)
//{
//	static u8 cnt = 0;
//	
//	TIM4 ->SR &= ~(0x1<<0);//���־
//	
//	Control_StepMotor(dir_mode);
//	
//	cnt++;
//	if(cnt >= 200)//200ms
//	{
//		cnt = 0;
//		TIM4->CR1 &= ~(0x1<<0);//�رռ�����
//		GPIOA->ODR &= ~(0xf << 4);//����ĸ��ܽŶ�����͵�ƽ
//	}
//	
//}


/***********************************------------------------------------------*******************************/
