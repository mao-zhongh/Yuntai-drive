#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"
#include "main.h"
#include <math.h>

#define CW  0
#define CCW 1

#define TRUE 1
#define FALSE 0

#define Pulse_width 20           //重装值

//系统状态
struct GLOBAL_FLAGS {
  //当步进电机正在运行时，值为1
  unsigned char running:1;
  //当串口接收到数据时，值为1
  unsigned char cmd:1;
  //当驱动器正常输出时,值为1
  unsigned char out_ena:1;
};

extern struct GLOBAL_FLAGS status;
extern int stepPosition;

#define T1_FREQ 1000000     //定时器频率         // 分频后clk
#define FSPR    4080         //步进电机单圈步数
#define SPR     (FSPR*32)  //32细分的步数
// 数学常数。 用于MSD_Move函数的简化计算
#define ALPHA (2*3.14159/SPR)                    // 2*pi/ (细分->单圈步数)              32 _0.00009624969
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100               32 _9624.0035
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676  32 _6760
#define A_SQ (long)(ALPHA*2*10000000000)         //                                     32 _1924993.8
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
    
//速度曲线状态
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

typedef struct {
  //电机运行状态
  unsigned char run_state : 3;
  //电机运行方向
  unsigned char dir : 1;
  //下一个脉冲延时周期，启动时为加速度速率
  unsigned int step_delay;
  //开始减速的位置
  unsigned int decel_start;
  //减速距离
  signed int decel_val;
  //最小延时（即最大速度）
  signed int min_delay;
  //加速或者减速计数器
  signed int accel_count;
	
	unsigned char over; 
} speedRampData;




unsigned char DRV_Motor(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);
void MSD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed);
void TIM4_Tint_(u32 psc,u32 arr);


#endif
