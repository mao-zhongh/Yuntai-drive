#include "eeprom.h"


//2015-6-2 16:51:29
//by @creep

//设置EEPROM的大小为1K
//可根据实际调整大小
u16 STMFLASH_BUFF[STMFLASH_SIZE];
//要写数据的起始地址为0-STMFLASH_SIZE,类型为HalfWord类型
//只能写数据类型为HalfWord的数据到EEPROM 里面
//void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
//{
//	u16 j;
//	STMFLASH_Read_Backup();															//读出备份区域的数据
//	FLASH_Unlock();                    								 //解锁
//	FLASH_ErasePage(FLASH_PAGE0_ADDR); 							   //擦除第0页
//	
//	for(j=0;j<NumToWrite;j++)
//	{
//		STMFLASH_BUFF[j+0] = *(pBuffer+j);			//将数据写到数组中去
//	}	
//	STMFLASH_Write_NoErase();													//将数据写回到第255页中
//	FLASH_Lock();																			//上锁	
//}
////读取数据为字节类型
////将EEEPROM的1K字节读出来
//void STMFLASH_Read_Backup(void)
//{
//	u16  t;
//	for(t=0;t<STMFLASH_SIZE;t++) 
//	{
//		*(STMFLASH_BUFF+t) = *(u16*)(FLASH_PAGE0_ADDR+t*2);		
//	}	
//}   
////将EEPROM的1K的数组写回到第255页
////写数据前不擦除
//void STMFLASH_Write_NoErase(void)
//{
//	u16 t;
//	for(t=0;t<STMFLASH_SIZE;t++)
//	{		
//		FLASH_ProgramHalfWord(FLASH_PAGE0_ADDR+t*2,*(STMFLASH_BUFF+t));
//	}	
//}
//从flash中读数据   俩字节 --> 16bit
//void STMFLASH_Read(u16 ReadAddr,u16 *pstr,u16 NumberToRead)
//{
//	u16 t;

//	
//	for(t=0;t<NumberToRead;t++)
//	{
//		*(pstr+t) = *(u16*)(FLASH_PAGE0_ADDR+ReadAddr+t*2);	
//		//*(pstr+t) = ;	//俩字节 16bit	
//	}  

//		
//}




//闪存页擦除
void Erasure_(u32 ADDR_)
{
tou:	
	FLASH_Unlock();                    								 //解锁
//检查FLASH_SR寄存器的BSY位，以确认没有其他正在进行的闪存操作；
	if((FLASH->CR & (0x01<<7))==1)
	{
			goto tou;
	}
//设置FLASH_CR寄存器的PER位为’1’；
	FLASH->CR |=0x1<<1;                                     //页擦除
//用FLASH_AR寄存器选择要擦除的页；
	FLASH->AR = ADDR_;
//设置FLASH_CR寄存器的STRT位为’1’；
	FLASH->CR |= 0x1<<6;
//等待BSY位变为’0’；
	while(FLASH->SR & (0x1<<0));
//读出被擦除的页并做验证。

// RDPRT键 = 0x000000A5 
// KEY1 = 0x45670123 
// KEY2 = 0xCDEF89AB

}

/*读半节 8bit*/
unsigned short IapReadByte(u32 addr,u16 *pstr)
{
	
	FLASH_Unlock();                    								 //解锁
 	*pstr = *(u16*)(FLASH_PAGE0_ADDR*2);		
	
//	FLASH_Lock();
}





/***************************************************/
//3、读取FLASH状态寄存器，查看状态，返回值为状态
u8 STMFLASH_GetStatus(void)
{
	u32 res;

	res=FLASH->SR; 
	if(res&(1<<0))return 1;    //忙碌则返回1
	else if(res&(1<<2))return 2; //编程错误返回2
	else if(res&(1<<4))return 3; //写保护错误返回3

	return 0; //非忙碌状态且无任何错误时，返回0，或者说可以读写操作的话返回0
}

//4、等待操作完成函数
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

u8 STMFLASH_ErasePage(u32 paddr)

{

	u8 res=0;

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

//6、在指定位置写入半字
u8 STMFLASH_WriteHalfWord(u32 faddr, u32 dat)
{

		u8 res=0;      
	  FLASH_Unlock();                    								 //解锁
	
//		res=STMFLASH_WaitDone(0XFF);  
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
//		FLASH_Lock();																			//上锁	
		return res;

} 

//7、读函数
u32 STMFLASH_ReadHalfWord(u32 faddr)
{

	return *(vu32*)faddr; //返回指向该地址的内容

}


//10、从指定的地址读指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)  
{
	u16 i;

	for(i=0;i<NumToRead;i++)
	{
	pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取改地址数据

	ReadAddr+=2;// 一次读取两字节的数据，因此地址要相应的+2
	}

}

//从指定的地址写一字(32bit)长度的数据
u8 STMFLASH_Write(u32 ADDRESS_X,u32 *pBuffer[])  
{
	u8 i=0;
	u32 addr =ADDRESS_X+4;// 一次读取两字节的数据，因此地址要相应的+2
//printf("bushu = %d \r\n",*pBuffer[0]);
//printf("dir_ =%d \r\n",*pBuffer[1]);
	STMFLASH_WriteHalfWord(ADDRESS_X,*pBuffer[0]);

	i = STMFLASH_WriteHalfWord(addr,*pBuffer[1]);
	
	return i;

}

/*函数功能：向flash中写一个预置位值 { dat步数 ，dir方向 }
	内含解锁，上锁、判断标准    调用唯要注意清页
*/
unsigned char Write_EEPROM(u8 dir,u32 ADDRESS_X,u16 dat)
{
	
	u32 addr =ADDRESS_X+2;
	STMFLASH_WriteHalfWord(ADDRESS_X,dat);
	STMFLASH_WriteHalfWord(addr,dir);
	
}





