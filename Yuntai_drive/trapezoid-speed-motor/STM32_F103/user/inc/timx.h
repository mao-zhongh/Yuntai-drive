#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"
#include "main.h"
#include <math.h>

#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20           //��װֵ

//ϵͳ״̬
struct GLOBAL_FLAGS {
  //�����������������ʱ��ֵΪ1
  unsigned char running:1;
  //�����ڽ��յ�����ʱ��ֵΪ1
  unsigned char cmd:1;
  //���������������ʱ,ֵΪ1
  unsigned char out_ena:1;
};

extern struct GLOBAL_FLAGS status;
extern int stepPosition;

#define T1_FREQ 1000000     //��ʱ��Ƶ��         // ��Ƶ��clk
#define FSPR    4080         //���������Ȧ����
#define SPR     (FSPR*32)  //32ϸ�ֵĲ���
// ��ѧ������ ����MSD_Move�����ļ򻯼���
#define ALPHA (2*3.14159/SPR)                    // 2*pi/ (ϸ��->��Ȧ����)              32 _0.00009624969
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100               32 _9624.0035
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676  32 _6760
#define A_SQ (long)(ALPHA*2*10000000000)         //                                     32 _1924993.8
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//�ٶ�����״̬
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

typedef struct {
  //�������״̬
  unsigned char run_state : 3;
  //������з���
  unsigned char dir : 1;
  //��һ��������ʱ���ڣ�����ʱΪ���ٶ�����
  unsigned int step_delay;
  //��ʼ���ٵ�λ��
  unsigned int decel_start;
  //���پ���
  signed int decel_val;
  //��С��ʱ��������ٶȣ�
  signed int min_delay;
  //���ٻ��߼��ټ�����
  signed int accel_count;
	
	unsigned char over; 
} speedRampData;




unsigned char DRV_Motor(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);
void MSD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);
void TIM4_Tint_(u32 psc,u32 arr);


#endif
