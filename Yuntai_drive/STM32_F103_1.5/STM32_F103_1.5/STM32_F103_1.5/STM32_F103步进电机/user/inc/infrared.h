#ifndef _INFRARED_H
#define _INFRARED_H

#include "main.h"
#include "io_bit.h"

#define  infrared    PBin(9) 

typedef struct {
		u32 data_code;/*地址码，地址反码  .  控制码，控制反码 */
	  u16 capture_time;
		u8  start;    /*收到同步码头*/
		u8  over;     
}infrared_;


extern infrared_ data_;
void infrared_printf(void);
#endif
