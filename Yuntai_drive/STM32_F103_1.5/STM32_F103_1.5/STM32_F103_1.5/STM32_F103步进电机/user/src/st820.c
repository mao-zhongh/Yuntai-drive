#include "st820.h"

//u32 ST_TurnMeter_1 = 0,ST_TurnMeter_0 = 0;
//u32 ST_lnterruptnum = 0;
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
函数功能    ：ST820 驱动减速步进电机30BYJ-26驱动函数
函数参数    ：int step 移动的节拍数量 -->( 根据正负数判断 移动方向：0：顺时针    1：逆时针 )
函数返回值  ：1-->遇新PELCO_D指令   0-->正执行完成 
备注        ：管脚定义参考.h文件
版本        ：v0.0
****************************************
*/
unsigned char ST820_Drive( int step )
{
	static u8 ST_operation_dir=0;
	Toggle_aisle = 1;    TIM4_Tint_PWM();  /* PWM通道初始化 */
	
	motor_disturb=1;        /*监听是否有新指令*/
	ST_ENABLE = 1;    			/*使能输入*/

  /* 判断方向 */	
	(step > 0) ? (ST_DIR = 1) : (ST_DIR = 0); 
	step = abs(step);					//abs函数:取绝对值; abs（-123）= 123；

/*驱动前按照Step算出加速及减速的步数段*/	
	Start_Speedup_Compute(step);
//	LED_1 =! LED_1 ;
	
	if(Continue_disturb)
	{
		Continue_disturb = 0;         //清相应标志
		if( ST_DIR == ST_operation_dir )/* 现运行方向 == 指令前运行方向 */
		{

			LED_1 =! LED_1 ;
			st_spta.step_arr = 120;//st_spta.last_arr;//继续指令前速度运行
		}
		else 
		{
			st_spta.steps_taken = 0;     //清已运行的步数	按现接收指令步数运行			
		}		
	}
	TIM4->CR1 |= (0x1<<0);//开始计数器 		


	while( !st_spta.over )
	{		
      /* 运行过程中检测是否超左右限位 */  
			if(ST_Compare_TurnMeter() >= st_scope &&  ST_Direction != (ST_DIR_&0X01)  ) //&& ST_Direction == st_left) || (ST_Compare_TurnMeter() >= st_scope && ST_Direction == st_right) )
			{
				goto ST_out;
			}
			Speed_Decision();			
			if( Continue_disturb )         //如突然有新指令则退出电机运行
			{
				TIM4->CR1 &= ~(0x1<<0);			  //失能计数器:					
//Continue_disturb = 0;         //清相应标志			

				/* 转为空闲状态 清spta相关数据 */						
				st_spta.steps_taken = 0;     //已运行的步数		
				/* 记录退出时运行方向,用于下次同方向时速度无缝链接^_^ */				
				ST_operation_dir = ST_DIR;
				/* 记录退出时ARR的值,用于下次同方向时速度无缝链接^_^ */
				st_spta.last_arr = st_spta.step_arr;


				
				ST_ENABLE = 0;    			     //失能输入				
				//Delay2ms();
				return 1;
			}
	}
ST_out:	

	/* 转为空闲状态 清spta相关数据 */						
	st_spta.step_state = IDLE;
	Speed_Decision(); 
	st_spta.over = 0;


	/*每次运行完成记录当前位置*/
	STMFLASH_ErasePage(FLASH_Presetting);//擦除		//Erase current sector  清扇区	
	Write_EEPROM(Drv_Direction,FLASH_Presetting_DRV,DRV_Compare_TurnMeter());
	Write_EEPROM(ST_Direction,FLASH_Presetting_ST,ST_Compare_TurnMeter());	
	FLASH_Lock();																			//上锁
	
  motor_disturb=0;				//正常运行完毕		
	Delay_us(50);		
	return 0;
}


/*
******************************************************
函数功能：ST820 ->根据运动方向判断步进电机的运行位置 
函数形参：None
函数返回值：返回当前位置值
备注：1# st_spta.TurnMeter ‘左’顺时_为正整数;   ‘右’逆时_为负整数; 方便比较返回出来时转换成正整数
      2# 返回值为往 ST_Direction 运行 Step 为原点

版本：v0.0
******************************************************
*/
int ST_Compare_TurnMeter(void)
{
	int32_t Step = 0;
	Step = st_spta.TurnMeter;
	
		if( Step > 0 )         /* 左顺 */
		{
			ST_Direction=0;		
			return Step;
		}
		else              		 /* 右逆 */
		{
			ST_Direction=1;		
			Step = abs(Step);    /* 取绝对值 */   
			return Step;
		}
}




