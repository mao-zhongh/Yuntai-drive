#include "eeprom.h"


//2015-6-2 16:51:29
//by @creep

//����EEPROM�Ĵ�СΪ1K
//�ɸ���ʵ�ʵ�����С
u16 STMFLASH_BUFF[STMFLASH_SIZE];
//Ҫд���ݵ���ʼ��ַΪ0-STMFLASH_SIZE,����ΪHalfWord����
//ֻ��д��������ΪHalfWord�����ݵ�EEPROM ����
//void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
//{
//	u16 j;
//	STMFLASH_Read_Backup();															//�����������������
//	FLASH_Unlock();                    								 //����
//	FLASH_ErasePage(FLASH_PAGE0_ADDR); 							   //������0ҳ
//	
//	for(j=0;j<NumToWrite;j++)
//	{
//		STMFLASH_BUFF[j+0] = *(pBuffer+j);			//������д��������ȥ
//	}	
//	STMFLASH_Write_NoErase();													//������д�ص���255ҳ��
//	FLASH_Lock();																			//����	
//}
////��ȡ����Ϊ�ֽ�����
////��EEEPROM��1K�ֽڶ�����
//void STMFLASH_Read_Backup(void)
//{
//	u16  t;
//	for(t=0;t<STMFLASH_SIZE;t++) 
//	{
//		*(STMFLASH_BUFF+t) = *(u16*)(FLASH_PAGE0_ADDR+t*2);		
//	}	
//}   
////��EEPROM��1K������д�ص���255ҳ
////д����ǰ������
//void STMFLASH_Write_NoErase(void)
//{
//	u16 t;
//	for(t=0;t<STMFLASH_SIZE;t++)
//	{		
//		FLASH_ProgramHalfWord(FLASH_PAGE0_ADDR+t*2,*(STMFLASH_BUFF+t));
//	}	
//}
//��flash�ж�����   ���ֽ� --> 16bit
//void STMFLASH_Read(u16 ReadAddr,u16 *pstr,u16 NumberToRead)
//{
//	u16 t;

//	
//	for(t=0;t<NumberToRead;t++)
//	{
//		*(pstr+t) = *(u16*)(FLASH_PAGE0_ADDR+ReadAddr+t*2);	
//		//*(pstr+t) = ;	//���ֽ� 16bit	
//	}  

//		
//}




//����ҳ����
void Erasure_(u32 ADDR_)
{
tou:	
	FLASH_Unlock();                    								 //����
//���FLASH_SR�Ĵ�����BSYλ����ȷ��û���������ڽ��е����������
	if((FLASH->CR & (0x01<<7))==1)
	{
			goto tou;
	}
//����FLASH_CR�Ĵ�����PERλΪ��1����
	FLASH->CR |=0x1<<1;                                     //ҳ����
//��FLASH_AR�Ĵ���ѡ��Ҫ������ҳ��
	FLASH->AR = ADDR_;
//����FLASH_CR�Ĵ�����STRTλΪ��1����
	FLASH->CR |= 0x1<<6;
//�ȴ�BSYλ��Ϊ��0����
	while(FLASH->SR & (0x1<<0));
//������������ҳ������֤��

// RDPRT�� = 0x000000A5 
// KEY1 = 0x45670123 
// KEY2 = 0xCDEF89AB

}

/*����� 8bit*/
unsigned short IapReadByte(u32 addr,u16 *pstr)
{
	
	FLASH_Unlock();                    								 //����
 	*pstr = *(u16*)(FLASH_PAGE0_ADDR*2);		
	
//	FLASH_Lock();
}





/***************************************************/
//3����ȡFLASH״̬�Ĵ������鿴״̬������ֵΪ״̬
u8 STMFLASH_GetStatus(void)
{
	u32 res;

	res=FLASH->SR; 
	if(res&(1<<0))return 1;    //æµ�򷵻�1
	else if(res&(1<<2))return 2; //��̴��󷵻�2
	else if(res&(1<<4))return 3; //д�������󷵻�3

	return 0; //��æµ״̬�����κδ���ʱ������0������˵���Զ�д�����Ļ�����0
}

//4���ȴ�������ɺ���
u8 STMFLASH_WaitDone(u16 time)
{

	u8 res;

	do

	{
		res=STMFLASH_GetStatus();//��ȡFLASH״̬�Ƿ���æµ״̬
		
		if(res!=1)break;//��æµ״̬��break

		Delay_us(1);

		time--;
	}
	while(time);

	if(time==0)res=0xff;//TIMEOUT��ʱ�ˣ�res����0xff

	return res;//������ɷ���0

}

u8 STMFLASH_ErasePage(u32 paddr)

{

	u8 res=0;

	res=STMFLASH_WaitDone(0X5FFF);// ���Ȳ鿴�Ƿ���æµ״̬��

	if(res==0)//��æµ״̬���Բ���

	{ 

	FLASH->CR|=1<<1;//ѡ��ҳ����

	FLASH->AR=paddr;//������ַ

	FLASH->CR|=1<<6;//  �������� 

	res=STMFLASH_WaitDone(0X5FFF);//�Ƿ�������

	if(res!=1)//

	{

	FLASH->CR&=~(1<<1);//��ɺ�ҳ����ѡ��λ����

	}

	}
	
	return res;

}

//6����ָ��λ��д�����
u8 STMFLASH_WriteHalfWord(u32 faddr, u32 dat)
{

		u8 res=0;      
	  FLASH_Unlock();                    								 //����
	
//		res=STMFLASH_WaitDone(0XFF);  
//		STMFLASH_ErasePage(faddr); 							  				 //���� 

		if(res==0)//OK����æµ״̬�½���
		{

			FLASH->CR|=1<<0;//����д����

			*(vu32*)faddr=dat;//���ַ��д������

			res=STMFLASH_WaitDone(0XFF);//�ȴ��������

			if(res!=1)//��æ
			{

				FLASH->CR&=~(1<<0);//����д����λ
 
			}
		} 
//		FLASH_Lock();																			//����	
		return res;

} 

//7��������
u32 STMFLASH_ReadHalfWord(u32 faddr)
{

	return *(vu32*)faddr; //����ָ��õ�ַ������

}


//10����ָ���ĵ�ַ��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)  
{
	u16 i;

	for(i=0;i<NumToRead;i++)
	{
	pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ�ĵ�ַ����

	ReadAddr+=2;// һ�ζ�ȡ���ֽڵ����ݣ���˵�ַҪ��Ӧ��+2
	}

}

//��ָ���ĵ�ַдһ��(32bit)���ȵ�����
u8 STMFLASH_Write(u32 ADDRESS_X,u32 *pBuffer[])  
{
	u8 i=0;
	u32 addr =ADDRESS_X+4;// һ�ζ�ȡ���ֽڵ����ݣ���˵�ַҪ��Ӧ��+2
//printf("bushu = %d \r\n",*pBuffer[0]);
//printf("dir_ =%d \r\n",*pBuffer[1]);
	STMFLASH_WriteHalfWord(ADDRESS_X,*pBuffer[0]);

	i = STMFLASH_WriteHalfWord(addr,*pBuffer[1]);
	
	return i;

}

/*�������ܣ���flash��дһ��Ԥ��λֵ { dat���� ��dir���� }
	�ں��������������жϱ�׼    ����ΨҪע����ҳ
*/
unsigned char Write_EEPROM(u8 dir,u32 ADDRESS_X,u16 dat)
{
	
	u32 addr =ADDRESS_X+2;
	STMFLASH_WriteHalfWord(ADDRESS_X,dat);
	STMFLASH_WriteHalfWord(addr,dir);
	
}





