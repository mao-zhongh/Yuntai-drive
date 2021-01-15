#include "key.h"



/*
******************************************************
函数功能：Key管脚初始化
函数形参：None
函数返回值：None
备注：KEY1 - PA0      KEY2 - PA8   默认高电平
								APB2

作者：HaI
日期：2020年09月24日
版本：v0.0
******************************************************
*/
void Key_Lib_Pin_Init(void)
{
	RCC->APB2ENR |= 0x1<<2;//开A时钟;
	
	GPIOA ->CRL &= ~(0xf<<0);//PA0
	GPIOA ->CRL |= 0x4<<0;//浮空输入;
	GPIOA ->CRH &= ~(0xf<<0);//PA8
	GPIOA ->CRH |= 0x4<<0;//浮空输入;
	
	AFIO->EXTICR[0] &= ~(0XF<<0);//将PA0复用到EXTI0上;
	AFIO->EXTICR[2] &= ~(0XF<<0);//将PA8复用到EXTI8上;
	
	
	EXTI->IMR |= 0x1<<0;//开发来自0 / 8 线上的中断请求
	EXTI->IMR |= 0x1<<8;
	
//	EXTI->EMR &= ~(0x1<<0);//关0 / 8 线上事件屏蔽器
//	EXTI->EMR &= ~(0x1<<8);
//	EXTI->SWIER &= ~(0x101<<0);//关闭软件中断
	EXTI->EMR = 0;		 //关闭事件功能
	EXTI->SWIER = 0;	 //关闭软件中断
	
	EXTI->FTSR |= 0x1<<0;//下降沿触发
	EXTI->FTSR |= 0x1<<8;

	EXTI->IMR |= 0x1<<0;//开发来自0 / 8 线上的中断请求
	EXTI->IMR |= 0x1<<8;
	
//设置EXTI0的中断优先级
	NVIC_SetPriority( EXTI0_IRQn, NVIC_EncodePriority( 7-2,2,2 ) );
	NVIC_SetPriority( EXTI9_5_IRQn, NVIC_EncodePriority( 7-2,2,3 ) );	
//使能NVIC响应EXTI0的中断请求 	
	NVIC_EnableIRQ ( EXTI0_IRQn );
	NVIC_EnableIRQ ( EXTI9_5_IRQn );	
}


/*
******************************************************
函数功能：Key中断扫描函数
函数形参：None
函数返回值：0：无按键按下   其它：按键按下
备注：KEY1 - PA0   PA8
							APB2
      按键按下前高电平  按键按下按下后低电平   
作者：HaI
日期：2020年09月24日
版本：v0.0
******************************************************
*/

//void EXTI0_IRQHandler(void)
//{
//	if(EXTI->PR & 0x1<<0)//按键PA0进来
//	{
//		dir_mode=1;
//		EXTI->PR |= 0x1<<0;		
////		LED_ON ;
//		TIM4->CR1 |= (0x1<<0);//使能计数器
//	}
////	Delay_ms(50);
//}
//void EXTI9_5_IRQHandler(void)
//{
//	if(EXTI->PR & 0x1<<8)//按键PA8进来
//	{
//		dir_mode=0;
//		EXTI->PR |= 0x1<<8;		
//	//	LED_OFF ;
//		TIM4->CR1 |= (0x1<<0);//使能计数器
//	}
////	Delay_ms(50);
//}



/*
******************************************************
函数功能：LED管脚初始化
函数形参：None
函数返回值：None
备注：
     推挽输出
	    APB2  PD2 -LED1     PA8-LED1 
作者：HAI
日期：2020年09月24日
版本：v0.0
******************************************************
*/
void LED_Iint(void)
{
	RCC->APB2ENR |= 0x1<<5;//D 口时钟开启;
	RCC->APB2ENR |= 0X1<<2;//A 口时钟开启
	
	GPIOD ->CRL &= ~(0xf<<8);//PD2推挽输出2MHZ
	GPIOD ->CRL |= 0x2<<8;

	GPIOA ->CRH &= ~(0xf<<0);//PA8推挽输出2MHZ
	GPIOA ->CRH |= 0x2<<0;	

	//默认高电平
	
//	GPIOA->ODR &= ~(0X1<<8);
	
//	GPIOD->ODR &= ~(0X1<<2);
	GPIOA->ODR |= 0X1<<8;
	GPIOD->ODR |= (0X1<<2);


}
