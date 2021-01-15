#include "st820.h"

//u32 ST_TurnMeter_1 = 0,ST_TurnMeter_0 = 0;
//u32 ST_lnterruptnum = 0;
u8 ST_Direction=0;		//����

/*
******************************************************
�������ܣ�ST�ܽų�ʼ��  
�����βΣ�None
��������ֵ��None
��ע��APB2---C10 C11 C12    B5 B6
���ߣ�hai 
���ڣ�
�汾��v0.0
******************************************************
*/
void ST820_Init(void)
{
	RCC->APB2ENR |= 0x7<<2;// A B C ��ʹ��
	
	GPIOA->CRL &= ~(0XF<<12);// PA3 PA5 PA7ͨ��������� 10MHZ
	GPIOA->CRL &= ~(0XF<<20);
	GPIOA->CRL |= 0X1<<12;
	GPIOA->CRL |= 0X1<<20;
	GPIOA->CRL &= ~(0XF<<28);
	GPIOA->CRL |= 0X1<<28;
	
	GPIOB->CRL &= ~(0xf<<4);// PB1 PB11 ͨ��������� 10MHZ
	GPIOB->CRH &= ~(0xf<<12);
	GPIOB->CRL |= 0X1<<4;
	GPIOB->CRH |= 0X1<<12;
	
	GPIOC->CRL &= ~(0XF<<20);//PC5 			ͨ��������� 10MHZ
	GPIOC->CRL |= 0X1<<20;
	
	
}



/*
****************************************
��������    ��ST820 �������ٲ������30BYJ-26��������
��������    ��int step �ƶ��Ľ������� -->( �����������ж� �ƶ�����0��˳ʱ��    1����ʱ�� )
��������ֵ  ��1-->����PELCO_Dָ��   0-->��ִ����� 
��ע        ���ܽŶ���ο�.h�ļ�
�汾        ��v0.0
****************************************
*/
unsigned char ST820_Drive( int step )
{
	static u8 ST_operation_dir=0;
	Toggle_aisle = 1;    TIM4_Tint_PWM();  /* PWMͨ����ʼ�� */
	
	motor_disturb=1;        /*�����Ƿ�����ָ��*/
	ST_ENABLE = 1;    			/*ʹ������*/

  /* �жϷ��� */	
	(step > 0) ? (ST_DIR = 1) : (ST_DIR = 0); 
	step = abs(step);					//abs����:ȡ����ֵ; abs��-123��= 123��

/*����ǰ����Step������ټ����ٵĲ�����*/	
	Start_Speedup_Compute(step);
//	LED_1 =! LED_1 ;
	
	if(Continue_disturb)
	{
		Continue_disturb = 0;         //����Ӧ��־
		if( ST_DIR == ST_operation_dir )/* �����з��� == ָ��ǰ���з��� */
		{

			LED_1 =! LED_1 ;
			st_spta.step_arr = 120;//st_spta.last_arr;//����ָ��ǰ�ٶ�����
		}
		else 
		{
			st_spta.steps_taken = 0;     //�������еĲ���	���ֽ���ָ�������			
		}		
	}
	TIM4->CR1 |= (0x1<<0);//��ʼ������ 		


	while( !st_spta.over )
	{		
      /* ���й����м���Ƿ�������λ */  
			if(ST_Compare_TurnMeter() >= st_scope &&  ST_Direction != (ST_DIR_&0X01)  ) //&& ST_Direction == st_left) || (ST_Compare_TurnMeter() >= st_scope && ST_Direction == st_right) )
			{
				goto ST_out;
			}
			Speed_Decision();			
			if( Continue_disturb )         //��ͻȻ����ָ�����˳��������
			{
				TIM4->CR1 &= ~(0x1<<0);			  //ʧ�ܼ�����:					
//Continue_disturb = 0;         //����Ӧ��־			

				/* תΪ����״̬ ��spta������� */						
				st_spta.steps_taken = 0;     //�����еĲ���		
				/* ��¼�˳�ʱ���з���,�����´�ͬ����ʱ�ٶ��޷�����^_^ */				
				ST_operation_dir = ST_DIR;
				/* ��¼�˳�ʱARR��ֵ,�����´�ͬ����ʱ�ٶ��޷�����^_^ */
				st_spta.last_arr = st_spta.step_arr;


				
				ST_ENABLE = 0;    			     //ʧ������				
				//Delay2ms();
				return 1;
			}
	}
ST_out:	

	/* תΪ����״̬ ��spta������� */						
	st_spta.step_state = IDLE;
	Speed_Decision(); 
	st_spta.over = 0;


	/*ÿ��������ɼ�¼��ǰλ��*/
	STMFLASH_ErasePage(FLASH_Presetting);//����		//Erase current sector  ������	
	Write_EEPROM(Drv_Direction,FLASH_Presetting_DRV,DRV_Compare_TurnMeter());
	Write_EEPROM(ST_Direction,FLASH_Presetting_ST,ST_Compare_TurnMeter());	
	FLASH_Lock();																			//����
	
  motor_disturb=0;				//�����������		
	Delay_us(50);		
	return 0;
}


/*
******************************************************
�������ܣ�ST820 ->�����˶������жϲ������������λ�� 
�����βΣ�None
��������ֵ�����ص�ǰλ��ֵ
��ע��1# st_spta.TurnMeter ����˳ʱ_Ϊ������;   ���ҡ���ʱ_Ϊ������; ����ȽϷ��س���ʱת����������
      2# ����ֵΪ�� ST_Direction ���� Step Ϊԭ��

�汾��v0.0
******************************************************
*/
int ST_Compare_TurnMeter(void)
{
	int32_t Step = 0;
	Step = st_spta.TurnMeter;
	
		if( Step > 0 )         /* ��˳ */
		{
			ST_Direction=0;		
			return Step;
		}
		else              		 /* ���� */
		{
			ST_Direction=1;		
			Step = abs(Step);    /* ȡ����ֵ */   
			return Step;
		}
}




