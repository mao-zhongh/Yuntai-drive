#include "drv8825.h"


u8 Drv_Direction=0;     /* ����  */

/*
******************************************************
�������ܣ�drv8825�ܽų�ʼ��   PB9_CH4
�����βΣ�None
��������ֵ��1-->����PELCO_Dָ��   0-->��ִ�����
��ע���ܽŶ���ο�.h�ļ�
�汾��v0.0
******************************************************
*/

void DRV8825_Init(void)
{
	RCC->APB2ENR |= 0x7<<2;// A B C ��ʹ��

	GPIOA->CRH &= ~(0XF<<16); // PA12 ͨ��������� 10MHZ
	GPIOA->CRH |= 0X1<<16;	
	
	
	GPIOB->CRL &= ~(0xf<<28);//PB3 5 7 ͨ��������� 10MHZ
	GPIOB->CRL |= 0x01<<28;		
	GPIOB->CRL &= ~(0xf<<20);
  GPIOB->CRL |= 0x01<<20;		
	GPIOB->CRL &= ~(0xf<<12);
  GPIOB->CRL |= 0x01<<12;	


	GPIOC->CRH &= ~(0XF<<8);// pc10 pc12  ͨ��������� 10MHZ
	GPIOC->CRH |= 0x1<<8;
	GPIOC->CRH &= ~(0XF<<16);
	GPIOC->CRH |= 0x1<<16;
}


unsigned char DRV_Drive(int step)
{
	u8 state = IDLE;         /* �ϴ�״̬ */
	static u8 Drv_operation_dir=0; //���ڱ���_������ָ��ǰ���������з���    �ж���һ��������һ�����Ƿ��ͻ	
	Toggle_aisle = 0;    TIM4_Tint_PWM();  /* PWMͨ����ʼ�� */

	motor_disturb=1;       /*�����Ƿ�����ָ��*/
	ENABLE =0;   				 	 /*ʹ������*/

  /* �жϷ��� */	
	(step > 0) ? (DIR = 1) : (DIR = 0); 
	step = abs(step);					//abs����:ȡ����ֵ; abs��-123��= 123��

	/* תΪ����״̬ ��spta������� */						
//	drv_spta.step_state = IDLE;
//	Speed_Decision(); 
//	drv_spta.over = 0;

	Start_Speedup_Compute(step);
	if( Continue_disturb )
	{
		Continue_disturb = 0;         //����Ӧ��־		
		/*ֻ���յ��ж�ָ��Ž���*/
		if( DIR == Drv_operation_dir )/* �����з��� == ָ��ǰ���з��� */
		{
			LED_1 =! LED_1 ;
			drv_spta.step_arr = drv_spta.last_arr;//����ָ��ǰ�ٶ�����
		}
		else /* ��ͬ����,���»��� */
		{
			drv_spta.steps_taken = 0;             //�������еĲ���	���ֽ���ָ�������			
		}
	}



	TIM4->CR1 |= (0x1<<0);//��ʼ������ 		
	
		/* �ó�������ִ�� */	
		while( !drv_spta.over ) 
		{
			if(!restart)
			{/*   ����λ	                                                                 ����λ*/
				if( (DRV_Compare_TurnMeter() >= drv_scope && drv_up != (DRV_DIR_&0x01) ) || ( DRV_Compare_TurnMeter() == drv_up && drv_down != (DRV_DIR_&0x01)  ) )
				{
					goto DRV_out;
				}				
			}			

			Speed_Decision();
//			Delay_us(13);//1/32
			if( Continue_disturb )    //��ͻȻ����ָ�����˳��������				
			{	//Delay_ms(10);	

				TIM4->CR1 &= ~(0x1<<0);			  //ʧ�ܼ�����:	

//				drv_spta.steps_taken = 0;     //�������еĲ���	���ֽ���ָ�������
				
				/* ��¼�˳�ʱ���з���,�����´�ͬ����ʱ�ٶ��޷�����^_^ */				
				Drv_operation_dir = DIR;
				/* ��¼�˳�ʱARR��ֵ,�����´�ͬ����ʱ�ٶ��޷�����^_^ */
				drv_spta.last_arr = drv_spta.step_arr;
				ENABLE = 1;								/* ����ָֹͣ��,���� */ 
				return 1;
			}                                                                                                    //  1        0  
		
		}

//LED_1 =1;
DRV_out:
//	printf("TurnMeter = %d\r\n  ",drv_spta.TurnMeter);


	/* תΪ����״̬ ��spta������� */						
	drv_spta.step_state = IDLE;
	Speed_Decision(); 
	drv_spta.over = 0;

	STMFLASH_ErasePage(FLASH_Presetting);//����		//Erase current sector  ������	
	Write_EEPROM(Drv_Direction,FLASH_Presetting_DRV,DRV_Compare_TurnMeter());
	Write_EEPROM(ST_Direction,FLASH_Presetting_ST,ST_Compare_TurnMeter());	
	FLASH_Lock();																			//����		
	motor_disturb = 0;				//�����������	
	Delay_ms(50);	


	return 0;
}


/*
******************************************************
�������ܣ�drv8825 ->�����˶������жϲ������������λ�� 
�����βΣ�None
��������ֵ�����ص�ǰλ��ֵ
��ע��1# ������ drv_spta.TurnMeter ֻ���� (0~�����������) Ϊ������
      2# ����ֵΪ�� Drv_Direction ���� step ��ԭ��
�汾��v0.0
******************************************************
*/
int DRV_Compare_TurnMeter(void)
{
	int32_t step =0;
	step = drv_spta.TurnMeter;

	if(step == drv_up)       /* ������λ�����ز��� */
	{
		Drv_Direction = 1;				 //����
		return 0;
	}
	else if(step >= drv_down)/* ����ִ�����²���,�򷵻����ϲ���ָ����� */
	{
		Drv_Direction = 0;				 //����
	//	step = (~step+1);
		return step;
	}
	

}

