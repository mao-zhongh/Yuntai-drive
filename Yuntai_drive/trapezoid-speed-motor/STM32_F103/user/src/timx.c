#include "timx.h"

//ϵͳ�Ӽ��ٲ���
speedRampData srd;
//��¼���������λ��
int stepPosition = 0;
//ϵͳ���������״̬
struct GLOBAL_FLAGS status = {FALSE, FALSE,TRUE};
/*
******************************************************
�������ܣ����PWM����TIM4��ʼ��
�����βΣ�None
��������ֵ��None
��ע��APB1---TIM4    72Mhz     
			PB8_CH3  PB9_CH4                  5us
���ߣ�hai 
���ڣ�
�汾��v0.0
******************************************************
*/
void TIM4_Tint_(u32 psc,u32 arr) //
{
	RCC->APB2ENR |= 0x1<<3;//��B��ʱ��;
	RCC->APB1ENR |= 0X1<<2;//��λTIM4��ʱ��:

	GPIOB->CRH &= ~(0Xf<<0);
	GPIOB->CRH |= 0XA<<0;//PB8�������
	
//	TIM4->SMCR &= ~(0X7<<0);//�ڲ�ʱ������
//	TIM4->SMCR &= ~(0x1<<14);
	
	TIM4->CR1 =0;//����CR1�Ĵ���;
	TIM4->CR1 &=~(0X01<<3);//��ֹͣ����

	/*
	TIMx_ARR�Ĵ���û�л��壻
	ѭ������  SYSTICK*8     TIM*2
	���ض���ģʽ
	���������ϼ���
	*/
	TIM4->PSC =psc-1;//Ԥ����ֵ72000000/72=1000Khz
	TIM4->ARR =arr;//�Զ���װ��1us��һ�Σ����ڣ�1*1000=1ms
	TIM4->CNT=0;
	
	/*CH3*/
	TIM4->CCR3 = arr>>1;//�Ƚ�ֵ  PB8_CH3
	TIM4->CCMR2 &= ~(0x3<<0);//CC3ͨ��������Ϊ���
	TIM4->CCMR2 &= ~(0X1<<7);//����ETRF�����Ӱ��;
	TIM4->CCMR2 &= ~(0x7<<4);
	TIM4->CCMR2 |= (0x06<<4);//PWMģʽ1;  110 
	
	
	TIM4->CCER &= ~(0x1<<9);//����Ч;
	TIM4->CCER |= 0x1<<8;	//����CH3�ź����	
		
		
	//����TIM4�ж����ȼ���
	NVIC_SetPriority( TIM4_IRQn, 3 );
	NVIC_EnableIRQ ( TIM4_IRQn );//ʹ��NVIC��Ӧ	
	TIM4->DIER |= 0x1<<0;//��������ж�


	TIM4->CR1 &= ~(0x1<<0);//ʧ�ܼ�����;	
//	TIM4->CR1 |= (0x1<<0);//ʹ�ܼ�����;	

}
/**

  * @brief  �����˶������жϲ������������λ��

  * @param  inc �˶�����

  * @retval ��

  */
void MSD_StepCounter(signed char inc)
{
  //���ݷ����жϵ��λ��
  if(inc == CCW)
  {
    stepPosition--;
  }
  else
  {
    stepPosition++;
  }
}
/*****TIM4�жϷ�����*****/
void TIM4_IRQHandler(void)
{
	LED_1 =0;
  // ������һ����ʱ����
  unsigned int new_step_delay;
  // ���ٹ��������һ����ʱ.
  static int last_accel_delay;
  // �ƶ�����������
  static unsigned int step_count = 0;
  // ��¼new_step_delay�е������������һ������ľ���
  static signed int rest = 0;

if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
{
    /* Clear MSD_PULSE_TIM Capture Compare1 interrupt pending bit*/
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    TIM4->CCR4=srd.step_delay >> 1;//���ڵ�һ��   //10830>>1
    TIM4->ARR=srd.step_delay;
    //�����ֹ����������ֹͣ�˶�
    if(status.out_ena != TRUE)
    {
        srd.run_state = STOP;
    }
  switch(srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      TIM4->CCER &= ~(0x1<<8); //��ֹ���
      TIM_Cmd(TIM4, DISABLE);
      status.running = FALSE;
			srd.over = 1;
      break;

    case ACCEL:
      TIM4->CCER |= 1<<8; //ʹ�����
      MSD_StepCounter(srd.dir);//��¼�˶�����λ��
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));         //4332.022
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //����ǹ�Ӧ�ÿ�ʼ����
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        srd.run_state = DECEL;
      }
      //����Ƿ񵽴�����������ٶ�
      else if(new_step_delay <= srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
      TIM4->CCER |= 1<<8; //ʹ�����
      MSD_StepCounter(srd.dir);
      step_count++;
      new_step_delay = srd.min_delay;
      //����Ƿ���Ҫ��ʼ����
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        //�����һ�μ��ٵ���ʱ��Ϊ��ʼ���ٵ���ʱ
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      TIM4->CCER |= 1<<8; //ʹ�����
      MSD_StepCounter(srd.dir);
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) 
                       + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      //����Ƿ�Ϊ���һ��
      if(srd.accel_count >= 0){
        srd.run_state = STOP; 
      }
      break;
  }
  srd.step_delay = new_step_delay;
  }
}
 /*! \brief �Ը����Ĳ����ƶ��������
 *  ͨ��������ٵ�����ٶȣ��Ը����Ĳ�����ʼ����
 *  ������ٶȺͼ��ٶȺ�С������������ƶ���������û�ﵽ����ٶȾ�Ҫ��ʼ����
 *  \param step   �ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
 *  \param accel  ���ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec^2=1rad/sec^2
 *  \param decel  ���ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec^2=1rad/sec^2
 *  \param speed  ����ٶ�,���ȡֵΪ100��ʵ��ֵΪ100*0.01*rad/sec=1rad/sec
 */
