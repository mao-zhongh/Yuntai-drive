#ifndef ST820_H
#define ST820_H

#include "main.h"
#include "io_bit.h"
/*
																						* ST820 *
		 ST_DIR    = PA3;		//反向控制输入                              GND    =   GND;   
		 ST_STEP   = PA5;		//控制信号输入（上升沿有效）                VDD    =    5V;   
		 ST_RESET  = 5V	 ;                                              2A     =   motor_A
		 NC				   NULL	 ;                                            1A     =   motor_A
		 ST_MODE2  = PA7;		//控制细分数                                1B     =   motor_B
		 ST_MODE1  = PC5;   //控制细分数                                2B     =   motor_B
		 ST_MODE0  = PB1;   //控制细分数                                GND   -----motor -   
		 ST_ENABLE = PB11;   //使能控制输入（低电平有效）               VMOT  -----motor + ( 8.2V~45V )
*/



#define ST_ENABLE  PBout(11)
#define ST_DIR     PAout(3)
#define ST_DIR_    PAin(3)
#define ST_STEP    PAout(5)
#define ST_MODE0   PBout(1)
#define	ST_MODE1   PCout(5)
#define ST_MODE2   PAout(7)


#define st_scope         (256*FSPR)/2           	/* [1/256] 细分后一圈所需脉冲数 */
#define st_left      								0   					//左顺
#define st_right     								1  						//右逆

#define st_Full_step    ST_MODE0=0,ST_MODE1=0,ST_MODE2=0;             //   000_ FULL
#define st_2_step     	ST_MODE0=0,ST_MODE1=0,ST_MODE2=1;             //   001_ 1/2
#define st_4_step     	ST_MODE0=0,ST_MODE1=1,ST_MODE2=0;             //   010_ 1/4
#define st_8_step     	ST_MODE0=0,ST_MODE1=1,ST_MODE2=1;             //   011_ 1/8
#define st_16_step     	ST_MODE0=1,ST_MODE1=0,ST_MODE2=0;             //   100_ 1/16
#define st_32_step     	ST_MODE0=1,ST_MODE1=0,ST_MODE2=1;             //   101_ 1/32
#define st_128_step     ST_MODE0=1,ST_MODE1=1,ST_MODE2=0;             //   110_ 1/128
#define st_256_step     ST_MODE0=1,ST_MODE1=1,ST_MODE2=1;             //   111_ 1/256

extern u8 ST_Direction;		//方向
//extern u32 ST_TurnMeter_1,ST_TurnMeter_0 ;
//extern u32 ST_lnterruptnum;
 


unsigned char ST820_Drive( int step );
int ST_Compare_TurnMeter(void);

void ST820_Init(void);

#endif