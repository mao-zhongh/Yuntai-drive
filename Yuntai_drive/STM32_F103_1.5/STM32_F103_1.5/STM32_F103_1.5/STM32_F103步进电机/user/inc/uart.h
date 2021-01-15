#ifndef _UART_H_
#define _UART_H_


#include "main.h"

#define UART_BUF_LEN  	 35

extern u8 UART1_Overflow_Flag;
//extern u8 in_reset;                             // 1 : 复位中,误扰;
extern u8 UART1_Recv_Buf[UART_BUF_LEN];  				//串口接收缓冲区
extern u8 UART1_Flag_count;  	 	 									//统计接收次数
extern u8 Continue_disturb,motor_disturb;



void Usart1__Init(u32 brr);
void Send_string (u8 *buf);

#endif
