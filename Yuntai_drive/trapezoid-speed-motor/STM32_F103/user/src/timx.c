#include "timx.h"

//系统加减速参数
speedRampData srd;
//记录步进电机的位置
int stepPosition = 0;
//系统电机、串口状态
struct GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};
/*
******************************************************
函数功能：输出PWM——TIM4初始化
函数形参：None
函数返回值：None
备注：APB1---TIM4    72Mhz     
			PB8_CH3  PB9_CH4                  5us
作者：hai 
日期：
版本：v0.0
******************************************************
*/
void TIM4_Tint_(u32 psc,u32 arr) //
{
	RCC->APB2ENR |= 0x1<<3;//开B口时钟;
	RCC->APB1ENR |= 0X1<<2;//复位TIM4定时器:

	GPIOB->CRH &= ~(0Xf<<0);
	GPIOB->CRH |= 0XA<<0;//PB8推挽输出
	
//	TIM4->SMCR &= ~(0X7<<0);//内部时钟驱动
//	TIM4->SMCR &= ~(0x1<<14);
	
	TIM4->CR1 =0;//配置CR1寄存器;
	TIM4->CR1 &=~(0X01<<3);//不停止计数

	/*
	TIMx_ARR寄存器没有缓冲；
	循环计数  SYSTICK*8     TIM*2
	边沿对齐模式
	计数器向上计数
	*/
	TIM4->PSC =psc-1;//预分配值72000000/72=1000Khz
	TIM4->ARR =arr;//自动重装载1us计一次，周期：1*1000=1ms
	TIM4->CNT=0;
	
	/*CH3*/
	TIM4->CCR3 = arr>>1;//比较值  PB8_CH3
	TIM4->CCMR2 &= ~(0x3<<0);//CC3通道被配置为输出
	TIM4->CCMR2 &= ~(0X1<<7);//不受ETRF输入的影响;
	TIM4->CCMR2 &= ~(0x7<<4);
	TIM4->CCMR2 |= (0x06<<4);//PWM模式1;  110 
	
	
	TIM4->CCER &= ~(0x1<<9);//高有效;
	TIM4->CCER |= 0x1<<8;	//开启CH3信号输出	
		
		
	//设置TIM4中断优先级；
	NVIC_SetPriority( TIM4_IRQn, 3 );
	NVIC_EnableIRQ ( TIM4_IRQn );//使能NVIC响应	
	TIM4->DIER |= 0x1<<0;//允许更新中断


	TIM4->CR1 &= ~(0x1<<0);//失能计数器;	
//	TIM4->CR1 |= (0x1<<0);//使能计数器;	

}
/**

  * @brief  根据运动方向判断步进电机的运行位置

  * @param  inc 运动方向

  * @retval 无

  */
