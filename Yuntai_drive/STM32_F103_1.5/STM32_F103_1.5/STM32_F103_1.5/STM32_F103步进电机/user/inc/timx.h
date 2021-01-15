#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"
#include "main.h"


#define TIMx_SCLK_APB1          72000000   //  只对付TIMx使用;

#define ST_PWM_ARR                  250 	 //  总占空比    /* st_14us一拍_ [5S /st_scope /0.013 = 736.472]/  */  7us / 0.013  == 538.461     
#define ST_PWM_PSC                  7.2    	 //  72000Mhz  0.013us
#define DRV_PWM_ARR                 1000	 //6100	 //  总占空比    /* drv_76us一拍_ [5S/drv_scope/0.013 = 5891.77]/  */  [70us / 0.013  == 5384.6 ]    [100us / 0.013 == 7692.30]
#define DRV_PWM_PSC                 7.2    	 //  72000Mhz  0.013us

/*减速时所需最大占空比*/
#define PWM_cut(step)  step/(drv_scope/PWM_ARR)   //低于 least_drvscope距离 减速专用（减速也要有加速的过程,只是速度有限制） step/214

#define ST_Max_speed                40          /* 速度限制越小越慢 */
//#define ST_interval_time            800        /* 间隔数越大越平顺 */ 
#define DRV_Max_speed               100          /* 速度限制越小越慢 */
//#define DRV_interval_time           100        /* 间隔数越大越平顺 */ 


//TIMx_SCLK_APB1/PWM_PSC 


void Systick_Init(void); //1MS中断
void Delay_ms(u32 num); //ms延时
void Delay_us(u32 num);


void TIM4_Tint_PWM(void); //PWM   
void Start_Speedup_Compute( int step ); 	 /* 启动加减速计算赋值 */
void Speed_Decision( void ); 							 /* 根据电机的状态进行状态转换以及参数计算 */	


void TIM6_Init_(u32 arr,u32 psc);				//ST
void TIM5_Init(u32 arr,u32 psc); 				//DRV

/* SPTA运行状态 */
typedef enum
{
  IDLE = 0U,         /* 停止 */
  ACCELERATING,      /* 加速 */
  UNIFORM,           /* 匀速 */
  DECELERATING,      /* 减速 */
}STEP_STATE_TypeDef;

/* SPTA相关参数结构体 */
typedef struct 
{/***********/
  uint32_t steps_required;       //总运行步数
  uint32_t steps_taken;          //已经运行的步数
	int32_t TurnMeter;         		 //记录顺逆节拍_依据判断方向

	uint32_t begin;                //缓起 [ST->32640  1/256__522240/4/4 22.5°]
	uint32_t finish;               //缓停 
	
  uint32_t steps_compare;        //方便步数比较,减少程序运算
	
	uint16_t step_arr; 
	uint16_t last_arr;             //存放上次ARR值, 
  int32_t step_speed;       		 //累计当前加速度（占空比）
	
//  uint32_t steps_middle;        //总步数的中点，三角轮廓时超过该步数就要减速
  uint32_t steps_add;         	//加速阶段的步数
  uint32_t steps_all;           //均速阶段的步数
  uint32_t steps_cut;           //减速阶段的步数
	
	
	uint16_t steps_interval;      //间隔数   假设周期75us 则 * Sustain_interval = 225us
  uint8_t  over;                //完成指令标志

  uint32_t speed_lim;           //电机的最大速度限制，运行速度小于速度限制时为三角轮廓，大于等于速度限制时为梯形轮廓

  STEP_STATE_TypeDef step_state;     //电机运行状态枚举
}SPTAData_Typedef;


extern unsigned char Toggle_aisle;  /* 切换输出通道  1为:ST  0为:DRV */
extern SPTAData_Typedef st_spta;
extern SPTAData_Typedef drv_spta;





#endif
