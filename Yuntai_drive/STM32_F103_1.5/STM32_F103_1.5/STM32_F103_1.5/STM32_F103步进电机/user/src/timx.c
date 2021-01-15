#include "timx.h"

SPTAData_Typedef drv_spta = {0};
SPTAData_Typedef st_spta  = {0};
unsigned char Toggle_aisle =0;  /* 切换输出通道  1为:ST  0为:DRV */

uint32_t runtime = 0;


/*
******************************************************
函数功能：输出PWM——TIM4初始化
函数形参：_u16 dat3 通道3占空比设置  ,  _u16 dat4   通道4占空比设置
函数返回值：None
备注：APB1---TIM4    72Mhz     
			PB8_CH3  PB9_CH4                  5us
作者：hai 
版本：v0.0
******************************************************
*/
void TIM4_Tint_PWM(void) //
{
	RCC->APB2ENR |= 0x1<<3;//开B口时钟;
	RCC->APB1ENR |= 0X1<<2;//复位TIM4定时器:

	GPIOB->CRH &= ~(0Xf<<0);
	GPIOB->CRH |= 0XA<<0;//PB8推挽输出
	
	GPIOB->CRH &= ~(0Xf<<4);
	GPIOB->CRH |= 0XA<<4;//PB9复用推挽输出
	
	TIM4->SMCR &= ~(0x7<<0);//高电平/上升沿有效
	TIM4->SMCR &= ~(0X1<<14);//禁止外部时钟模式
	
	TIM4->CR1 =0;//配置CR1寄存器; 
	TIM4->CR1 |= 0X1<<2;//计数器溢出/下溢产生更新(UEV)事件
	TIM4->CR1 |= 0x1<<7;//TIMx_ARR寄存器被装入缓冲器。
//	TIM4->CR1 |= 0X2<<8;//给到CK_INT 72Mhz / 4 = 18Mhz
	/*
	循环计数    TIM*2
	边沿对齐模式
	计数器向上计数
	不停止计数  5.5384615Mhz
	*/
	
	TIM4->CNT=0;	
	if(Toggle_aisle)
	{
		TIM4->PSC =ST_PWM_PSC-1;//预分配值72000000/72=1000Khz   72000000/18=4000Khz0.25us  72000000/14.4=5000Khz 0.2us
		TIM4->ARR =ST_PWM_ARR;//自动重装载1us计一次，周期：1*1000=1ms   5Mhz 70us周期	
		/*CH4*/
		TIM4->CCR4 = 0;//比较值  PB9_CH4
		TIM4->CCMR2 &= ~(0x3<<8);//CC4通道被配置为输出
		TIM4->CCMR2 &= ~(0x1<<15);//不受ETRF输入的影响;
		TIM4->CCMR2 &= ~(0x7<<12);
		TIM4->CCMR2 |= (0x06<<12);//PWM模式1;一旦TIMx_CNT<TIMx_CCR4时通道4为有效电平
		TIM4->CCER &= ~(0x1<<13);//高有效;
		TIM4->CCER |= 0x1<<12;//开启CH4信号输出	
	}
	else 
	{
		TIM4->PSC =DRV_PWM_PSC-1;//预分配值72000000/72=1000Khz   72000000/18=4000Khz0.25us  72000000/14.4=5000Khz 0.2us
		TIM4->ARR =DRV_PWM_ARR;//自动重装载1us计一次，周期：1*1000=1ms   5Mhz 70us周期		
		/*CH3*/
		TIM4->CCR3 = 1;//比较值  PB8_CH3  --->有效占空比例
		TIM4->CCMR2 &= ~(0x3<<0);//CC3通道被配置为输出
		TIM4->CCMR2 &= ~(0x7<<4);//不受ETRF输入的影响;
		TIM4->CCMR2 |= (0x06<<4);//PWM模式1;  110 
		TIM4->CCER &= ~(0x1<<9);//高有效;
		TIM4->CCER |= 0x1<<8;	//开启CH3信号输出		
	}



	//设置TIM4中断优先级；
	NVIC_SetPriority( TIM4_IRQn, 3 );
	NVIC_EnableIRQ ( TIM4_IRQn );//使能NVIC响应	
	TIM4->DIER |= 0x1<<0;//允许更新中断

	TIM4->CR1 &= ~(0x1<<0);//失能计数器:		
	
}


