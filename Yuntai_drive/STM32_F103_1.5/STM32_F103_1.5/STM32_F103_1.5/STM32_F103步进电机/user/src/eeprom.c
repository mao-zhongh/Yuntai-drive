#include "eeprom.h"

/* [ 开/关限位 ]         因DRV控制上下90° 复位时要向上移动(不符合限位要求),将其关闭！ */
u8 restart =0;             
/* ROM空间预置位成员 */
FLASH_PRESET FLASH_DATA={0};

//存放 DRV / ST  掉电前的位置     先发_步数,在放_方向

//设置EEPROM的大小为1K
//可根据实际调整大小

//要写数据的起始地址为0-STMFLASH_SIZE,类型为HalfWord类型
//只能写数据类型为HalfWord的数据到EEPROM 里面




/***************************************************/
//读取FLASH状态寄存器，查看状态，返回值为状态
u8 STMFLASH_GetStatus(void)
{
	u32 res;

	res=FLASH->SR; 
	if(res&(1<<0))return 1;    //忙碌则返回1
	else if(res&(1<<2))return 2; //编程错误返回2
	else if(res&(1<<4))return 3; //写保护错误返回3

	return 0; //非忙碌状态且无任何错误时，返回0，或者说可以读写操作的话返回0
}

//等待操作完成函数
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	
	do
	{
		res=STMFLASH_GetStatus();//读取FLASH状态是否处于忙碌状态
		if(res!=1)break;//非忙碌状态则break
		Delay_us(1);
		time--;
	}
	while(time);

	if(time==0)res=0xff;//TIMEOUT超时了，res等于0xff
	return res;//操作完成返回0

}
/*擦除页*/
u8 STMFLASH_ErasePage(u32 paddr)
{

	u8 res=0;
	FLASH_Unlock();                    								 //解锁
	res=STMFLASH_WaitDone(0X5FFF);// 首先查看是否是忙碌状态，

	if(res==0)//非忙碌状态可以操作
	{ 
		FLASH->CR|=1<<1;//选择页擦除
		FLASH->AR=paddr;//擦除地址
		FLASH->CR|=1<<6;//  擦除操作 
		res=STMFLASH_WaitDone(0X5FFF);//是否操作完成

		if(res!=1)//
		{
			FLASH->CR&=~(1<<1);//完成后，页擦除选择位清零
		}
	}
	
	return res;

}

//在指定位置写入一字
u8 STMFLASH_WriteHalfWord(u32 faddr, u32 dat)
{
		u8 res=0;      
	  FLASH_Unlock();                    								 //解锁
	
		res=STMFLASH_WaitDone(0XFF);  
//		STMFLASH_ErasePage(faddr); 							  				 //擦除 

		if(res==0)//OK，非忙碌状态下进入
		{
			FLASH->CR|=1<<0;//设置写操作
			*(vu32*)faddr=dat;//向地址中写入数据
			res=STMFLASH_WaitDone(0XFF);//等待操作完成
			if(res!=1)//不忙
			{
				FLASH->CR&=~(1<<0);//清零写操作位
			}
		} 
		FLASH_Lock();																			//上锁	
		return res;

} 


//从指定的地址写一字(32bit)长度的数据
u8 STMFLASH_Write(u32 ADDRESS_X,u32 *pBuffer[])  
{
	u8 i=0;

	FLASH_ProgramWord(ADDRESS_X, *pBuffer[0]);
	ADDRESS_X +=4;// 一次读取两字节的数据，因此地址要相应的+2	
	i = FLASH_ProgramWord(ADDRESS_X, *pBuffer[1]);
	FLASH_Lock();																			//上锁	
	return i;  //操作完成 return 4

}
//读函数
u32 STMFLASH_ReadHalfWord(u32 faddr)
{

	return *(vu32*)faddr; //返回指向该地址的内容

}




/*函数功能：向flash中写一个预置位值 { dat步数 ，dir方向 }
	内含解锁，上锁、判断标准    调用唯要注意清页
*/
void Write_EEPROM(u32 dir,u32 ADDRESS_X,u32 dat)
{
	
	u32 ADDR =ADDRESS_X;
	FLASH_ProgramWord(ADDR,dat);
	ADDR +=4;
	FLASH_ProgramWord(ADDR,dir);
//	FLASH_Lock();																			//上锁	
	Delay_us(10);
}

/*函数功能：DRV向flash中读一个预置位值 { dat步数 ，dir方向 }
	内含解锁，上锁、判断标准    调用唯要注意清页
*/
void Read_DRV_EEPROM(u32 ReadAddr,u16 NumToRead)
{
	u32 ADDR=ReadAddr;

	
	FLASH_DATA.DRVROM_Step[NumToRead]=*(vu32*)ADDR;//先步数
	ADDR +=4;          
	FLASH_DATA.DRVROM_Dir[NumToRead]=*(vu32*)ADDR; //后方向	
}
/* ST向flash中读一个预置位值 */
void Read_ST_EEPROM(u32 ReadAddr,u16 NumToRead)
{
	u32 ADDR=ReadAddr;

	
	FLASH_DATA.STROM_Step[NumToRead]=*(vu32*)ADDR;//先步数
	ADDR +=4;          
	FLASH_DATA.STROM_Dir[NumToRead]=*(vu32*)ADDR; //后方向	
}



//写所以预置位的数据
void STMFLASH_Writ( u32 ReadAddr )
{ 
	u16 i;
	u32 ADDR = ReadAddr;
	FLASH_Unlock();                    								 //解锁	
	Delay_us(10);	
	FLASH_ErasePage(ADDR);                             //擦除
//	STMFLASH_ErasePage(ADDR);//解锁擦除		//Erase current sector  清扇区	
	Delay_us(10);	
	for(i=0;i<member;i++)
	{
		Write_EEPROM(FLASH_DATA.DRVROM_Dir[i],ADDR,FLASH_DATA.DRVROM_Step[i]);
		ADDR+=8;// 一次写八字节的数据，因此地址要相应的+8	
		Write_EEPROM(FLASH_DATA.STROM_Dir[i],ADDR,FLASH_DATA.STROM_Step[i]);	
		ADDR+=8;// 一次写八字节的数据，因此地址要相应的+8			
	}		
	FLASH_Lock();																			//上锁		
}

//读所以预置位的数据
void STMFLASH_Read(u32 ReadAddr)  //5
{
	u16 i;
	u32 ADDR = ReadAddr;

	for(i=0;i<member;i++)
	{
		Read_DRV_EEPROM( ADDR,i );
		ADDR+=8;// 一次读八字节的数据，因此地址要相应的+8
		Read_ST_EEPROM( ADDR,i );		
		ADDR+=8;// 一次读八字节的数据，因此地址要相应的+8		
	}
}

