#ifndef ST820_H
#define ST820_H

#include "main.h"
#include "io_bit.h"
/*
																						* ST820 *
		 ST_DIR    = PA3;		//反向控制输入                              GND    =   GND;   
		 ST_STEP   = PA5;		//控制信号输入（上升沿有效）                VDD    =    5V;   
		 ST_RESET  = 5V	 ;                                                2A     =   motor_A
		 NC							 ;                                                1A     =   motor_A
		 ST_MODE2  = PA7;		//控制细分数                                1B     =   motor_B
		 ST_MODE1  = PC5;   //控制细分数                                 2B     =   motor_B
		 ST_MODE0  = PB1;   //控制细分数                                 GND   -----motor -   
		 ST_ENABLE = PB11;   //使能控制输入（低电平有效）                 VMOT  -----motor + ( 8.2V~45V )
*/


//sbit ST_DIR    = P2^0;		//反向控制输入
//sbit ST_STEP   = P2^1;		//控制信号输入（上升沿有效）
//sbit ST_MODE2  = P2^2;		//控制细分数
//sbit ST_MODE1  = P2^3;   //控制细分数
//sbit ST_MODE0  = P2^4;   //控制细分数
//sbit ST_ENABLE = P2^5;   //使能控制输入（低电平有效）



#define ST_ENABLE  PBout(11)
#define ST_DIR     PAout(3)
#define ST_DIR_    PAin(3)
#define ST_STEP    PAout(5)
#define ST_MODE0   PBout(1)
#define	ST_MODE1   PCout(5)
#define ST_MODE2   PAout(7)



//extern unsigned int ST_TurnMeter_1;
//extern unsigned int ST_TurnMeter_0;
extern u8 ST_Direction;		//方向
extern u32 ST_TurnMeter_1,ST_TurnMeter_0 ;
extern u32 ST_lnterruptnum;
 


unsigned char ST820_motor(unsigned char ST_Direc,unsigned int num);
unsigned int ST_Compare_TurnMeter(void);

void ST820_Init(void);

#endif