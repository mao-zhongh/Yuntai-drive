#include "stepmotor.h"

/*
******************************************************
函数功能：步进电机初始化
函数形参：None
函数返回值：None
备注：PA4~PA7    B   --  A         APB2_72Mhz       TIM4_APB1_36*2
作者：HaI
日期：2020年09月23日
版本：v0.0
******************************************************
*/

//void StepMotor_Lib_Init(void)
//{
//	RCC->APB2ENR |= 0X1<<2;//开A口时钟;
//	
//	GPIOA ->CRL &= ~(0xff<<16);
//	GPIOA ->CRL |= 0x1<<16;//PA4推挽输出10MHz
//	GPIOA ->CRL |= 0x1<<20;//PA5推挽输出10MHz
//	
//	GPIOA ->CRL &= ~(0xff<<24);		
//	GPIOA ->CRL |= 0x1<<24;//PA6推挽输出10MHz
//	GPIOA ->CRL |= 0x1<<28;//PA7推挽输出10MHz
//	
//	GPIOA ->ODR &= ~(0xf<<4);//操作ODR寄存器默认低电平
//	
//	
//	
//	RCC->APB1ENR |= 0x1<<2;//开TIM4时钟;
//	
//	TIM4 ->CR1 =0;
//	TIM4 ->CR1 |=0x1<<2; 
//	/*
//	连续计数
//	没有影子
//	计数器溢出/下溢才产生更新中断
//	*/
//	
//	TIM4 ->CNT =0;
//	TIM4 ->PSC =72-1;//1us计数一次
//	TIM4 ->ARR =800;//规定不能低于800us
//	
//	TIM4 ->DIER |=0x1<<0;//允许更新中断
//	
//	NVIC_SetPriority(TIM4_IRQn, NVIC_EncodePriority (7-2, 1, 1));//设置TIM4的中断优先级
//	NVIC_EnableIRQ(TIM4_IRQn);//响应定时器4的中断请求
//	
////	TIM4 ->CR1 |= 0x1<<0;//使能计数器;
//}



///*
//******************************************************
//函数功能：控制步进电机函数
//函数形参：转向  
//函数返回值：None
//备注：None
//作者：HaI
//日期：2020年09月23日
//版本：v0.0
//******************************************************
//*/
//void Control_StepMotor(u8 dir)
//{
//	static u8 cnt = 0;

//	cnt++;
//	if(cnt >= 5)  //1:  1 2 3 4     2 3 4    2 3 4      2 3 4   
//	{
//		cnt = 0;    //选0正确:   /  之前cnt=1一直卡扽  1 2 3 4    1 2 3 4   1 2 3 4
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

u8 dir_mode;//决定移动方向


///*TIM4中断服务函数*/
//void TIM4_IRQHandler(void)
//{
//	static u8 cnt = 0;
//	
//	TIM4 ->SR &= ~(0x1<<0);//清标志
//	
//	Control_StepMotor(dir_mode);
//	
//	cnt++;
//	if(cnt >= 200)//200ms
//	{
//		cnt = 0;
//		TIM4->CR1 &= ~(0x1<<0);//关闭计数器
//		GPIOA->ODR &= ~(0xf << 4);//电机四个管脚都输出低电平
//	}
//	
//}


/***********************************------------------------------------------*******************************/
