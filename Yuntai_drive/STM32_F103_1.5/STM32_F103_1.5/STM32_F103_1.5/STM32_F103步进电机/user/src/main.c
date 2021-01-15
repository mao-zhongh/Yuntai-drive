#include "main.h"



enum PELCO_D_CMD
{
	ZOOM_IN = 0,																																	/* 向左移动   0*/
	ZOOM_OUT ,																																		/* 向右移动   1*/
	Mount,                                                                        /* 向上移动   2*/
	Down,                                                                         /* 向下移动   3*/
	
	set_1,                                                                        /* 预置位1设置       4*/
	call_1,                                                                       /* 预置位1调用       5*/
	set_2,																																				/* 预置位2设置       6*/
	call_2,                                                                       /* 预置位2调用       7*/
	set_3,																																				/* 预置位3设置       8*/
	call_3,                                                                       /* 预置位3调用       9*/		
	set_4,																																				/* 预置位4设置      10*/
	call_4,                                                                       /* 预置位4调用      11*/			
	set_5,																																				/* 预置位5设置      12*/
	call_5,                                                                       /* 预置位5调用      13*/	

	
	STOP ,																																				/* 停止	 						4*/
	reset ,																																				/* 重置	 						5*/
};

/****************************************************************************
PELCO_D 协议说明：
数据格式：1位起始位、8位数据、1位停止位、无校验位;
命令格式：|  字节1   |  字节2 |  字节3   |   字节4  |  字节5   |   字节6  |  字节7 |
					  同步字节   地址码    指令码1    指令码2    数据码1    数据码2		校验码

1.该协议中所有数值都为十六进制数
2.同步字节始终为FFH
3.地址码为摄像机的逻辑地址号，地址范围：00H~FFH
4.指令码表示不同的动作
5.数据码1、2分别表示水平、垂直方向速度(00~3FH),FFH表示“turbo” 速度
6.校验码 = MOD[(字节2+字节3+字节4+字节5+字节6)/100H]
			以下地址码为0x01
****************************************************************************/

unsigned char PELCO_D[][7] = {
	/* 右转 逆时针*/
	{0xff,0x01,0x00,0x04,0x0F,0x00,0x14},//ff 01 00 04 0f 00 14

	/* 左转 顺时针 */	
	{0xff,0x01,0x00,0x02,0x0F,0x00,0x12},//ff 01 00 02 0f 00 12

	/* 向上 */
	{0xFF,0x01,0x00,0x08,0x00,0x1C,0x25},//FF 01 00 08 00 1C 25 
	
	/* 向下 */
	{0xFF,0x01,0x00,0x10,0x00,0x1C,0x2D},//FF 01 00 10 00 1C 2D 	
	
	/* 预置位1设置 */
	{0xff,0x01,0x00,0x03,0x00,0x01,0x05},//ff 01 00 03 00 01 05
	
	/* 预置位1调用 */
	{0xff,0x01,0x00,0x07,0x00,0x01,0x09},//ff 01 00 07 00 01 09
	
	/* 预置位2设置 */	
	{0xFF,0x01,0x00,0x03,0x00,0x02,0x06},//ff 01 00 03 00 02 06

	/* 预置位2调用 */
	{0xFF,0x01,0x00,0x07,0x00,0x02,0x0A},//ff 01 00 07 00 02 0a	
	
	/* 预置位3设置 */	
	{0xFF,0x01,0x00,0x03,0x00,0x03,0x07},//ff 01 00 03 00 03 07

	/* 预置位3调用 */
	{0xFF,0x01,0x00,0x07,0x00,0x03,0x0B},//ff 01 00 07 00 03 0b		
	
	
	/* 预置位4设置 */
	{0xff,0x01,0x00,0x03,0x00,0x04,0x08},//ff 01 00 03 00 04 08
	
	/* 预置位4调用 */
	{0xff,0x01,0x00,0x07,0x00,0x04,0x0c},//ff 01 00 07 00 04 0c

	/* 预置位5设置 */
	{0xff,0x01,0x00,0x03,0x00,0x05,0x09},//ff 01 00 03 00 05 09

	/* 预置位5调用 */
	{0xff,0x01,0x00,0x07,0x00,0x05,0x0D},//ff 01 00 07 00 05 0d	
	
	
	

	/* 停止 */
	{0xff,0x01,0x00,0x00,0x00,0x00,0x01},//ff 01 00 00 00 00 01

	/* 重置 回归起始位 */
	{0xff,0x01,0x98,0x00,0x0f,0x11,0xb9},//FF 01 98 00 0F 11 B9 
};


