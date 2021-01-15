#include  "uart.h"

u8 UART1_Overflow_Flag = 0;                     //缓冲区满标志
u8 UART1_Recv_Buf[UART_BUF_LEN] = {0};  				//串口接收缓冲区
u8 UART1_Flag_count = 0;  	 	 									//统计接收次数
u8 motor_disturb =0,Continue_disturb = 0;


//printf()函数底层支持代码
#pragma import(__use_no_semihosting_swi) //取消半主机状态

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
函数功能：Uart管脚初始化
函数形参：None
函数返回值：None   APB2
备注：UART1的时钟72Mhz   PA9(TX)  PA10(RX) 
版本：v0.0
******************************************************
*/
void Usart1__Init(u32 brr)
{
	double USARTDIV =0.0;
	u32 DIV_Man =0;//整数
	u32 DIV_Fra =0;//小数


	RCC->APB2ENR |= 0X1<<2;//开A口时钟;
	
	GPIOA->CRH &= ~(0Xf<<4);//PA9复用推挽输出
	GPIOA->CRH |= 0Xb<<4;//50Mhz;
	
	GPIOA->CRH &= ~(0Xf<<8);//PA10浮空输入
	GPIOA->CRH |= 0X4<<8;
	
	RCC->APB2ENR |= 0x1<<14;//开USART1时钟;
	
	USARTDIV = 72000000.0 / (16 * brr );
	DIV_Man = USARTDIV;
	DIV_Fra = ( USARTDIV - DIV_Man )*16+0.5; //四舍五入;
	//15—4位存放整数 ；     3—0位存放小数 ；
	USART1->BRR = DIV_Man << 4 | DIV_Fra ;
	
	
	USART1->CR1 =0;
	USART1->CR2 &= ~(0x3<<12);
	/*
	一个起始位，8个数据位，n个停止位；
	1个停止位；
	*/
	
	//设置串口1中断优先级；
	NVIC_SetPriority( USART1_IRQn, 3 );
	NVIC_EnableIRQ ( USART1_IRQn );//使能NVIC响应;
	
	USART1->CR1 |= 0x1<<5;//接收缓冲区非空中断使能
	USART1->CR1 |= 0x3<<2;//使能发送.接收
	USART1->CR1 |= 0X1<<13;//USART1使能;
}







/*
******************************************************
函数功能：发送字符串
函数形参：buf需发送的字符串首地址
函数返回值：None
备注：
版本：v0.0
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
函数功能：发送字符
函数形参：需发送的字符 ‘data’
函数返回值：None
备注：
作者：Fannnnn  
日期：2020年06月27日
版本：v0.0
******************************************************
*/
void send_Byte(u8 data )
{
	while(!(USART1->SR & (1 << 7)));
	USART1->DR = data;
}
/*
******************************************************
函数功能：接收子符
函数形参：None
函数返回值：None
备注：
作者：
版本：v0.0
******************************************************
*/
u8 Rev_Byte(void )
{
	while(!(USART1->SR & (1 << 5)));
	return  USART1->DR ;
}

//usart1公用中断服务函数;
void USART1_IRQHandler(void)
{

	if(USART1->SR & 0x1<<3)//过载错误                             一次发送大量指令会造成过载错误中断
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE);
		__NOP();		
		UART1_Recv_Buf[UART1_Flag_count] = USART1 ->DR ; 					//赋值,清零;将数据寄存器的值存放到 UART1_Recv_Buf
		//USART1->SR &= ~(0x1<<3);
	}
	if((USART1 ->SR &0x1<<5))//读数据寄存器非空
	{
		if( !UART1_Overflow_Flag )											//没数据就进来
		{			
			if( UART1_Flag_count < UART_BUF_LEN)
			{
				UART1_Recv_Buf[UART1_Flag_count] = USART1 ->DR ; 					//赋值,清零;将数据寄存器的值存放到 UART1_Recv_Buf
	//返还回来.回显				{调试时使用后期可屏蔽}
	//			send_Byte(UART1_Recv_Buf[UART1_Flag_count]);	
	//			__NOP(); 
				UART1_Flag_count++;  
				USART1 ->SR &= ~(0X1<<5);

				if( (UART1_Recv_Buf[UART1_Flag_count-1] == '\r' && UART1_Recv_Buf[UART1_Flag_count-2] == '\n') || UART1_Flag_count >= 7 )	//PELCO_D格式数据7位~
				{	
					UART1_Flag_count =0;					
					UART1_Overflow_Flag = 1;													//关闭串口中断,停止接收 并跳到主函数执行语句
//					UART1_Recv_Buf[UART1_Flag_count] = '\0';
	LED_1 =! LED_1 ;
					if( motor_disturb )																//突发标志  0：能正常运行完成为  /   1:旧指令没做完,又有新指令来
					{
						Continue_disturb = 1;                          
					}
				}	
			}
			else
			{		
				  UART1_Overflow_Flag = 1;   												//关闭串口中断,停止接收
			}
		}		
	}

	
}	
	



