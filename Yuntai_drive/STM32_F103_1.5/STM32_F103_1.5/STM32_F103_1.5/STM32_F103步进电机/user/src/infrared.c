#include "infrared.h"

infrared_ data_ = {0};

void infrared_printf(void)
{
	u8 addr_code=0,addr_reverse_code=0;			  //地址码   、  地址反码 
	u8 control_code=0,control_reverse_code=0; //控制码   、  控制反码


	if(data_.over)
	{
		data_.over = 0;
		control_code = (data_.data_code>>24) &0xff;	
		control_reverse_code = (data_.data_code>>16) &0xff;
		if(control_code == (u8)(~control_reverse_code))
		{
			addr_code = data_.data_code>>8 &0xff;
			addr_reverse_code = data_.data_code & 0xff; 
			if( addr_code == (u8)(~addr_reverse_code) )
			{
				printf("该按键值 == %x \r\n",control_code );
			}
			
		}
	}
	
}