/************************************调用预置位*****************************************/
/*
函数功能：根据当前位置值计算出离预置位的(current position)的数据值
函数参数：( step_A(电机现在的位置)   step_B(预置位的位置)    , count 成员 )
函数返回值：none   
备注：		1/500 = 0
*/
unsigned int DRV_Current_Position(unsigned int step_A,unsigned int step_B,unsigned char count )      //DRV
{
		if( Drv_Direction == FLASH_DATA.DRVROM_Dir[count] )//都在同一方向时
		{
			if( step_A > step_B )
			return step_B = step_A - step_B;
			//FLASH_DATA.DRVROM_Dir != ROM_Dir;
			
			else //if(step_A < step_B )
			{
				FLASH_DATA.DRVROM_Dir[count] =! FLASH_DATA.DRVROM_Dir[count];					
				return step_B = step_B - step_A;								
			}			
		}
		else //if( Drv_Direction != ROM_Dir )								
		{
			FLASH_DATA.DRVROM_Dir[count] =! FLASH_DATA.DRVROM_Dir[count];
			return step_B = step_B + step_A;
		}
}
/*
函数功能：根据当前位置值计算出离预置位的(current position)的数据值
函数参数：( step_A(电机现在的位置)   step_B(预置位的位置)    , count 成员 )
函数返回值：none   
备注：		1/500 = 0
*/
unsigned int ST_Current_Position(unsigned int step_A,unsigned int step_B,unsigned char count )     //ST
{
		if( ST_Direction == FLASH_DATA.STROM_Dir[count] )//都在同一方向时
		{
			if( step_A > step_B )
			return step_B = step_A - step_B;
			
			else
			{
				FLASH_DATA.STROM_Dir[count] =! FLASH_DATA.STROM_Dir[count];					
				return step_B = step_B - step_A;								
			}			
		}
		else 							
		{
			FLASH_DATA.STROM_Dir[count] =! FLASH_DATA.STROM_Dir[count];
			return step_B = step_B + step_A;
		}
}
/************************************设置预置位*****************************************/
void Ser_Presetting_bit(u8 numbers,u32 addr_)
{
		FLASH_Unlock();                    								 //解锁	
		Delay_us(100);	
		STMFLASH_Read( addr_ );  //读该扇区值
		//ZOOM_IN,ZOOM_OUT,Mount,Down	
		FLASH_DATA.DRVROM_Step[numbers] = DRV_Compare_TurnMeter();    //计当前位置离原点步数
		FLASH_DATA.DRVROM_Dir[numbers] = Drv_Direction;
		FLASH_DATA.STROM_Step[numbers] = ST_Compare_TurnMeter();
		FLASH_DATA.STROM_Dir[numbers] = ST_Direction;										

		STMFLASH_Writ( addr_ ); //对一个扇区写值

		Delay_us(10);	
}
/************************************设置预置位*****************************************/

