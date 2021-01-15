#include "st820.h"

u32 ST_TurnMeter_1 = 0,ST_TurnMeter_0 = 0;
u32 ST_lnterruptnum = 0;
u8 ST_Direction=0;		//方向

/*
******************************************************
函数功能：ST管脚初始化  
函数形参：None
函数返回值：None
备注：APB2---C10 C11 C12    B5 B6
作者：hai 
日期：
版本：v0.0
******************************************************
*/
void ST820_Init(void)
{
	RCC->APB2ENR |= 0x7<<2;// A B C 口使能
	
	GPIOA->CRL &= ~(0XF<<12);// PA3 PA5 PA7通用推挽输出 10MHZ
	GPIOA->CRL &= ~(0XF<<20);
	GPIOA->CRL |= 0X1<<12;
	GPIOA->CRL |= 0X1<<20;
	GPIOA->CRL &= ~(0XF<<28);
	GPIOA->CRL |= 0X1<<28;
	
	GPIOB->CRL &= ~(0xf<<4);// PB1 PB11 通用推挽输出 10MHZ
	GPIOB->CRH &= ~(0xf<<12);
	GPIOB->CRL |= 0X1<<4;
	GPIOB->CRH |= 0X1<<12;
	
	GPIOC->CRL &= ~(0XF<<20);//PC5 			通用推挽输出 10MHZ
	GPIOC->CRL |= 0X1<<20;
	
	
}



/*
****************************************
函数功能：ST820 驱动减速步进电机30BYJ-26驱动函数
函数参数：( Direction(移动方向)：0：顺时针    1：逆时针  , 移动的节拍数量 )
函数返回值：none   
备注：		1/500 = 0.002s = 2ms    360°/7.5° * 85step == 4080        16320 = 1/4
作者：Haiiii
日期：20.11.24
版本：v0.0
****************************************
*/
unsigned char ST820_motor(unsigned char ST_Direc,unsigned int num)
{
	motor_disturb=1;
	ST_ENABLE = 1;    			//使能输入
	ST_DIR = ST_Direc&0X01;

	TIM4->CR1 |= (0x1<<0);//开始计数器 	
 
	while(ST_lnterruptnum <= num)
	{		
			
			if( ST_TurnMeter_1 - ST_TurnMeter_0 == 0 || ST_TurnMeter_0 - ST_TurnMeter_1 == 0)
			{
				ST_TurnMeter_1 = 0; 
				ST_TurnMeter_0 = 0;				
			}
			if( Continue_disturb )    //如突然有新指令则退出电机运行
			{
				TIM4->CR1 &= ~(0x1<<0);//停止计数器 				
				ST_lnterruptnum = 0;
				Continue_disturb = 0;
				motor_disturb=0;
				ST_ENABLE = 0;    			//失能输入
 
				//Delay2ms();
				return 1;
			}
			if(ST_Compare_TurnMeter() >= 65280 && ST_Direction != (ST_Direc&0X01) )//90
			{
				goto ST_out;
			}

		
	}
ST_out:	
	
	TIM4->CR1 &= ~(0x1<<0);//停止计数器 
  motor_disturb=0;

	ST_lnterruptnum = 0;
	ST_ENABLE = 0;    			//失能输入
	Delay_ms(10);		
	return 0;
}


/*ST返回当前位置值*/
unsigned int ST_Compare_TurnMeter(void)
{

			if( ST_TurnMeter_1 > ST_TurnMeter_0 )//
			{
				ST_Direction=0;		//方向
				return (ST_TurnMeter_1 - ST_TurnMeter_0);
			}
			else
			{
				ST_Direction=1;		//方向
				return (ST_TurnMeter_0 - ST_TurnMeter_1);
			}	
}