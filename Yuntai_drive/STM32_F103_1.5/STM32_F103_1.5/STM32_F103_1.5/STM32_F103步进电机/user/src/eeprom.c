#include "eeprom.h"

/* [ ��/����λ ]         ��DRV��������90�� ��λʱҪ�����ƶ�(��������λҪ��),����رգ� */
u8 restart =0;             
/* ROM�ռ�Ԥ��λ��Ա */
FLASH_PRESET FLASH_DATA={0};

//��� DRV / ST  ����ǰ��λ��     �ȷ�_����,�ڷ�_����

//����EEPROM�Ĵ�СΪ1K
//�ɸ���ʵ�ʵ�����С

//Ҫд���ݵ���ʼ��ַΪ0-STMFLASH_SIZE,����ΪHalfWord����
//ֻ��д��������ΪHalfWord�����ݵ�EEPROM ����




/***************************************************/
//��ȡFLASH״̬�Ĵ������鿴״̬������ֵΪ״̬
u8 STMFLASH_GetStatus(void)
{
	u32 res;

	res=FLASH->SR; 
	if(res&(1<<0))return 1;    //æµ�򷵻�1
	else if(res&(1<<2))return 2; //��̴��󷵻�2
	else if(res&(1<<4))return 3; //д�������󷵻�3

	return 0; //��æµ״̬�����κδ���ʱ������0������˵���Զ�д�����Ļ�����0
}

//�ȴ�������ɺ���
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
/*����ҳ*/
u8 STMFLASH_ErasePage(u32 paddr)
{

	u8 res=0;
	FLASH_Unlock();                    								 //����
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

//��ָ��λ��д��һ��
u8 STMFLASH_WriteHalfWord(u32 faddr, u32 dat)
{
		u8 res=0;      
	  FLASH_Unlock();                    								 //����
	
		res=STMFLASH_WaitDone(0XFF);  
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
		FLASH_Lock();																			//����	
		return res;

} 


//��ָ���ĵ�ַдһ��(32bit)���ȵ�����
u8 STMFLASH_Write(u32 ADDRESS_X,u32 *pBuffer[])  
{
	u8 i=0;

	FLASH_ProgramWord(ADDRESS_X, *pBuffer[0]);
	ADDRESS_X +=4;// һ�ζ�ȡ���ֽڵ����ݣ���˵�ַҪ��Ӧ��+2	
	i = FLASH_ProgramWord(ADDRESS_X, *pBuffer[1]);
	FLASH_Lock();																			//����	
	return i;  //������� return 4

}
//������
u32 STMFLASH_ReadHalfWord(u32 faddr)
{

	return *(vu32*)faddr; //����ָ��õ�ַ������

}




/*�������ܣ���flash��дһ��Ԥ��λֵ { dat���� ��dir���� }
	�ں��������������жϱ�׼    ����ΨҪע����ҳ
*/
void Write_EEPROM(u32 dir,u32 ADDRESS_X,u32 dat)
{
	
	u32 ADDR =ADDRESS_X;
	FLASH_ProgramWord(ADDR,dat);
	ADDR +=4;
	FLASH_ProgramWord(ADDR,dir);
//	FLASH_Lock();																			//����	
	Delay_us(10);
}

/*�������ܣ�DRV��flash�ж�һ��Ԥ��λֵ { dat���� ��dir���� }
	�ں��������������жϱ�׼    ����ΨҪע����ҳ
*/
void Read_DRV_EEPROM(u32 ReadAddr,u16 NumToRead)
{
	u32 ADDR=ReadAddr;

	
	FLASH_DATA.DRVROM_Step[NumToRead]=*(vu32*)ADDR;//�Ȳ���
	ADDR +=4;          
	FLASH_DATA.DRVROM_Dir[NumToRead]=*(vu32*)ADDR; //����	
}
/* ST��flash�ж�һ��Ԥ��λֵ */
void Read_ST_EEPROM(u32 ReadAddr,u16 NumToRead)
{
	u32 ADDR=ReadAddr;

	
	FLASH_DATA.STROM_Step[NumToRead]=*(vu32*)ADDR;//�Ȳ���
	ADDR +=4;          
	FLASH_DATA.STROM_Dir[NumToRead]=*(vu32*)ADDR; //����	
}



//д����Ԥ��λ������
void STMFLASH_Writ( u32 ReadAddr )
{ 
	u16 i;
	u32 ADDR = ReadAddr;
	FLASH_Unlock();                    								 //����	
	Delay_us(10);	
	FLASH_ErasePage(ADDR);                             //����
//	STMFLASH_ErasePage(ADDR);//��������		//Erase current sector  ������	
	Delay_us(10);	
	for(i=0;i<member;i++)
	{
		Write_EEPROM(FLASH_DATA.DRVROM_Dir[i],ADDR,FLASH_DATA.DRVROM_Step[i]);
		ADDR+=8;// һ��д���ֽڵ����ݣ���˵�ַҪ��Ӧ��+8	
		Write_EEPROM(FLASH_DATA.STROM_Dir[i],ADDR,FLASH_DATA.STROM_Step[i]);	
		ADDR+=8;// һ��д���ֽڵ����ݣ���˵�ַҪ��Ӧ��+8			
	}		
	FLASH_Lock();																			//����		
}

//������Ԥ��λ������
void STMFLASH_Read(u32 ReadAddr)  //5
{
	u16 i;
	u32 ADDR = ReadAddr;

	for(i=0;i<member;i++)
	{
		Read_DRV_EEPROM( ADDR,i );
		ADDR+=8;// һ�ζ����ֽڵ����ݣ���˵�ַҪ��Ӧ��+8
		Read_ST_EEPROM( ADDR,i );		
		ADDR+=8;// һ�ζ����ֽڵ����ݣ���˵�ַҪ��Ӧ��+8		
	}
}

