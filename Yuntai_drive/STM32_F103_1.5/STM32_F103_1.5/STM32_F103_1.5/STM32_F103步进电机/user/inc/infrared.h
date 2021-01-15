#ifndef _INFRARED_H
#define _INFRARED_H

#include "main.h"
#include "io_bit.h"

#define  infrared    PBin(9) 

typedef struct {
		u32 data_code;/*��ַ�룬��ַ����  .  �����룬���Ʒ��� */
	  u16 capture_time;
		u8  start;    /*�յ�ͬ����ͷ*/
		u8  over;     
}infrared_;


extern infrared_ data_;
void infrared_printf(void);
#endif
