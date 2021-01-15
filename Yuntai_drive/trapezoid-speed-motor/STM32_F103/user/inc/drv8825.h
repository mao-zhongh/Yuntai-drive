#ifndef _DRV8825_H_
#define _DRV8825_H_

/*
																						* DRV8825 *
		 DIR    = P1^5;		//�����������                                  GND    =   GND;   
		 STEP   = P1^4;		//�����ź����루��������Ч��                    FAULT   
		 SLEEP  = 5V;                                                     2A     =   motor_A
		 RESET  = 5V;                                                     1A     =   motor_A
		 MODE2  = P1^3;   //����ϸ����                                    1B     =   motor_B
		 MODE1  = P1^2;   //����ϸ����                                    2B     =   motor_B
		 MODE0  = P1^1;		//����ϸ����                                    GND   -----motor -   
		 ENABLE = P1^0;   //ʹ�ܿ������루�͵�ƽ��Ч��                    VMOT  -----motor + ( 8.2V~45V )
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


extern u32 Drv_lnterruptnum;
extern u32 TurnMeter_1,TurnMeter_0;
extern u8 Drv_Direction;


void DRV8825_Init(void);


/*���ص�ǰλ��ֵ*/
unsigned int Compare_TurnMeter(void);
#endif

