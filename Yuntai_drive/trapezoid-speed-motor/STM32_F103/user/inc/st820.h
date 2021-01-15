#ifndef ST820_H
#define ST820_H

#include "main.h"
#include "io_bit.h"
/*
																						* ST820 *
		 ST_DIR    = PA3;		//�����������                              GND    =   GND;   
		 ST_STEP   = PA5;		//�����ź����루��������Ч��                VDD    =    5V;   
		 ST_RESET  = 5V	 ;                                                2A     =   motor_A
		 NC							 ;                                                1A     =   motor_A
		 ST_MODE2  = PA7;		//����ϸ����                                1B     =   motor_B
		 ST_MODE1  = PC5;   //����ϸ����                                 2B     =   motor_B
		 ST_MODE0  = PB1;   //����ϸ����                                 GND   -----motor -   
		 ST_ENABLE = PB11;   //ʹ�ܿ������루�͵�ƽ��Ч��                 VMOT  -----motor + ( 8.2V~45V )
*/


//sbit ST_DIR    = P2^0;		//�����������
//sbit ST_STEP   = P2^1;		//�����ź����루��������Ч��
//sbit ST_MODE2  = P2^2;		//����ϸ����
//sbit ST_MODE1  = P2^3;   //����ϸ����
//sbit ST_MODE0  = P2^4;   //����ϸ����
//sbit ST_ENABLE = P2^5;   //ʹ�ܿ������루�͵�ƽ��Ч��



#define ST_ENABLE  PBout(11)
#define ST_DIR     PAout(3)
#define ST_DIR_    PAin(3)
#define ST_STEP    PAout(5)
#define ST_MODE0   PBout(1)
#define	ST_MODE1   PCout(5)
#define ST_MODE2   PAout(7)



//extern unsigned int ST_TurnMeter_1;
//extern unsigned int ST_TurnMeter_0;
extern u8 ST_Direction;		//����
extern u32 ST_TurnMeter_1,ST_TurnMeter_0 ;
extern u32 ST_lnterruptnum;
 


unsigned char ST820_motor(unsigned char ST_Direc,unsigned int num);
unsigned int ST_Compare_TurnMeter(void);

void ST820_Init(void);

#endif