void MSD_Move(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{//                     16320                 75                  75                    62
    //�ﵽ����ٶ�ʱ�Ĳ���.
    unsigned int max_s_lim;
    //���뿪ʼ���ٵĲ���(�����û���ٵ�������ٶ�ʱ)��
    unsigned int accel_lim;


    // ���õ��Ϊ���״̬
    //status.out_ena = TRUE;
    
    // ���ֻ�ƶ�һ��
    if(step == 1)
    {
        // ֻ�ƶ�һ��
        srd.accel_count = -1;
        // ����״̬
        srd.run_state = DECEL;
        // ����ʱ
        srd.step_delay = 1000;
        // ���õ��Ϊ����״̬
        status.running = TRUE;
        //���ö�ʱ����װֵ	
        TIM_SetAutoreload(TIM4 ,Pulse_width);
        //����ռ�ձ�Ϊ50%	
        TIM_SetCompare2(TIM4,Pulse_width>>1);
        //ʹ�ܶ�ʱ��	      
        TIM_Cmd(TIM4, ENABLE); 
     }
    // ������Ϊ����ƶ�
    else if(step != 0)
    {
    // ���ǵ��������û��ֲ�����ϸ�ļ��㼰�Ƶ�����

    // ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ��
    // min_delay = (alpha / tt)/ w
    srd.min_delay = A_T_x100 / speed;    //155.2258

    // ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.01rad/sec^2
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd.step_delay = (T1_FREQ_148 * sqrt(A_SQ / accel))/100;                      //10830.0567

    // ������ٲ�֮��ﵽ����ٶȵ�����
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel)/100);    //2662.529
    // ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0
    // ��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ�
    if(max_s_lim == 0)
    {
        max_s_lim = 1;
    }
 
    // ������ٲ�֮�����Ǳ��뿪ʼ����
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((long)step*decel) / (accel+decel);  //8160
    // ���Ǳ����������1�����ܲ��ܿ�ʼ����.
    if(accel_lim == 0)
    {
        accel_lim = 1;
    }
    // ʹ�������������ǿ��Լ������һ�ο�ʼ���ٵ�λ��
    //srd.decel_valΪ����
    if(accel_lim <= max_s_lim)
    {
        srd.decel_val = accel_lim - step;
    }
    else
    {
        srd.decel_val = -(long)(max_s_lim*accel/decel);   //2662.529
    }
    // ��ֻʣ��һ�����Ǳ������
    if(srd.decel_val == 0)
    {
        srd.decel_val = -1;
    }

    // ���㿪ʼ����ʱ�Ĳ���
    srd.decel_start = step + srd.decel_val;               //18982.529

    // �������ٶȺ��������ǾͲ���Ҫ���м����˶�
    if(srd.step_delay <= srd.min_delay)
    {
        srd.step_delay = srd.min_delay;
        srd.run_state = RUN;
    }
    else
		{
        srd.run_state = ACCEL;
    }

    // ��λ���ٶȼ���ֵ
    srd.accel_count = 0;
    status.running = TRUE;
    //���ö�ʱ����װֵ	
    TIM_SetAutoreload(TIM4,Pulse_width);
    //����ռ�ձ�Ϊ50%	
    TIM_SetCompare2(TIM4,Pulse_width>>1);
    //ʹ�ܶ�ʱ��	      
    TIM_Cmd(TIM4, ENABLE); 
    }
}






unsigned char DRV_Motor(signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
	ENABLE =0;

	// ���ݲ������������жϷ���
	if(step < 0)//��ʱ��
	{
			srd.dir = CCW;
			step = -step;
	}
	else//˳ʱ��
	{
			srd.dir = CW;
	}	
	DIR = srd.dir;
	/***********/
 
	MSD_Move(step,accel,decel,speed);
	
	
	/***********/

	TIM4->CR1 |= (0x1<<0);//��ʼ������ 	
		while(!srd.over)//400
		{




		}
		
	
DRV_out:
 	TIM4->CR1 &= ~(0x1<<0);//ֹͣ������ 
	ENABLE = 1;								//ʧ�ܲ������
	srd.over=0;

	Delay_ms(10);	

		return 0;
}


/*
		TIM4->CCR3 = arr_tim4;//�Ƚ�ֵ  PB8_CH3
		if(flag == 0)
		{
			i += 1;
			if(i>=3000)
			{
				i=0;
				arr_tim4 +=1;  		
				if(arr_tim4 >= 60) { flag =1; LED_1 =1;	DIR =1; }//
			}
		}else if(flag==1)
		{
			i +=1;
			if(i >= 3000)
			{
				i=0;
				arr_tim4 -=1;  		
				if(arr_tim4 <= 10) { flag=0; LED_1 =0;	 DIR=0; }
			}
		}
*/