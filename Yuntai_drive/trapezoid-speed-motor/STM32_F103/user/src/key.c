#include "key.h"



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