/************************************调用预置位*****************************************/
u8 Call_presetting_bit(u8 numbers,u32 addr)
{
	int dev_step_=0,st_step=0;
	u8 temp_res=0;

	
	dev_step_ = DRV_Compare_TurnMeter();    	//返回当前位置值
	st_step  = ST_Compare_TurnMeter();
	/*返回的都是负整数*/
	//dev_step_ = abs(dev_step_);					    //abs函数:取绝对值; abs（-123）= 123；
	
	Read_DRV_EEPROM(addr,numbers); 					//DRV读一个预置位值
  //ST地址在DRV地址基础加8字节
	Read_ST_EEPROM((addr+=8),numbers); 			//ST 读一个预置位值        

	
//	temp_res = DRV_Drive(FLASH_DATA.DRVROM_Dir[numbers],DRV_Current_Position( dev_step,DRVROM_Step[numbers],numbers));
	dev_step_ = DRV_Current_Position( (unsigned int)dev_step_,(unsigned int)FLASH_DATA.DRVROM_Step[numbers],numbers);
	/* 根据方向转换'-,+'步数 */
	(FLASH_DATA.DRVROM_Dir[numbers] == 1) ? (dev_step_ = dev_step_) : ( dev_step_ = ~dev_step_ +1);
	temp_res = DRV_Drive(dev_step_);
	if(temp_res)      { return 1; }		 /*中途到接收新指令*/
	
	
	Delay_us(10);		
	
	
	st_step = ST_Current_Position( (unsigned int)st_step,(unsigned int)FLASH_DATA.STROM_Step[numbers],numbers);
	(FLASH_DATA.STROM_Dir[numbers] == 1) ? (st_step = st_step) : ( st_step = ~st_step +1);
	temp_res = ST820_Drive(st_step); 
	if(temp_res)      { return 1; }		

	return 0;	
}
/***************************读取flash中存储掉电前的位置,并回到原点,优先级最高****************************************/
void Restart_MOTOR(void)
{
/*                       flash测试                          */
	USART1->CR1 &= ~(0X1<<13);//USART1失能;
	restart = 1;

//	FLASH_DATA.DRVROM_Dir[0] = 0;
//	FLASH_DATA.DRVROM_Step[0] =0;
//	FLASH_DATA.STROM_Dir[0] =0;
//	FLASH_DATA.STROM_Step[0] =0;
	

	Read_DRV_EEPROM(FLASH_Presetting_DRV,0);//读
	Read_ST_EEPROM(FLASH_Presetting_ST,0); //读

	Delay_ms(500);		Delay_ms(500);	
	
//	DRV_Drive(1);
	((FLASH_DATA.STROM_Dir[0]&0x01)  == st_right) ? (ST820_Drive(FLASH_DATA.STROM_Step[0])) : ( ST820_Drive((-FLASH_DATA.STROM_Step[0])) );	
	((FLASH_DATA.DRVROM_Dir[0]&0x01) == drv_down) ? (DRV_Drive(FLASH_DATA.DRVROM_Step[0]) ) : ( DRV_Drive((-FLASH_DATA.DRVROM_Step[0])) );	


//	printf("FLASH_DATA.DRVROM_Dir[0] == %d\r\n",FLASH_DATA.DRVROM_Dir[0]);
//	printf("dev_step == %d\r\n",FLASH_DATA.DRVROM_Step[0]);
//	printf("FLASH_DATA.STROM_Dir[0] == %d\r\n",FLASH_DATA.STROM_Dir[0]);
//	printf("STROM_Step == %d\r\n",FLASH_DATA.STROM_Step[0]);		
	
	/*防止一直开关电源*/
	STMFLASH_ErasePage(FLASH_Presetting);   //擦除	
	Write_EEPROM(0,FLASH_Presetting_DRV,0); //上电复位记录点写0		
	Write_EEPROM(0,FLASH_Presetting_ST,0);  //上电复位记录点写0		

	st_spta.TurnMeter = 0; 
	drv_spta.TurnMeter = 0;	
	restart = 0;
	USART1->CR1 |= 0X1<<13;//USART1使能;
/*                       flash测试                          */	
}


