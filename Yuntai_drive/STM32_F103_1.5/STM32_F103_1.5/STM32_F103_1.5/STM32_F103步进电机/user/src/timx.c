#include "timx.h"

SPTAData_Typedef drv_spta = {0};
SPTAData_Typedef st_spta  = {0};
unsigned char Toggle_aisle =0;  /* �л����ͨ��  1Ϊ:ST  0Ϊ:DRV */

uint32_t runtime = 0;


/*
******************************************************
�������ܣ����PWM����TIM4��ʼ��
�����βΣ�_u16 dat3 ͨ��3ռ�ձ�����  ,  _u16 dat4   ͨ��4ռ�ձ�����
��������ֵ��None
��ע��APB1---TIM4    72Mhz     
			PB8_CH3  PB9_CH4                  5us
���ߣ�hai 
�汾��v0.0
******************************************************
*/
void TIM4_Tint_PWM(void) //
{
	RCC->APB2ENR |= 0x1<<3;//��B��ʱ��;
	RCC->APB1ENR |= 0X1<<2;//��λTIM4��ʱ��:

	GPIOB->CRH &= ~(0Xf<<0);
	GPIOB->CRH |= 0XA<<0;//PB8�������
	
	GPIOB->CRH &= ~(0Xf<<4);
	GPIOB->CRH |= 0XA<<4;//PB9�����������
	
	TIM4->SMCR &= ~(0x7<<0);//�ߵ�ƽ/��������Ч
	TIM4->SMCR &= ~(0X1<<14);//��ֹ�ⲿʱ��ģʽ
	
	TIM4->CR1 =0;//����CR1�Ĵ���; 
	TIM4->CR1 |= 0X1<<2;//���������/�����������(UEV)�¼�
	TIM4->CR1 |= 0x1<<7;//TIMx_ARR�Ĵ�����װ�뻺������
//	TIM4->CR1 |= 0X2<<8;//����CK_INT 72Mhz / 4 = 18Mhz
	/*
	ѭ������    TIM*2
	���ض���ģʽ
	���������ϼ���
	��ֹͣ����  5.5384615Mhz
	*/
	
	TIM4->CNT=0;	
	if(Toggle_aisle)
	{
		TIM4->PSC =ST_PWM_PSC-1;//Ԥ����ֵ72000000/72=1000Khz   72000000/18=4000Khz0.25us  72000000/14.4=5000Khz 0.2us
		TIM4->ARR =ST_PWM_ARR;//�Զ���װ��1us��һ�Σ����ڣ�1*1000=1ms   5Mhz 70us����	
		/*CH4*/
		TIM4->CCR4 = 0;//�Ƚ�ֵ  PB9_CH4
		TIM4->CCMR2 &= ~(0x3<<8);//CC4ͨ��������Ϊ���
		TIM4->CCMR2 &= ~(0x1<<15);//����ETRF�����Ӱ��;
		TIM4->CCMR2 &= ~(0x7<<12);
		TIM4->CCMR2 |= (0x06<<12);//PWMģʽ1;һ��TIMx_CNT<TIMx_CCR4ʱͨ��4Ϊ��Ч��ƽ
		TIM4->CCER &= ~(0x1<<13);//����Ч;
		TIM4->CCER |= 0x1<<12;//����CH4�ź����	
	}
	else 
	{
		TIM4->PSC =DRV_PWM_PSC-1;//Ԥ����ֵ72000000/72=1000Khz   72000000/18=4000Khz0.25us  72000000/14.4=5000Khz 0.2us
		TIM4->ARR =DRV_PWM_ARR;//�Զ���װ��1us��һ�Σ����ڣ�1*1000=1ms   5Mhz 70us����		
		/*CH3*/
		TIM4->CCR3 = 1;//�Ƚ�ֵ  PB8_CH3  --->��Чռ�ձ���
		TIM4->CCMR2 &= ~(0x3<<0);//CC3ͨ��������Ϊ���
		TIM4->CCMR2 &= ~(0x7<<4);//����ETRF�����Ӱ��;
		TIM4->CCMR2 |= (0x06<<4);//PWMģʽ1;  110 
		TIM4->CCER &= ~(0x1<<9);//����Ч;
		TIM4->CCER |= 0x1<<8;	//����CH3�ź����		
	}



	//����TIM4�ж����ȼ���
	NVIC_SetPriority( TIM4_IRQn, 3 );
	NVIC_EnableIRQ ( TIM4_IRQn );//ʹ��NVIC��Ӧ	
	TIM4->DIER |= 0x1<<0;//��������ж�

	TIM4->CR1 &= ~(0x1<<0);//ʧ�ܼ�����:		
	
}


