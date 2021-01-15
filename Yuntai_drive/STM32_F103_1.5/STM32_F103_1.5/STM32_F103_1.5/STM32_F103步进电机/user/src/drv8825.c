#include "drv8825.h"


u8 Drv_Direction=0;     /* 方向  */

/*
******************************************************
函数功能：drv8825管脚初始化   PB9_CH4
函数形参：None
函数返回值：1-->遇新PELCO_D指令   0-->正执行完成
备注：管脚定义参考.h文件
版本：v0.0
******************************************************
*/

void DRV8825_Init(void)
{
	RCC->APB2ENR |= 0x7<<2;// A B C 口使能

	GPIOA->CRH &= ~(0XF<<16); // PA12 通用推挽输出 10MHZ
	GPIOA->CRH |= 0X1<<16;	
	
	
	GPIOB->CRL &= ~(0xf<<28);//PB3 5 7 通用推挽输出 10MHZ
	GPIOB->CRL |= 0x01<<28;		
	GPIOB->CRL &= ~(0xf<<20);
  GPIOB->CRL |= 0x01<<20;		
	GPIOB->CRL &= ~(0xf<<12);
  GPIOB->CRL |= 0x01<<12;	


	GPIOC->CRH &= ~(0XF<<8);// pc10 pc12  通用推挽输出 10MHZ
	GPIOC->CRH |= 0x1<<8;
	GPIOC->CRH &= ~(0XF<<16);
	GPIOC->CRH |= 0x1<<16;
}


unsigned char DRV_Drive(int step)
{
	u8 state = IDLE;         /* 上次状态 */
	static u8 Drv_operation_dir=0; //用于保存_接收新指令前面电机的运行方向    判断下一动作与这一动作是否冲突	
	Toggle_aisle = 0;    TIM4_Tint_PWM();  /* PWM通道初始化 */

	motor_disturb=1;       /*监听是否有新指令*/
	ENABLE =0;   				 	 /*使能输入*/

  /* 判断方向 */	
	(step > 0) ? (DIR = 1) : (DIR = 0); 
	step = abs(step);					//abs函数:取绝对值; abs（-123）= 123；

	/* 转为空闲状态 清spta相关数据 */						
//	drv_spta.step_state = IDLE;
//	Speed_Decision(); 
//	drv_spta.over = 0;

	Start_Speedup_Compute(step);
	if( Continue_disturb )
	{
		Continue_disturb = 0;         //清相应标志		
		/*只有收到中断指令才进来*/
		if( DIR == Drv_operation_dir )/* 现运行方向 == 指令前运行方向 */
		{
			LED_1 =! LED_1 ;
			drv_spta.step_arr = drv_spta.last_arr;//继续指令前速度运行
		}
		else /* 不同方向,重新缓启 */
		{
			drv_spta.steps_taken = 0;             //清已运行的步数	按现接收指令步数运行			
		}
	}



	TIM4->CR1 |= (0x1<<0);//开始计数器 		
	
		/* 让程序卡在这执行 */	
		while( !drv_spta.over ) 
		{
			if(!restart)
			{/*   下限位	                                                                 上限位*/
				if( (DRV_Compare_TurnMeter() >= drv_scope && drv_up != (DRV_DIR_&0x01) ) || ( DRV_Compare_TurnMeter() == drv_up && drv_down != (DRV_DIR_&0x01)  ) )
				{
					goto DRV_out;
				}				
			}			

			Speed_Decision();
//			Delay_us(13);//1/32
			if( Continue_disturb )    //如突然有新指令则退出电机运行				
			{	//Delay_ms(10);	

				TIM4->CR1 &= ~(0x1<<0);			  //失能计数器:	

//				drv_spta.steps_taken = 0;     //清已运行的步数	按现接收指令步数运行
				
				/* 记录退出时运行方向,用于下次同方向时速度无缝链接^_^ */				
				Drv_operation_dir = DIR;
				/* 记录退出时ARR的值,用于下次同方向时速度无缝链接^_^ */
				drv_spta.last_arr = drv_spta.step_arr;
				ENABLE = 1;								/* 防单停止指令,发烫 */ 
				return 1;
			}                                                                                                    //  1        0  
		
		}

//LED_1 =1;
DRV_out:
//	printf("TurnMeter = %d\r\n  ",drv_spta.TurnMeter);


	/* 转为空闲状态 清spta相关数据 */						
	drv_spta.step_state = IDLE;
	Speed_Decision(); 
	drv_spta.over = 0;

	STMFLASH_ErasePage(FLASH_Presetting);//擦除		//Erase current sector  清扇区	
	Write_EEPROM(Drv_Direction,FLASH_Presetting_DRV,DRV_Compare_TurnMeter());
	Write_EEPROM(ST_Direction,FLASH_Presetting_ST,ST_Compare_TurnMeter());	
	FLASH_Lock();																			//上锁		
	motor_disturb = 0;				//正常运行完毕	
	Delay_ms(50);	


	return 0;
}


/*
******************************************************
函数功能：drv8825 ->根据运动方向判断步进电机的运行位置 
函数形参：None
函数返回值：返回当前位置值
备注：1# 因限制 drv_spta.TurnMeter 只会在 (0~向下最大限制) 为正整数
      2# 返回值为往 Drv_Direction 运行 step 到原点
版本：v0.0
******************************************************
*/
int DRV_Compare_TurnMeter(void)
{
	int32_t step =0;
	step = drv_spta.TurnMeter;

	if(step == drv_up)       /* 到上限位不返回步数 */
	{
		Drv_Direction = 1;				 //方向
		return 0;
	}
	else if(step >= drv_down)/* 如现执行向下操作,则返回向上操作指令及步数 */
	{
		Drv_Direction = 0;				 //方向
	//	step = (~step+1);
		return step;
	}
	

}