void MSD_StepCounter(signed char inc)
{
  //根据方向判断电机位置
  if(inc == CCW)
  {
    stepPosition--;
  }
  else
  {
    stepPosition++;
  }
}
/*****TIM4中断服务函数*****/
void TIM4_IRQHandler(void)
{
	LED_1 =0;
  // 保存下一个延时周期
  unsigned int new_step_delay;
  // 加速过程中最后一次延时.
  static int last_accel_delay;
  // 移动步数计数器
  static unsigned int step_count = 0;
  // 记录new_step_delay中的余数，提高下一步计算的精度
  static signed int rest = 0;

if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
{
    /* Clear MSD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    TIM4->CCR4=srd.step_delay >> 1;//周期的一半   //10830>>1
    TIM4->ARR=srd.step_delay;
    //如果禁止输出，电机则停止运动
    if(status.out_ena != TRUE)
    {
        srd.run_state = STOP;
    }
  switch(srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      TIM4->CCER &= ~(0x1<<8); //禁止输出
      TIM_Cmd(TIM4, DISABLE);
      status.running = FALSE;
			srd.over = 1;
      break;

    case ACCEL:
      TIM4->CCER |= 1<<8; //使能输出
      MSD_StepCounter(srd.dir);//记录运动方向及位置
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));         //4332.022
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //检查是够应该开始减速
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        srd.run_state = DECEL;
      }
      //检查是否到达期望的最大速度
      else if(new_step_delay <= srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
      TIM4->CCER |= 1<<8; //使能输出
      MSD_StepCounter(srd.dir);
      step_count++;
      new_step_delay = srd.min_delay;
      //检查是否需要开始减速
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        //以最后一次加速的延时作为开始减速的延时
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      TIM4->CCER |= 1<<8; //使能输出
      MSD_StepCounter(srd.dir);
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //检查是否为最后一步
      if(srd.accel_count >= 0){
        srd.run_state = STOP; 
      }
      break;
  }
  srd.step_delay = new_step_delay;
  }
}
 /*! \brief 以给定的步数移动步进电机
 *  通过计算加速到最大速度，以给定的步数开始减速
 *  如果加速度和减速度很小，步进电机会移动很慢，还没达到最大速度就要开始减速
 *  \param step   移动的步数 (正数为顺时针，负数为逆时针).
 *  \param accel  加速度,如果取值为100，实际值为100*0.01*rad/sec^2=1rad/sec^2
 *  \param decel  减速度,如果取值为100，实际值为100*0.01*rad/sec^2=1rad/sec^2
 *  \param speed  最大速度,如果取值为100，实际值为100*0.01*rad/sec=1rad/sec
 */
void MSD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{//                     16320                 75                  75                    62
    //达到最大速度时的步数.
    unsigned int max_s_lim;
    //必须开始减速的步数(如果还没加速到达最大速度时)。
    unsigned int accel_lim;


    // 配置电机为输出状态
    //status.out_ena = TRUE;
    
    // 如果只移动一步
    if(step == 1)
    {
        // 只移动一步
        srd.accel_count = -1;
        // 减速状态
        srd.run_state = DECEL;
        // 短延时
        srd.step_delay = 1000;
        // 配置电机为运行状态
        status.running = TRUE;
        //设置定时器重装值	
        TIM_SetAutoreload(TIM4 ,Pulse_width);
        //设置占空比为50%	
        TIM_SetCompare2(TIM4,Pulse_width>>1);
        //使能定时器	      
        TIM_Cmd(TIM4, ENABLE); 
     }
    // 步数不为零才移动
    else if(step != 0)
    {
    // 我们的驱动器用户手册有详细的计算及推导过程

    // 设置最大速度极限, 计算得到min_delay用于定时器的计数器的值。
    // min_delay = (alpha / tt)/ w
    srd.min_delay = A_T_x100 / speed;    //155.2258

    // 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.01rad/sec^2
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd.step_delay = (T1_FREQ_148 * sqrt(A_SQ / accel))/100;                      //10830.0567

    // 计算多少步之后达到最大速度的限制
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);    //2662.529
    // 如果达到最大速度小于0.5步，我们将四舍五入为0
    // 但实际我们必须移动至少一步才能达到想要的速度
    if(max_s_lim == 0)
    {
        max_s_lim = 1;
    }
 
    // 计算多少步之后我们必须开始减速
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);  //8160
    // 我们必须加速至少1步才能才能开始减速.
    if(accel_lim == 0)
    {
        accel_lim = 1;
    }
    // 使用限制条件我们可以计算出第一次开始减速的位置
    //srd.decel_val为负数
    if(accel_lim <= max_s_lim)
    {
        srd.decel_val = accel_lim - step;
    }
    else
    {
        srd.decel_val = -(long)(max_s_lim*accel/decel);   //2662.529
    }
    // 当只剩下一步我们必须减速
    if(srd.decel_val == 0)
    {
        srd.decel_val = -1;
    }

    // 计算开始减速时的步数
    srd.decel_start = step + srd.decel_val;               //18982.529

    // 如果最大速度很慢，我们就不需要进行加速运动
    if(srd.step_delay <= srd.min_delay)
    {
        srd.step_delay = srd.min_delay;
        srd.run_state = RUN;
    }
    else
		{
        srd.run_state = ACCEL;
    }

    // 复位加速度计数值
    srd.accel_count = 0;
    status.running = TRUE;
    //设置定时器重装值	
    TIM_SetAutoreload(TIM4,Pulse_width);
    //设置占空比为50%	
    TIM_SetCompare2(TIM4,Pulse_width>>1);
    //使能定时器	      
    TIM_Cmd(TIM4, ENABLE); 
    }
}






unsigned char DRV_Motor(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
	ENABLE =0;

	// 根据步数的正负来判断方向
	if(step < 0)//逆时针
	{
			srd.dir = CCW;
			step = -step;
	}
	else//顺时针
	{
			srd.dir = CW;
	}	
	DIR = srd.dir;
	/***********/
 
	MSD_Move(step,accel,decel,speed);
	
	
	/***********/

	TIM4->CR1 |= (0x1<<0);//开始计数器 	
		while(!srd.over)//400
		{




		}
		
	
DRV_out:
 	TIM4->CR1 &= ~(0x1<<0);//停止计数器 
	ENABLE = 1;								//失能步进电机
	srd.over=0;

	Delay_ms(10);	

		return 0;
}


/*
		TIM4->CCR3 = arr_tim4;//比较值  PB8_CH3
		if(flag == 0)
		{
			i += 1;
			if(i>=3000)
			{
				i=0;
				arr_tim4 +=1;  		
				if(arr_tim4 >= 60) { flag =1; LED_1 =1;	DIR =1; }//
			}
		}else if(flag==1)
		{
			i +=1;
			if(i >= 3000)
			{
				i=0;
				arr_tim4 -=1;  		
				if(arr_tim4 <= 10) { flag=0; LED_1 =0;	 DIR=0; }
			}
		}
*/