/******因周期相同,每溢出一次则代表PWM走一个波形*******/
void TIM4_IRQHandler(void)
{
	static u32 DRV_i=0,ST_i=0;
	if(TIM4->SR & 0x1<<0)
	{
		TIM4->SR &= ~(0x1<<0);
		
		if(Toggle_aisle)  /* ST */
		{
			st_spta.steps_taken++;
			TIM4->CCR4 = st_spta.step_speed;//比较值  PB8_CH3		

			if     (ST_DIR_&0x01)    { 		  st_spta.TurnMeter++;     }						//记录顺时针每一节拍
			else						         {      st_spta.TurnMeter--;     }						//记录逆时针每一节拍		

			ST_i++;
//			if((st_spta.step_state == ACCELERATING) && (ST_i >= 50))    //加速时递减缩小更新时间
//			{				

//				ST_i=0;
//				if(st_spta.step_arr >156 ) { st_spta.step_arr -= 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //计数器溢出/下溢可产生更新事件		
//			}
//			else if((st_spta.step_state == DECELERATING) && (ST_i >= 50))//减速时递增大更新时间
//			{

//				ST_i=0;
//				if(st_spta.step_arr < 310) { st_spta.step_arr += 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //计数器溢出/下溢可产生更新事件		
//			}

			if((st_spta.step_state == ACCELERATING) && (st_spta.steps_taken <=st_spta.begin) && (ST_i >= 100))    //加速时递减缩小更新时间
			{				

				ST_i=0;
				if(st_spta.step_arr >100 ) { st_spta.step_arr -= 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //计数器溢出/下溢可产生更新事件		
			}
			else if((st_spta.step_state == DECELERATING) && ( st_spta.steps_taken >= st_spta.finish ) && (ST_i >= 35))//减速时递增大更新时间
			{

				ST_i=0;
				if(st_spta.step_arr < 200) { st_spta.step_arr += 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //计数器溢出/下溢可产生更新事件		
			}			
		}

		
	
		else  /* DRV */
		{
			drv_spta.steps_taken++;   //已运行的步数
			TIM4->CCR3 = drv_spta.step_speed;//比较值  PB8_CH3		
		
			if     (DRV_DIR_&0x01)    { 		 drv_spta.TurnMeter++;     }						//记录顺时针每一节拍
			else						      		{      drv_spta.TurnMeter--;     }						//记录逆时针每一节拍	

			DRV_i++;
			if((drv_spta.step_state == ACCELERATING) && (DRV_i >= 35))    //加速时递减缩小更新时间
			{				

				DRV_i=0;
				if(drv_spta.step_arr >720 ) { drv_spta.step_arr -= 1 ;	TIM4->ARR = drv_spta.step_arr;	 }                //计数器溢出/下溢可产生更新事件		
			}
			else if((drv_spta.step_state == DECELERATING) && (DRV_i >= 35))//减速时递增大更新时间
			{

				DRV_i=0;
				if(drv_spta.step_arr < 1000) { drv_spta.step_arr += 1 ;	TIM4->ARR = drv_spta.step_arr;	 }                //计数器溢出/下溢可产生更新事件		
			}
		}
	}
}	

/*
******************************************************
函数功能：根据电机的状态进行状态转换以及参数计算
函数形参： None 
函数返回值：None
备注： Toggle_aisle 来切换ST与DRV的初始化
作者：hai 
版本：v0.0
******************************************************
*/	
void Speed_Decision( void )
{
/****************************************************ST状态部分**************************************************************/
	if(Toggle_aisle)
	{
		switch (st_spta.step_state)
		{
			/* SPTA加速状态 */
			case ACCELERATING:
				
						if(st_spta.steps_all != 0)
						{
							/* 达到加速所需步数 */			
							if( st_spta.steps_taken >= st_spta.steps_add)   
							{ 
								TIM4->ARR = 100;
								/*预计到达完成匀速步数 = 已加速的步数 + 需均速的步数*/
								st_spta.steps_compare = st_spta.steps_taken + st_spta.steps_all;         
								/* 转为匀速状态 */					
								st_spta.step_state = UNIFORM;			
								break;
							}							
						}
						else//
						{
							/* 转为空闲状态 */						
							st_spta.step_state = IDLE;								
							/*减速状态 */
						//	st_spta.step_state = DECELERATING;						
						}
						
							/* 没到达最大速度限制 && 刚好过一个电平保持周期 */
//						if( (st_spta.step_speed <= st_spta.speed_lim) && ( (st_spta.steps_taken % st_spta.steps_interval) == 0) )							 				
						if( (st_spta.step_speed <= st_spta.step_arr) && ( (st_spta.steps_taken % st_spta.steps_interval) == 0) )	
						{
						 /* 加速阶段，速度累加溢出，则速度值递增 */
							st_spta.step_speed++ ;		
							Delay_ms(1);	
						}
						break;							

					
			/* SPTA匀速状态 */				
			case UNIFORM:	
							/* 当前已运行完步数  >= 已加速步数 + 需均速步数 */
						if( st_spta.steps_taken >= st_spta.steps_compare ) 
						{	
							st_spta.step_state = DECELERATING;
							
							//printf("steps_cut == %d\r\n",drv_spta.steps_cut);	
						}
					 break;						
			

			
			/* SPTA减速状态 */				
			case DECELERATING:
	//printf("steps_taken == %d\r\n",drv_spta.steps_taken);			

						if( st_spta.steps_taken >= st_spta.steps_required )/* 当前步数 >= 总步数 #达到减速所需步数*/
						{	
	//printf("总步数-当前步数 '0'  -> %d\r\n",(drv_spta.steps_required-drv_spta.steps_taken) );

							/* 转为空闲状态 */						
							st_spta.step_state = IDLE;					
							break;
						}		
						/* 没到最小速度 && 一个电平状态保持 drv_spta.steps_interval 个周期 */					
						else if( (	st_spta.step_speed >= 2 ) && ((st_spta.steps_taken % st_spta.steps_interval) == 0) ) 
						{
						 /* 减速阶段，速度累加溢出，则速度值递减 */
							st_spta.step_speed-- ;		

	//printf("step_speed == %d\r\n",drv_spta.step_speed);							
							Delay_ms(1);		
						}				
								
					 break;		
			
			
			/* SPTA空闲状态 */		
			case IDLE:
		
						TIM4->CR1 &= ~(0x1<<0);       //失能计数器;				
						ST_ENABLE = 0;								//失能步进电机
	//printf("\r\nST已空闲状态,该次行程已共走steps_taken -> %d\r\n",st_spta.steps_taken);
	//printf("已空闲状态,当前位置在    TurnMeter   -> %d\r\n",st_spta.TurnMeter);	
						/* 清零spta相关数据 */
						st_spta.steps_taken = 0;     //已运行的步数
						st_spta.step_speed = 0;   //当前加速度（占空比）
						st_spta.steps_add = 0;      	//需加速的步数  
						st_spta.steps_all = 0;       //需均速的步数
						st_spta.steps_cut = 0;       //需减速的步数	
						st_spta.speed_lim = 0;			  //最大加速度限制	
						st_spta.steps_interval = 0;  //间隔数
						st_spta.steps_required = 0;  //总步数
						st_spta.over =1;
						st_spta.steps_compare=0;
					//	Delay_ms(50);	

					 break;			

			default:           
							break;		
			
		}			
	}
/****************************************************DRV状态部分**************************************************************/
	else
	{
		switch (drv_spta.step_state)
		{
			/* SPTA加速状态 */
			case ACCELERATING:
				
						if(drv_spta.steps_add != 0)
						{
							/* 达到加速所需步数 */			
							if( drv_spta.steps_taken >= drv_spta.steps_add)   
							{ 
								TIM4->ARR = 720;
								/*预计到达完成匀速步数 = 已加速的步数 + 需均速的步数*/
								drv_spta.steps_compare = drv_spta.steps_taken + drv_spta.steps_all;    
//								printf("drv_spta.step_arr = %d\r\n",drv_spta.step_arr);									
								/* 转为匀速状态 */					
								drv_spta.step_state = UNIFORM;			
								break;
							}							
						}
						else//只运行一步 
						{
 
							/*减速状态 */
							drv_spta.step_state = DECELERATING;						
						}
						
						/*是否达到最大速度*/
	//					if(drv_spta.step_speed >= drv_spta.speed_lim)
	//					{
	//					//	drv_spta.step_speed = drv_spta.speed_lim;
	//						/*预计到达完成匀速步数 = 已加速的步数 + 需均速的步数*/
	//						drv_spta.steps_compare = drv_spta.steps_taken + drv_spta.steps_all;    						
	//						/* 转为匀速状态 */					
	//						drv_spta.step_state = UNIFORM;				
	//						break;
	//					}
							/* 没到达最大速度限制 && 刚好过一个电平保持周期 */
	//						if( (drv_spta.step_speed <= drv_spta.speed_lim) && ( (drv_spta.steps_taken % drv_spta.steps_interval) == 0) )		
					if( (drv_spta.step_speed <= drv_spta.step_arr) && ( (drv_spta.steps_taken % drv_spta.steps_interval) == 0) )									
						{
						 /* 加速阶段，速度累加溢出，则速度值递增 */
							drv_spta.step_speed++ ;		
							Delay_ms(1);	
						}
		

						break;						

						
			/* SPTA匀速状态 */				
			case UNIFORM:	


							/* 当前已运行完步数  >= 已加速步数 + 需均速步数 */
						if( drv_spta.steps_taken >= drv_spta.steps_compare ) 
						{	
							/* 需减速步数 = 总步数 - (已加速步数 + 已均速步数) */
							//drv_spta.steps_cut = drv_spta.steps_required - drv_spta.steps_taken;  //在次确认需减速步数
							/* 减速状态 */
	//printf("已均速完steps_taken == %d\r\n",drv_spta.steps_taken);
							drv_spta.step_state = DECELERATING;
							//printf("steps_cut == %d\r\n",drv_spta.steps_cut);	

//									printf("drv_spta.step_arr = %d\r\n",drv_spta.step_arr);		
						}
						
	
					 break;
			
			/* SPTA减速状态 */				
			case DECELERATING:	
	//printf("steps_taken == %d\r\n",drv_spta.steps_taken);				
						if( drv_spta.steps_taken >= drv_spta.steps_required )/* 当前步数 >= 总步数 #达到减速所需步数*/
						{	
	//printf("总步数-当前步数 '0'  -> %d\r\n",(drv_spta.steps_required-drv_spta.steps_taken) );

							/* 转为空闲状态 */						
							drv_spta.step_state = IDLE;					
							break;
						}		
						/* 没到最小速度 && 一个电平状态保持 drv_spta.steps_interval 个周期 */					
						else if( (	drv_spta.step_speed >= 10 ) && ((drv_spta.steps_taken % drv_spta.steps_interval) == 0) ) 
						{
						 /* 减速阶段，速度累加溢出，则速度值递减 */
							drv_spta.step_speed-- ;		

	//printf("step_speed == %d\r\n",drv_spta.step_speed);							
							Delay_ms(1);		
						}				
								
					 break;		
			
			
			/* SPTA空闲状态 */		
			case IDLE:
		
						TIM4->CR1 &= ~(0x1<<0);       //失能计数器;				
	//					TIM4->CCR3 = 0;
						ENABLE = 1;								//失能步进电机
	//printf("\r\nDRV已空闲状态,该次行程已共走steps_taken -> %d\r\n",drv_spta.steps_taken);
	//printf("已空闲状态,当前位置在    TurnMeter   -> %d\r\n",drv_spta.TurnMeter);	
						/* 清零spta相关数据 */
						drv_spta.steps_taken = 0;     //已运行的步数
						drv_spta.step_speed = 0;   //当前加速度（占空比）
						drv_spta.steps_add = 0;      	//需加速的步数  
						drv_spta.steps_all = 0;       //需均速的步数
						drv_spta.steps_cut = 0;       //需减速的步数	
						drv_spta.speed_lim = 0;			  //最大加速度限制	
						drv_spta.steps_interval = 0;  //间隔数
						drv_spta.steps_required = 0;  //总步数
						drv_spta.over =1;
						drv_spta.steps_compare=0;
					//	Delay_ms(50);	
					
					 break;			

			default:           
							break;		
			
		}			
	}

	
	
}
/*
******************************************************
函数功能：启动加减速计算赋值
函数形参： dir：方向 ,   step： 给的步数 
函数返回值：None
作者：hai 
版本：v0.0
******************************************************
*/
void Start_Speedup_Compute( int step )
{

  /* 判断是否正在加减速 */
//  if(drv_spta.step_state != IDLE)//IDLE == 0     1加 2匀速 3减
//    return;
	
  /* 判断方向 */	
//		(step > 0) ? (DIR = 1) : (DIR = 0); 
//		step = abs(step);					//abs函数:取绝对值; abs（-123）= 123；	
//		ENABLE = 0;								//使能步进电机	


	if(Toggle_aisle)
	{
  /* ST---->SPTA运行参数初始化 */
		st_spta.steps_required = step; 

		st_spta.steps_add = (st_spta.steps_required / 4) ;     	                  		                                 //需加速的步数  2
		st_spta.steps_cut = st_spta.steps_add;      																																	 //需减速的步数	 2              								                  	
		st_spta.steps_all = st_spta.steps_required - (st_spta.steps_add + st_spta.steps_cut); 											 //需均速的步数  4      取余数增加精准度
 
		
		
		st_spta.begin =  19600;
		st_spta.finish = st_spta.steps_required - 19600; /* 开始减速 */
		
		st_spta.steps_interval = st_spta.steps_add/ST_Max_speed;  //间隔数
		
		/* 最大速度与需运行步数的多少相干 */	
		st_spta.speed_lim = ST_Max_speed;					//设_65280步 则_占空比使用 ==  101(ARR) 
		
		st_spta.step_state = ACCELERATING;					/* 加速状态 */ 		
		st_spta.step_speed =1;
		st_spta.step_arr = ST_PWM_ARR;
//		printf("间隔数         steps_interval == %d\r\n",drv_spta.steps_interval);
//		printf("最大加速度限制 speed_lim 			== %d\r\n",drv_spta.speed_lim);		
		ST_ENABLE = 1;								//使能步进电机	
		
	}
	else 
	{
  /* DRV---->SPTA运行参数初始化 */
		drv_spta.steps_required = step; 

		drv_spta.steps_add = (drv_spta.steps_required / 4) ;     	                  		                                 //需加速的步数  2
		drv_spta.steps_cut = drv_spta.steps_add;      																																	 //需减速的步数	 2              								                  	
		drv_spta.steps_all = drv_spta.steps_required - (drv_spta.steps_add + drv_spta.steps_cut); 											 //需均速的步数  4      取余数增加精准度


		/* 间隔数 */
		drv_spta.steps_interval = drv_spta.steps_add /DRV_Max_speed ;//drv_spta.steps_add / drv_spta.speed_lim; 
		 
		
			/* 最大速度与需运行步数的多少相干 */	
		drv_spta.speed_lim = DRV_Max_speed;//PWM_cut(drv_spta.steps_add);  
		
		drv_spta.step_state = ACCELERATING;					/* 加速状态 */ 		
		drv_spta.step_speed =1;                     /* 起始速度 */
		drv_spta.step_arr = DRV_PWM_ARR;
		
//		printf("间隔数         steps_interval == %d\r\n",drv_spta.steps_interval);
//		printf("最大加速度限制 speed_lim 			== %d\r\n",drv_spta.speed_lim);		
		ENABLE = 0;								//使能步进电机	
	
	}
	

//		TIM4->CR1 |= 0x1<<0;			//使能计数器:		
		
}





/*
******************************************************
函数功能：SYSTICK初始化
函数形参：None
函数返回值：None
备注：时钟源 / 8  == 9Mhz     计一次时间 = 1 / 9 us
作者：hai 
版本：v0.0
******************************************************
*/
void Systick_Init(void)
{
//	//选择时钟源；
//	SysTick ->CTRL &= ~( 0x1<<2 );//72Mhz (/8) == 选择9Mhz  

//	//对重装载值进行写值；
//	SysTick ->LOAD =9*1000;//1ms
//	
//	//对递减计数器执行写操作；
//	SysTick ->VAL =0;

//	//使能系统滴嗒定时器中断;
//	SysTick ->CTRL |= (0x1<<1);   
//	
//	//关闭定时器
////	SysTick->CTRL &= ~(1 << 0);       
//	SysTick->CTRL |= (1 << 0); 	
	
	/* 1 =失败，0 =成功 */
	if(SysTick_Config(72000))//1ms
	{
		printf("稍等..\r\n");
		while(1);//等待初始化完成;
	}
	printf("SysTick --> 1ms_inter 初始化完成");

}


void SysTick_Handler(void)
{
	static uint32_t runiwdg =0;
		
	SysTick->CTRL &= ~(1 <<16);

	runtime++;/* 记录系统时间 */
	
	runiwdg++;
	if(runiwdg > 1000) // 1S
	{
		runiwdg =0;
		IWDG->KR = 0XAAAA;    //喂狗
	}

}	


void Delay_ms(u32 num) //ms延时
{
	uint32_t tim_ms = runtime;
	
	while( runtime < tim_ms + num ) /* 大于系统时间则退出 */
	{
		;
	}
}


void Delay_us(u32 num) //us延时
{
	
	__NOP();		/* 延时一个指令周期 */	
}

