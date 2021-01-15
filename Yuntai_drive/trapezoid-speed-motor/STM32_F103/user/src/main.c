#include "main.h"



void Delay_ms(u32 num) //ms延时
{
	//选择时钟源；
	SysTick ->CTRL &= ~( 0x1<<2 );
	
	//对重装载值进行写值；
	SysTick ->LOAD =9 * 1000 * num;
	
	//对递减计数器执行写操作；
	SysTick ->VAL =0;

	//使能计算器工作（使能系统滴嗒定时器）;
	SysTick ->CTRL |= 0x1<<0;
	
	//等待定时时间到，标志位置一
	while(!(SysTick->CTRL &(1 <<16)));
	
	//关闭定时器
	SysTick->CTRL &= ~(1 << 0);       
	
}

void Delay_us(u32 num) //ms延时
{
	//选择时钟源；
	SysTick ->CTRL &= ~( 0x1<<2 );//21M  计一次时间 = 1 / 21 us
	
	//对重装载值进行写值；
	SysTick ->LOAD =9  * num;
	
	//对递减计数器执行写操作；
	SysTick ->VAL =0;

	//使能计算器工作（使能系统滴嗒定时器）;
	SysTick ->CTRL |= 0x1<<0;
	
	//等待定时时间到，标志位置一
	while(!(SysTick->CTRL &(1 <<16)));
	
	//关闭定时器
	SysTick->CTRL &= ~(1 << 0);       
	
}


int main(void)
{

	LED_Iint();
	Usart1__Init(115200);
	DRV8825_Init();
	TIM4_Tint_(72,20);//周期：1us*60=60us

 // [2041 1/2]    [16320 1/4]    [65280 1/16]   [130560 1/32]   [522240 1/128_11]  [1044480 1/256]运行节拍数
/*       现取32/1         */
	MODE0=1;//30    [130560_qua]
	MODE1=1;
	MODE2=1;
//	LED_1  =0;	

	DRV_Motor(-65280,1,1,200);//  max-->越大,周期越短
	Delay_ms(300);	Delay_ms(300);	Delay_ms(300);	Delay_ms(300);
	DRV_Motor(65280,20,20,200);//  max-->越大,周期越短
	while(1)
	{	
//		DRV_Motor(1,65280);
//		arr_tim4 = 60;
//		DRV_Motor(0,65280);
//		Delay_us(50);
	
//		if(ok==1)
//		{ok=0;
//			TIM4->CR1 &= ~(0x1<<0);//失能计数器;
//			ENABLE = 1;								//失能步进电机
//			Delay_ms(400);Delay_ms(400);Delay_ms(400);Delay_ms(400);Delay_ms(400);Delay_ms(400);Delay_ms(400);Delay_ms(400);
//			TIM4->CR1 |= (0x1<<0);//使能计数器;
//			ENABLE = 0;								//失能步进电机			
//		}
	}
}




/*
	Delay_ms(500);	Delay_ms(1000);	Delay_ms(1000);
		DIR = 0;
			Delay_ms(500);	Delay_ms(1000);	Delay_ms(1000);
		DIR = 1;


    if(strcmp((char*)Read_buff,"LED_ON")== 0)
		{
         	LED_1=0;
			memset(Read_buff,'\0',sizeof(Read_buff));
		}
		else if (strcmp((char*)Read_buff,"LED_OFF")==0)
		{
			LED_1=1;
			memset(Read_buff,'\0',sizeof(Read_buff));
		}

*/