int main(void)
{

	unsigned char reverse = STOP;								//存放指令	
	unsigned char temp_IN =0;										//响应中断否              
	int dev_step=0;    //存放临时ram空间步数
	int st_step=0;
	
//	这里的代码只会执行一次  初始化代码
	NVIC_SetPriorityGrouping(7-2);  //2抢占(0-3)   2响应(0-3)	
	
	

	/*LED初始化*/
	LED_Iint();
	
	/*串口初始化*/
	Usart1__Init(115200);
	
	/*电机IO初始化*/
	DRV8825_Init();
	ST820_Init();

	/*TIM4->PWM3_4通道初始化*/
	TIM4_Tint_PWM();/*	 70us方波  14.287Khz  DRV */
	
 /* 步距脚 7.5°/85  [Full_step时11.333(step)为1°；4079.999(step)走360°] */
 /* [8159 1/2]  [16320 1/4]  [32640 1/8]   [65280 1/16]   [130560 1/32]   [522240 1/128]  [1044480 1/256]运行节拍数 */	
 /* 细分数设置 */
	drv_32_step;
	st_256_step;
	
	/*系统时钟初始化->产生1ms中断*/	
	Systick_Init(); 	Iwdg_Init();  //   独立看门狗 ->1s检测；	
	
//	FLASH_SetLatency(FLASH_Latency_2);       //FLASH设置延迟 72Mhz两个延时周期
	Delay_ms(500);	
	/*根据掉电前位执行复位原点*/
	Restart_MOTOR();


		
	Delay_ms(100);
	printf("复位了\r\n");

	while(1)
	{		

#if 1
Interrupt_to_bothe:			
			if(UART1_Overflow_Flag)						//缓冲区满标志		（ UART1_Recv_Buf[] 已存放了可以比较的数据）
			{ 	
					/* 变倍短 (放大+) 0*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[ZOOM_IN][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[ZOOM_IN][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[ZOOM_IN][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[ZOOM_IN][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[ZOOM_IN][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[ZOOM_IN][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[ZOOM_IN][6]))
						{
								reverse = ZOOM_IN;   		//右转 Dir1
						}

						/* 变倍长 缩小- 	  1*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[ZOOM_OUT][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[ZOOM_OUT][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[ZOOM_OUT][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[ZOOM_OUT][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[ZOOM_OUT][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[ZOOM_OUT][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[ZOOM_OUT][6]))
						{
							reverse = ZOOM_OUT;   		//左转 Dir0
						}
						
		 if(	 	 (UART1_Recv_Buf[0] == PELCO_D[Mount][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[Mount][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[Mount][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[Mount][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[Mount][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[Mount][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[Mount][6]))
						{
							reverse = Mount;   		//向上 Dir0
						}					
						

		 if(	 	 (UART1_Recv_Buf[0] == PELCO_D[Down][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[Down][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[Down][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[Down][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[Down][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[Down][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[Down][6]))
						{
							reverse = Down;   		//向下 Dir1
						}							

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_1][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_1][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_1][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_1][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_1][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_1][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_1][6]))
						{
							reverse = set_1;   				//预置位1设置
						}						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_1][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_1][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_1][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_1][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_1][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_1][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_1][6]))
						{
							reverse = call_1;   				//预置位1调用
						}									
						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_2][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_2][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_2][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_2][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_2][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_2][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_2][6]))
						{
							reverse = set_2;   				//预置位2设置
						}						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_2][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_2][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_2][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_2][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_2][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_2][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_2][6]))
						{
							reverse = call_2;   				//预置位2调用
						}		
												
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_3][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_3][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_3][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_3][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_3][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_3][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_3][6]))
						{
							reverse = set_3;   				//预置位3设置
						}						

					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_3][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_3][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_3][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_3][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_3][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_3][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_3][6]))
						{
							reverse = call_3;   				//预置位3调用
						}								

						
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_4][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_4][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_4][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_4][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_4][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_4][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_4][6]))
						{
							reverse = set_4;   				//预置位4调用
						}		


					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_4][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_4][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_4][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_4][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_4][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_4][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_4][6]))
						{
							reverse = call_4;   				//预置位4调用
						}								
						
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[set_5][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[set_5][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[set_5][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[set_5][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[set_5][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[set_5][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[set_5][6]))
						{
							reverse = set_5;   				//预置位5调用
						}		


					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[call_5][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[call_5][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[call_5][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[call_5][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[call_5][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[call_5][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[call_5][6]))
						{
							reverse = call_5;   				//预置位5调用
						}	
						
						/* 停止	 						2*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[STOP][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[STOP][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[STOP][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[STOP][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[STOP][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[STOP][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[STOP][6]))
						{
							reverse = STOP;   				//停止移动							
						}

					  /* 重置 回归原位 3*/
					if(	 	 (UART1_Recv_Buf[0] == PELCO_D[reset][0])&&
								 (UART1_Recv_Buf[1] == PELCO_D[reset][1])&&
								 (UART1_Recv_Buf[2] == PELCO_D[reset][2])&&
								 (UART1_Recv_Buf[3] == PELCO_D[reset][3])&&
								 (UART1_Recv_Buf[4] == PELCO_D[reset][4])&&
								 (UART1_Recv_Buf[5] == PELCO_D[reset][5])&&
								 (UART1_Recv_Buf[6] == PELCO_D[reset][6]))
						{
							reverse = reset;   				//复位指令		
						}						


						
						
						
					UART1_Flag_count = 0;  	 			//清零统计接收次数
					UART1_Overflow_Flag = 0;			//允许串口继续接收数据 

						
						
						
			}

		switch(reverse)
			{		
				
				case ZOOM_IN : //LED_1 =! LED_1;
											st_step = ST_Compare_TurnMeter();
											if(st_step >= st_scope && ST_Direction == st_right ) { st_step = 0; break; }//返回电机左转方向标志 65280是45°度的    
											
											temp_IN = ST820_Drive(-130560);// 左顺  45°
											if( temp_IN ) { goto Interrupt_to_bothe; }			/* reverse = STOP; */	
											Delay_us(100);											
											break;

				case ZOOM_OUT://LED_1 =! LED_1;
											st_step = ST_Compare_TurnMeter();
											if(st_step >= st_scope && ST_Direction == st_left) { st_step = 0; break; }//返回电机左转方向标志   或者goto 到清标志处
											temp_IN = ST820_Drive(130560);//  右逆45°
											if( temp_IN ) { goto Interrupt_to_bothe; }
											Delay_us(100);
											break;

				case Mount	 ://LED_1 =! LED_1;

											dev_step = DRV_Compare_TurnMeter();       //到下限位,退出
											if(dev_step >= drv_scope ) { dev_step = 0; break; };
											temp_IN = DRV_Drive(16320);//8160 下顺 45°移动  
											if( temp_IN ) { goto Interrupt_to_bothe; }
											Delay_us(100);
											break;
				

				case Down		 :		//LED_1 =! LED_1;		
											dev_step = DRV_Compare_TurnMeter();
											if(dev_step == drv_up ) { dev_step = 0; break; }//到上限位,退出 
											
											temp_IN = DRV_Drive(-16320);//8160 上逆 45°移动
											if( temp_IN ) { goto Interrupt_to_bothe; }				
											Delay_us(100);											
											break;		
				

											
				case set_1   :	/* 0123 一个完整预置位  */
											Ser_Presetting_bit(0,FLASH_start_ADDRESS);
											Delay_us(100);
											break;					

				
				case call_1	 :					
											temp_IN = Call_presetting_bit(0,one_ADDRESS); 
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//说明相应中断
											Delay_us(100);
											break;

				case set_2   :	

											Ser_Presetting_bit(1,FLASH_start_ADDRESS);			
											Delay_us(100);
											break;
				
				
				case call_2	 :

											temp_IN = Call_presetting_bit(1,two_ADDRESS );
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//说明相应中断				
											Delay_us(100);
											break;
		/******************************/	
				case set_3   :	

											Ser_Presetting_bit(2,FLASH_start_ADDRESS);											
											Delay_us(100);
											break;
				
				
				case call_3	 :

											temp_IN = Call_presetting_bit( 2,three_ADDRESS );	
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//说明相应中断						
											Delay_us(100);
											break;
	
				case set_4   :	

											Ser_Presetting_bit(3,FLASH_start_ADDRESS);											
											Delay_us(100);
											break;
				
				case call_4	 :
					
											temp_IN = Call_presetting_bit( 3,foue_ADDRESS );
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//说明相应中断						
											Delay_us(100);				
											break;				
				
				
				case set_5   :	
					
											Ser_Presetting_bit(4,FLASH_start_ADDRESS);
											Delay_us(100);
											break;

				case call_5	 :
		
											temp_IN = Call_presetting_bit(4,five_ADDRESS);
											if( temp_IN )   { reverse = STOP;  goto Interrupt_to_bothe; }//说明相应中断						
											Delay_us(100);				
											break;					
				
				
				
				
				
				case STOP    :
											motor_disturb = 0;	
											TIM4->CR1 &= ~(0x1<<0);			/*  失能计数器  */	
											ENABLE = 1;									/* 及时失能止烫 */
											ST_ENABLE = 0;              

											/*记录停止前位置*/
											STMFLASH_ErasePage(FLASH_Presetting);//擦除		//Erase current sector  清扇区	
											Write_EEPROM(Drv_Direction,FLASH_Presetting_DRV,DRV_Compare_TurnMeter());
											Write_EEPROM(ST_Direction,FLASH_Presetting_ST,ST_Compare_TurnMeter());	
											FLASH_Lock();																			//上锁
				
											drv_spta.steps_taken = 0;     //已运行的步数
											st_spta.steps_taken = 0; 			
											Delay_ms(100);			


											break;				

				case reset   :/* 复位->最高优先级指令 */

											USART1->CR1 &= ~(0X1<<13);//USART1失能;
				
											/*DRV 上限位的特殊性 复位值只为负数*/
											dev_step = DRV_Compare_TurnMeter();
											DRV_Drive(~dev_step+1);
				
											/*ST_Direction为‘0’时,则右逆,赋值给驱动函数该为负整数*/
											st_step = ST_Compare_TurnMeter();
											(ST_Direction == st_right) ? (ST820_Drive(st_step)) : (ST820_Drive(-st_step));			

											drv_spta.TurnMeter = 0;	
											st_spta.TurnMeter  = 0;
											
											USART1->CR1 |= 0X1<<13;//USART1使能;	
							//				printf("dev_step %d",(~dev_step+1));
	
											Delay_ms(100);
											break;												
				default: 			
											break;

			}
			reverse = STOP;//默认停止		
#endif		
	
			
	}
}



