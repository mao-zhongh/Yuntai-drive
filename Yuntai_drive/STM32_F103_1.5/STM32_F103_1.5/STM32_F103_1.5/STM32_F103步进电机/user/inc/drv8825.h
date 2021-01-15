#ifndef _DRV8825_H_
#define _DRV8825_H_

/*
																						* DRV8825 *
		 DIR    = PB14;		//反向控制输入                                  GND    =   GND;   
		 STEP   = PC6;		//控制信号输入（上升沿有效）                    FAULT      NULL
		 SLEEP  = 5V;                                                     2A     =   motor_A
		 RESET  = 5V;                                                     1A     =   motor_A
		 MODE2  = PC8;   //控制细分数                                     1B     =   motor_B
		 MODE1  = PA12;   //控制细分数                                    2B     =   motor_B
		 MODE0  = PC10;		//控制细分数                                    GND   -----motor -   
		 ENABLE = PC12;   //使能控制输入（低电平有效）                    VMOT  -----motor + ( 8.2V~45V )
*/
#include "main.h"
#include "io_bit.h"

#define ENABLE   PBout(7)                   // PCout(12)
#define DIR      PAout(12)                  // PBout(14)
#define DRV_DIR_ PAin(12)                   // PBin(14)
#define STEP     PCout(10)                  // PCout(6)
#define MODE0    PBout(5)                   // PCout(10)
#define	MODE1    PBout(3)                   // PAout(12)
#define MODE2    PCout(12)                  // PCout(8)

#define drv_up       0      //上, 逆时针
#define drv_down     1      //下, 顺时针

#define drv_Full_step   	MODE0=0,MODE1=0,MODE2=0;   // 000_Full
#define drv_2_step   			MODE0=0,MODE1=0,MODE2=1;   // 000_1/2
#define drv_4_step   			MODE0=0,MODE1=1,MODE2=0;   // 010_1/4
#define drv_8_step   			MODE0=0,MODE1=1,MODE2=1;   // 011_1/8  
#define drv_16_step   		MODE0=1,MODE1=0,MODE2=0;   // 100_1/16  
#define drv_32_step   		MODE0=1,MODE1=1,MODE2=1;   // 111_1/32    

/*电机单圈参数*/
#define STEP_ANGLE        11.333f                 //步进电机的步距角 单位：度
#define FSPR              (360.0f*STEP_ANGLE)     //步进电机的一圈所需脉冲数   4079.98
#define drv_scope         (32*FSPR)/2           	/* [1/32] 细分后一圈所需脉冲数 */



//extern u32 Drv_lnterruptnum;
//extern u32 TurnMeter_1,TurnMeter_0;
extern u8 Drv_Direction;


void DRV8825_Init(void);
unsigned char DRV_Drive(int step);

/*返回当前位置值*/
int DRV_Compare_TurnMeter(void);
#endif