/******��������ͬ,ÿ���һ�������PWM��һ������*******/
void TIM4_IRQHandler(void)
{
	static u32 DRV_i=0,ST_i=0;
	if(TIM4->SR & 0x1<<0)
	{
		TIM4->SR &= ~(0x1<<0);
		
		if(Toggle_aisle)  /* ST */
		{
			st_spta.steps_taken++;
			TIM4->CCR4 = st_spta.step_speed;//�Ƚ�ֵ  PB8_CH3		

			if     (ST_DIR_&0x01)    { 		  st_spta.TurnMeter++;     }						//��¼˳ʱ��ÿһ����
			else						         {      st_spta.TurnMeter--;     }						//��¼��ʱ��ÿһ����		

			ST_i++;
//			if((st_spta.step_state == ACCELERATING) && (ST_i >= 50))    //����ʱ�ݼ���С����ʱ��
//			{				

//				ST_i=0;
//				if(st_spta.step_arr >156 ) { st_spta.step_arr -= 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //���������/����ɲ��������¼�		
//			}
//			else if((st_spta.step_state == DECELERATING) && (ST_i >= 50))//����ʱ���������ʱ��
//			{

//				ST_i=0;
//				if(st_spta.step_arr < 310) { st_spta.step_arr += 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //���������/����ɲ��������¼�		
//			}

			if((st_spta.step_state == ACCELERATING) && (st_spta.steps_taken <=st_spta.begin) && (ST_i >= 100))    //����ʱ�ݼ���С����ʱ��
			{				

				ST_i=0;
				if(st_spta.step_arr >100 ) { st_spta.step_arr -= 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //���������/����ɲ��������¼�		
			}
			else if((st_spta.step_state == DECELERATING) && ( st_spta.steps_taken >= st_spta.finish ) && (ST_i >= 35))//����ʱ���������ʱ��
			{

				ST_i=0;
				if(st_spta.step_arr < 200) { st_spta.step_arr += 1 ;	TIM4->ARR = st_spta.step_arr;	 }                //���������/����ɲ��������¼�		
			}			
		}

		
	
		else  /* DRV */
		{
			drv_spta.steps_taken++;   //�����еĲ���
			TIM4->CCR3 = drv_spta.step_speed;//�Ƚ�ֵ  PB8_CH3		
		
			if     (DRV_DIR_&0x01)    { 		 drv_spta.TurnMeter++;     }						//��¼˳ʱ��ÿһ����
			else						      		{      drv_spta.TurnMeter--;     }						//��¼��ʱ��ÿһ����	

			DRV_i++;
			if((drv_spta.step_state == ACCELERATING) && (DRV_i >= 35))    //����ʱ�ݼ���С����ʱ��
			{				

				DRV_i=0;
				if(drv_spta.step_arr >720 ) { drv_spta.step_arr -= 1 ;	TIM4->ARR = drv_spta.step_arr;	 }                //���������/����ɲ��������¼�		
			}
			else if((drv_spta.step_state == DECELERATING) && (DRV_i >= 35))//����ʱ���������ʱ��
			{

				DRV_i=0;
				if(drv_spta.step_arr < 1000) { drv_spta.step_arr += 1 ;	TIM4->ARR = drv_spta.step_arr;	 }                //���������/����ɲ��������¼�		
			}
		}
	}
}	

/*
******************************************************
�������ܣ����ݵ����״̬����״̬ת���Լ���������
�����βΣ� None 
��������ֵ��None
��ע�� Toggle_aisle ���л�ST��DRV�ĳ�ʼ��
���ߣ�hai 
�汾��v0.0
******************************************************
*/	
void Speed_Decision( void )
{
/****************************************************ST״̬����**************************************************************/
	if(Toggle_aisle)
	{
		switch (st_spta.step_state)
		{
			/* SPTA����״̬ */
			case ACCELERATING:
				
						if(st_spta.steps_all != 0)
						{
							/* �ﵽ�������貽�� */			
							if( st_spta.steps_taken >= st_spta.steps_add)   
							{ 
								TIM4->ARR = 100;
								/*Ԥ�Ƶ���������ٲ��� = �Ѽ��ٵĲ��� + ����ٵĲ���*/
								st_spta.steps_compare = st_spta.steps_taken + st_spta.steps_all;         
								/* תΪ����״̬ */					
								st_spta.step_state = UNIFORM;			
								break;
							}							
						}
						else//
						{
							/* תΪ����״̬ */						
							st_spta.step_state = IDLE;								
							/*����״̬ */
						//	st_spta.step_state = DECELERATING;						
						}
						
							/* û��������ٶ����� && �պù�һ����ƽ�������� */
//						if( (st_spta.step_speed <= st_spta.speed_lim) && ( (st_spta.steps_taken % st_spta.steps_interval) == 0) )							 				
						if( (st_spta.step_speed <= st_spta.step_arr) && ( (st_spta.steps_taken % st_spta.steps_interval) == 0) )	
						{
						 /* ���ٽ׶Σ��ٶ��ۼ���������ٶ�ֵ���� */
							st_spta.step_speed++ ;		
							Delay_ms(1);	
						}
						break;							

					
			/* SPTA����״̬ */				
			case UNIFORM:	
							/* ��ǰ�������경��  >= �Ѽ��ٲ��� + ����ٲ��� */
						if( st_spta.steps_taken >= st_spta.steps_compare ) 
						{	
							st_spta.step_state = DECELERATING;
							
							//printf("steps_cut == %d\r\n",drv_spta.steps_cut);	
						}
					 break;						
			

			
			/* SPTA����״̬ */				
			case DECELERATING:
	//printf("steps_taken == %d\r\n",drv_spta.steps_taken);			

						if( st_spta.steps_taken >= st_spta.steps_required )/* ��ǰ���� >= �ܲ��� #�ﵽ�������貽��*/
						{	
	//printf("�ܲ���-��ǰ���� '0'  -> %d\r\n",(drv_spta.steps_required-drv_spta.steps_taken) );

							/* תΪ����״̬ */						
							st_spta.step_state = IDLE;					
							break;
						}		
						/* û����С�ٶ� && һ����ƽ״̬���� drv_spta.steps_interval ������ */					
						else if( (	st_spta.step_speed >= 2 ) && ((st_spta.steps_taken % st_spta.steps_interval) == 0) ) 
						{
						 /* ���ٽ׶Σ��ٶ��ۼ���������ٶ�ֵ�ݼ� */
							st_spta.step_speed-- ;		

	//printf("step_speed == %d\r\n",drv_spta.step_speed);							
							Delay_ms(1);		
						}				
								
					 break;		
			
			
			/* SPTA����״̬ */		
			case IDLE:
		
						TIM4->CR1 &= ~(0x1<<0);       //ʧ�ܼ�����;				
						ST_ENABLE = 0;								//ʧ�ܲ������
	//printf("\r\nST�ѿ���״̬,�ô��г��ѹ���steps_taken -> %d\r\n",st_spta.steps_taken);
	//printf("�ѿ���״̬,��ǰλ����    TurnMeter   -> %d\r\n",st_spta.TurnMeter);	
						/* ����spta������� */
						st_spta.steps_taken = 0;     //�����еĲ���
						st_spta.step_speed = 0;   //��ǰ���ٶȣ�ռ�ձȣ�
						st_spta.steps_add = 0;      	//����ٵĲ���  
						st_spta.steps_all = 0;       //����ٵĲ���
						st_spta.steps_cut = 0;       //����ٵĲ���	
						st_spta.speed_lim = 0;			  //�����ٶ�����	
						st_spta.steps_interval = 0;  //�����
						st_spta.steps_required = 0;  //�ܲ���
						st_spta.over =1;
						st_spta.steps_compare=0;
					//	Delay_ms(50);	

					 break;			

			default:           
							break;		
			
		}			
	}
/****************************************************DRV״̬����**************************************************************/
	else
	{
		switch (drv_spta.step_state)
		{
			/* SPTA����״̬ */
			case ACCELERATING:
				
						if(drv_spta.steps_add != 0)
						{
							/* �ﵽ�������貽�� */			
							if( drv_spta.steps_taken >= drv_spta.steps_add)   
							{ 
								TIM4->ARR = 720;
								/*Ԥ�Ƶ���������ٲ��� = �Ѽ��ٵĲ��� + ����ٵĲ���*/
								drv_spta.steps_compare = drv_spta.steps_taken + drv_spta.steps_all;    
//								printf("drv_spta.step_arr = %d\r\n",drv_spta.step_arr);									
								/* תΪ����״̬ */					
								drv_spta.step_state = UNIFORM;			
								break;
							}							
						}
						else//ֻ����һ�� 
						{
 
							/*����״̬ */
							drv_spta.step_state = DECELERATING;						
						}
						
						/*�Ƿ�ﵽ����ٶ�*/
	//					if(drv_spta.step_speed >= drv_spta.speed_lim)
	//					{
	//					//	drv_spta.step_speed = drv_spta.speed_lim;
	//						/*Ԥ�Ƶ���������ٲ��� = �Ѽ��ٵĲ��� + ����ٵĲ���*/
	//						drv_spta.steps_compare = drv_spta.steps_taken + drv_spta.steps_all;    						
	//						/* תΪ����״̬ */					
	//						drv_spta.step_state = UNIFORM;				
	//						break;
	//					}
							/* û��������ٶ����� && �պù�һ����ƽ�������� */
	//						if( (drv_spta.step_speed <= drv_spta.speed_lim) && ( (drv_spta.steps_taken % drv_spta.steps_interval) == 0) )		
					if( (drv_spta.step_speed <= drv_spta.step_arr) && ( (drv_spta.steps_taken % drv_spta.steps_interval) == 0) )									
						{
						 /* ���ٽ׶Σ��ٶ��ۼ���������ٶ�ֵ���� */
							drv_spta.step_speed++ ;		
							Delay_ms(1);	
						}
		

						break;						

						
			/* SPTA����״̬ */				
			case UNIFORM:	


							/* ��ǰ�������경��  >= �Ѽ��ٲ��� + ����ٲ��� */
						if( drv_spta.steps_taken >= drv_spta.steps_compare ) 
						{	
							/* ����ٲ��� = �ܲ��� - (�Ѽ��ٲ��� + �Ѿ��ٲ���) */
							//drv_spta.steps_cut = drv_spta.steps_required - drv_spta.steps_taken;  //�ڴ�ȷ������ٲ���
							/* ����״̬ */
	//printf("�Ѿ�����steps_taken == %d\r\n",drv_spta.steps_taken);
							drv_spta.step_state = DECELERATING;
							//printf("steps_cut == %d\r\n",drv_spta.steps_cut);	

//									printf("drv_spta.step_arr = %d\r\n",drv_spta.step_arr);		
						}
						
	
					 break;
			
			/* SPTA����״̬ */				
			case DECELERATING:	
	//printf("steps_taken == %d\r\n",drv_spta.steps_taken);				
						if( drv_spta.steps_taken >= drv_spta.steps_required )/* ��ǰ���� >= �ܲ��� #�ﵽ�������貽��*/
						{	
	//printf("�ܲ���-��ǰ���� '0'  -> %d\r\n",(drv_spta.steps_required-drv_spta.steps_taken) );

							/* תΪ����״̬ */						
							drv_spta.step_state = IDLE;					
							break;
						}		
						/* û����С�ٶ� && һ����ƽ״̬���� drv_spta.steps_interval ������ */					
						else if( (	drv_spta.step_speed >= 10 ) && ((drv_spta.steps_taken % drv_spta.steps_interval) == 0) ) 
						{
						 /* ���ٽ׶Σ��ٶ��ۼ���������ٶ�ֵ�ݼ� */
							drv_spta.step_speed-- ;		

	//printf("step_speed == %d\r\n",drv_spta.step_speed);							
							Delay_ms(1);		
						}				
								
					 break;		
			
			
			/* SPTA����״̬ */		
			case IDLE:
		
						TIM4->CR1 &= ~(0x1<<0);       //ʧ�ܼ�����;				
	//					TIM4->CCR3 = 0;
						ENABLE = 1;								//ʧ�ܲ������
	//printf("\r\nDRV�ѿ���״̬,�ô��г��ѹ���steps_taken -> %d\r\n",drv_spta.steps_taken);
	//printf("�ѿ���״̬,��ǰλ����    TurnMeter   -> %d\r\n",drv_spta.TurnMeter);	
						/* ����spta������� */
						drv_spta.steps_taken = 0;     //�����еĲ���
						drv_spta.step_speed = 0;   //��ǰ���ٶȣ�ռ�ձȣ�
						drv_spta.steps_add = 0;      	//����ٵĲ���  
						drv_spta.steps_all = 0;       //����ٵĲ���
						drv_spta.steps_cut = 0;       //����ٵĲ���	
						drv_spta.speed_lim = 0;			  //�����ٶ�����	
						drv_spta.steps_interval = 0;  //�����
						drv_spta.steps_required = 0;  //�ܲ���
						drv_spta.over =1;
						drv_spta.steps_compare=0;
					//	Delay_ms(50);	
					
					 break;			

			default:           
							break;		
			
		}			
	}

	
	
}
/*
******************************************************
�������ܣ������Ӽ��ټ��㸳ֵ
�����βΣ� dir������ ,   step�� ���Ĳ��� 
��������ֵ��None
���ߣ�hai 
�汾��v0.0
******************************************************
*/
void Start_Speedup_Compute( int step )
{

  /* �ж��Ƿ����ڼӼ��� */
//  if(drv_spta.step_state != IDLE)//IDLE == 0     1�� 2���� 3��
//    return;
	
  /* �жϷ��� */	
//		(step > 0) ? (DIR = 1) : (DIR = 0); 
//		step = abs(step);					//abs����:ȡ����ֵ; abs��-123��= 123��	
//		ENABLE = 0;								//ʹ�ܲ������	


	if(Toggle_aisle)
	{
  /* ST---->SPTA���в�����ʼ�� */
		st_spta.steps_required = step; 

		st_spta.steps_add = (st_spta.steps_required / 4) ;     	                  		                                 //����ٵĲ���  2
		st_spta.steps_cut = st_spta.steps_add;      																																	 //����ٵĲ���	 2              								                  	
		st_spta.steps_all = st_spta.steps_required - (st_spta.steps_add + st_spta.steps_cut); 											 //����ٵĲ���  4      ȡ�������Ӿ�׼��
 
		
		
		st_spta.begin =  19600;
		st_spta.finish = st_spta.steps_required - 19600; /* ��ʼ���� */
		
		st_spta.steps_interval = st_spta.steps_add/ST_Max_speed;  //�����
		
		/* ����ٶ��������в����Ķ������ */	
		st_spta.speed_lim = ST_Max_speed;					//��_65280�� ��_ռ�ձ�ʹ�� ==  101(ARR) 
		
		st_spta.step_state = ACCELERATING;					/* ����״̬ */ 		
		st_spta.step_speed =1;
		st_spta.step_arr = ST_PWM_ARR;
//		printf("�����         steps_interval == %d\r\n",drv_spta.steps_interval);
//		printf("�����ٶ����� speed_lim 			== %d\r\n",drv_spta.speed_lim);		
		ST_ENABLE = 1;								//ʹ�ܲ������	
		
	}
	else 
	{
  /* DRV---->SPTA���в�����ʼ�� */
		drv_spta.steps_required = step; 

		drv_spta.steps_add = (drv_spta.steps_required / 4) ;     	                  		                                 //����ٵĲ���  2
		drv_spta.steps_cut = drv_spta.steps_add;      																																	 //����ٵĲ���	 2              								                  	
		drv_spta.steps_all = drv_spta.steps_required - (drv_spta.steps_add + drv_spta.steps_cut); 											 //����ٵĲ���  4      ȡ�������Ӿ�׼��


		/* ����� */
		drv_spta.steps_interval = drv_spta.steps_add /DRV_Max_speed ;//drv_spta.steps_add / drv_spta.speed_lim; 
		 
		
			/* ����ٶ��������в����Ķ������ */	
		drv_spta.speed_lim = DRV_Max_speed;//PWM_cut(drv_spta.steps_add);  
		
		drv_spta.step_state = ACCELERATING;					/* ����״̬ */ 		
		drv_spta.step_speed =1;                     /* ��ʼ�ٶ� */
		drv_spta.step_arr = DRV_PWM_ARR;
		
//		printf("�����         steps_interval == %d\r\n",drv_spta.steps_interval);
//		printf("�����ٶ����� speed_lim 			== %d\r\n",drv_spta.speed_lim);		
		ENABLE = 0;								//ʹ�ܲ������	
	
	}
	

//		TIM4->CR1 |= 0x1<<0;			//ʹ�ܼ�����:		
		
}





/*
******************************************************
�������ܣ�SYSTICK��ʼ��
�����βΣ�None
��������ֵ��None
��ע��ʱ��Դ / 8  == 9Mhz     ��һ��ʱ�� = 1 / 9 us
���ߣ�hai 
�汾��v0.0
******************************************************
*/
void Systick_Init(void)
{
//	//ѡ��ʱ��Դ��
//	SysTick ->CTRL &= ~( 0x1<<2 );//72Mhz (/8) == ѡ��9Mhz  

//	//����װ��ֵ����дֵ��
//	SysTick ->LOAD =9*1000;//1ms
//	
//	//�Եݼ�������ִ��д������
//	SysTick ->VAL =0;

//	//ʹ��ϵͳ��શ�ʱ���ж�;
//	SysTick ->CTRL |= (0x1<<1);   
//	
//	//�رն�ʱ��
////	SysTick->CTRL &= ~(1 << 0);       
//	SysTick->CTRL |= (1 << 0); 	
	
	/* 1 =ʧ�ܣ�0 =�ɹ� */
	if(SysTick_Config(72000))//1ms
	{
		printf("�Ե�..\r\n");
		while(1);//�ȴ���ʼ�����;
	}
	printf("SysTick --> 1ms_inter ��ʼ�����");

}


void SysTick_Handler(void)
{
	static uint32_t runiwdg =0;
		
	SysTick->CTRL &= ~(1 <<16);

	runtime++;/* ��¼ϵͳʱ�� */
	
	runiwdg++;
	if(runiwdg > 1000) // 1S
	{
		runiwdg =0;
		IWDG->KR = 0XAAAA;    //ι��
	}

}	


void Delay_ms(u32 num) //ms��ʱ
{
	uint32_t tim_ms = runtime;
	
	while( runtime < tim_ms + num ) /* ����ϵͳʱ�����˳� */
	{
		;
	}
}


void Delay_us(u32 num) //us��ʱ
{
	
	__NOP();		/* ��ʱһ��ָ������ */	
}

