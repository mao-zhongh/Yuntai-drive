#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"
#include "main.h"


#define TIMx_SCLK_APB1          72000000   //  ֻ�Ը�TIMxʹ��;

#define ST_PWM_ARR                  250 	 //  ��ռ�ձ�    /* st_14usһ��_ [5S /st_scope /0.013 = 736.472]/  */  7us / 0.013  == 538.461     
#define ST_PWM_PSC                  7.2    	 //  72000Mhz  0.013us
#define DRV_PWM_ARR                 1000	 //6100	 //  ��ռ�ձ�    /* drv_76usһ��_ [5S/drv_scope/0.013 = 5891.77]/  */  [70us / 0.013  == 5384.6 ]    [100us / 0.013 == 7692.30]
#define DRV_PWM_PSC                 7.2    	 //  72000Mhz  0.013us

/*����ʱ�������ռ�ձ�*/
#define PWM_cut(step)  step/(drv_scope/PWM_ARR)   //���� least_drvscope���� ����ר�ã�����ҲҪ�м��ٵĹ���,ֻ���ٶ������ƣ� step/214

#define ST_Max_speed                40          /* �ٶ�����ԽСԽ�� */
//#define ST_interval_time            800        /* �����Խ��Խƽ˳ */ 
#define DRV_Max_speed               100          /* �ٶ�����ԽСԽ�� */
//#define DRV_interval_time           100        /* �����Խ��Խƽ˳ */ 


//TIMx_SCLK_APB1/PWM_PSC 


void Systick_Init(void); //1MS�ж�
void Delay_ms(u32 num); //ms��ʱ
void Delay_us(u32 num);


void TIM4_Tint_PWM(void); //PWM   
void Start_Speedup_Compute( int step ); 	 /* �����Ӽ��ټ��㸳ֵ */
void Speed_Decision( void ); 							 /* ���ݵ����״̬����״̬ת���Լ��������� */	


void TIM6_Init_(u32 arr,u32 psc);				//ST
void TIM5_Init(u32 arr,u32 psc); 				//DRV

/* SPTA����״̬ */
typedef enum
{
  IDLE = 0U,         /* ֹͣ */
  ACCELERATING,      /* ���� */
  UNIFORM,           /* ���� */
  DECELERATING,      /* ���� */
}STEP_STATE_TypeDef;

/* SPTA��ز����ṹ�� */
typedef struct 
{/***********/
  uint32_t steps_required;       //�����в���
  uint32_t steps_taken;          //�Ѿ����еĲ���
	int32_t TurnMeter;         		 //��¼˳�����_�����жϷ���

	uint32_t begin;                //���� [ST->32640  1/256__522240/4/4 22.5��]
	uint32_t finish;               //��ͣ 
	
  uint32_t steps_compare;        //���㲽���Ƚ�,���ٳ�������
	
	uint16_t step_arr; 
	uint16_t last_arr;             //����ϴ�ARRֵ, 
  int32_t step_speed;       		 //�ۼƵ�ǰ���ٶȣ�ռ�ձȣ�
	
//  uint32_t steps_middle;        //�ܲ������е㣬��������ʱ�����ò�����Ҫ����
  uint32_t steps_add;         	//���ٽ׶εĲ���
  uint32_t steps_all;           //���ٽ׶εĲ���
  uint32_t steps_cut;           //���ٽ׶εĲ���
	
	
	uint16_t steps_interval;      //�����   ��������75us �� * Sustain_interval = 225us
  uint8_t  over;                //���ָ���־

  uint32_t speed_lim;           //���������ٶ����ƣ������ٶ�С���ٶ�����ʱΪ�������������ڵ����ٶ�����ʱΪ��������

  STEP_STATE_TypeDef step_state;     //�������״̬ö��
}SPTAData_Typedef;


extern unsigned char Toggle_aisle;  /* �л����ͨ��  1Ϊ:ST  0Ϊ:DRV */
extern SPTAData_Typedef st_spta;
extern SPTAData_Typedef drv_spta;





#endif
