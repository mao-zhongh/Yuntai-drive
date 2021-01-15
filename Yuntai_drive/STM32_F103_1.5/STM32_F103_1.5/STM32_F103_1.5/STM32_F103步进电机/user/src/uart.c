#include  "uart.h"

u8 UART1_Overflow_Flag = 0;                     //����������־
u8 UART1_Recv_Buf[UART_BUF_LEN] = {0};  				//���ڽ��ջ�����
u8 UART1_Flag_count = 0;  	 	 									//ͳ�ƽ��մ���
u8 motor_disturb =0,Continue_disturb = 0;


//printf()�����ײ�֧�ִ���
#pragma import(__use_no_semihosting_swi) //ȡ��������״̬

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;

int fputc(int ch, FILE *f) 
{
	while((USART1->SR &(0X01<<7))==0){};
		USART1->DR=ch;
  return (ch);
}
int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  while((USART1->SR &(0X01<<7))==0);
		USART1->DR=ch;
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}


/*
******************************************************
�������ܣ�Uart�ܽų�ʼ��
�����βΣ�None
��������ֵ��None   APB2
��ע��UART1��ʱ��72Mhz   PA9(TX)  PA10(RX) 
�汾��v0.0
******************************************************
*/
void Usart1__Init(u32 brr)
{
	double USARTDIV =0.0;
	u32 DIV_Man =0;//����
	u32 DIV_Fra =0;//С��


	RCC->APB2ENR |= 0X1<<2;//��A��ʱ��;
	
	GPIOA->CRH &= ~(0Xf<<4);//PA9�����������
	GPIOA->CRH |= 0Xb<<4;//50Mhz;
	
	GPIOA->CRH &= ~(0Xf<<8);//PA10��������
	GPIOA->CRH |= 0X4<<8;
	
	RCC->APB2ENR |= 0x1<<14;//��USART1ʱ��;
	
	USARTDIV = 72000000.0 / (16 * brr );
	DIV_Man = USARTDIV;
	DIV_Fra = ( USARTDIV - DIV_Man )*16+0.5; //��������;
	//15��4λ������� ��     3��0λ���С�� ��
	USART1->BRR = DIV_Man << 4 | DIV_Fra ;
	
	
	USART1->CR1 =0;
	USART1->CR2 &= ~(0x3<<12);
	/*
	һ����ʼλ��8������λ��n��ֹͣλ��
	1��ֹͣλ��
	*/
	
	//���ô���1�ж����ȼ���
	NVIC_SetPriority( USART1_IRQn, 3 );
	NVIC_EnableIRQ ( USART1_IRQn );//ʹ��NVIC��Ӧ;
	
	USART1->CR1 |= 0x1<<5;//���ջ������ǿ��ж�ʹ��
	USART1->CR1 |= 0x3<<2;//ʹ�ܷ���.����
	USART1->CR1 |= 0X1<<13;//USART1ʹ��;
}







/*
******************************************************
�������ܣ������ַ���
�����βΣ�buf�跢�͵��ַ����׵�ַ
��������ֵ��None
��ע��
�汾��v0.0
******************************************************
*/
void Send_string (u8 *buf)
{
	while( *buf!='\0' )
	{
		while(!(USART1->SR & (0x1<<7)));
		USART1->DR =*buf;
		buf++;
	}
}

/*
******************************************************
�������ܣ������ַ�
�����βΣ��跢�͵��ַ� ��data��
��������ֵ��None
��ע��
���ߣ�Fannnnn  
���ڣ�2020��06��27��
�汾��v0.0
******************************************************
*/
void send_Byte(u8 data )
{
	while(!(USART1->SR & (1 << 7)));
	USART1->DR = data;
}
/*
******************************************************
�������ܣ������ӷ�
�����βΣ�None
��������ֵ��None
��ע��
���ߣ�
�汾��v0.0
******************************************************
*/
u8 Rev_Byte(void )
{
	while(!(USART1->SR & (1 << 5)));
	return  USART1->DR ;
}

//usart1�����жϷ�����;
void USART1_IRQHandler(void)
{

	if(USART1->SR & 0x1<<3)//���ش���                             һ�η��ʹ���ָ�����ɹ��ش����ж�
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE);
		__NOP();		
		UART1_Recv_Buf[UART1_Flag_count] = USART1 ->DR ; 					//��ֵ,����;�����ݼĴ�����ֵ��ŵ� UART1_Recv_Buf
		//USART1->SR &= ~(0x1<<3);
	}
	if((USART1 ->SR &0x1<<5))//�����ݼĴ����ǿ�
	{
		if( !UART1_Overflow_Flag )											//û���ݾͽ���
		{			
			if( UART1_Flag_count < UART_BUF_LEN)
			{
				UART1_Recv_Buf[UART1_Flag_count] = USART1 ->DR ; 					//��ֵ,����;�����ݼĴ�����ֵ��ŵ� UART1_Recv_Buf
	//��������.����				{����ʱʹ�ú��ڿ�����}
	//			send_Byte(UART1_Recv_Buf[UART1_Flag_count]);	
	//			__NOP(); 
				UART1_Flag_count++;  
				USART1 ->SR &= ~(0X1<<5);

				if( (UART1_Recv_Buf[UART1_Flag_count-1] == '\r' && UART1_Recv_Buf[UART1_Flag_count-2] == '\n') || UART1_Flag_count >= 7 )	//PELCO_D��ʽ����7λ~
				{	
					UART1_Flag_count =0;					
					UART1_Overflow_Flag = 1;													//�رմ����ж�,ֹͣ���� ������������ִ�����
//					UART1_Recv_Buf[UART1_Flag_count] = '\0';
	LED_1 =! LED_1 ;
					if( motor_disturb )																//ͻ����־  0���������������Ϊ  /   1:��ָ��û����,������ָ����
					{
						Continue_disturb = 1;                          
					}
				}	
			}
			else
			{		
				  UART1_Overflow_Flag = 1;   												//�رմ����ж�,ֹͣ����
			}
		}		
	}

	
}	
	



