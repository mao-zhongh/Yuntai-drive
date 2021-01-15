#ifndef _EEPROM_H
#define _EEPROM_H
#include "stm32f10x.h"
#include "main.h"
/*===============================2020/12/3.map===============================================

    Total RO  Size (Code + RO Data)                 7160 (   6.99kB)
    Total RW  Size (RW Data + ZI Data)             16648 (  16.26kB)
    Total ROM Size (Code + RO Data + RW Data)       7292 (   7.12kB)      

=================================����ǰд���ַ=============================================*/
//����EEPROM��ģ����ʼ��ַΪFLASH�ĵ�255ҳ����ʼ��ַ  һҳ2K == 2047�ֽ�
/*  ��¼DRV_�ػ�ǰ���λ��ԭ�㲽��,���� / ��¼ST _�ػ�ǰ���λ��ԭ�㲽��,����  */
#define FLASH_Presetting       0x0807f800    // 
#define FLASH_Presetting_DRV   0x0807f800    // ע�⣺����4�ֽ�+����4�ֽ� = ƫ��+8
#define FLASH_Presetting_ST    0x0807f808    //
//#define Record_origin          0x0807f810    //

/*===================================��Ԥ��λ��ַ=============================================*/

//  ���ڴ����� { 0x0800 0000 ~0x0807 F800 }  Ϊ�˰�ȫ��0x0800 4000      128*256 = 32768
//һ��Ԥ��λƫ��8�ֽ� , ��128/1  һȦ���� 522240 ��4λ;   ���� ��4λ;
#define FLASH_start_ADDRESS  0x08004000        //��оƬFLASH��ʼ��ַ

#define one_ADDRESS          0x08004000 		   //1��Ԥ��λ
#define two_ADDRESS          0x08004010        //2��Ԥ��λ
#define three_ADDRESS        0x08004020        //3��Ԥ��λ
#define foue_ADDRESS				 0X08004030        //4��Ԥ��λ
#define five_ADDRESS				 0X08004040        //5��Ԥ��λ

/* ��5��Ԥ��λ,����Ԥ��λ�ó�Ա������ */
#define member               5                 /* Ԥ��λ�� */

/*
0��Ա				 ----> 			1��Ԥ��λ
1��Ա				 ----> 			2��Ԥ��λ
2��Ա				 ----> 			3��Ԥ��λ
.            ...        .........             
.            ...        .........  
member-1��Ա ----> member��Ԥ��λ
*/

/* FLASH��ز����ṹ�� */
typedef struct 
{
	uint32_t DRVROM_Step[member];   
	uint32_t DRVROM_Dir[member];
	uint32_t STROM_Step[member];
	uint32_t STROM_Dir[member];
	
}FLASH_PRESET;


extern FLASH_PRESET FLASH_DATA;
extern u8 restart;          //����������־

u8 STMFLASH_ErasePage(u32 paddr);		/* ����ҳ */

void STMFLASH_Read(u32 ReadAddr);		//��һҳԤ��λ������  ��ŵ�ram��
void STMFLASH_Writ( u32 ReadAddr );	//дһҳԤ��λ������  ��ŵ�rom��

void Write_EEPROM(u32 dir,u32 ADDRESS_X,u32 dat);//дһ��Ԥ��λֵ
void Read_DRV_EEPROM(u32 ReadAddr,u16 NumToRead);//DRV��һ��Ԥ��λֵ
void Read_ST_EEPROM(u32 ReadAddr,u16 NumToRead); //ST��һ��Ԥ��λֵ

#endif


