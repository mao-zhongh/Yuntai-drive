#ifndef _EEPROM_H
#define _EEPROM_H
#include "stm32f10x.h"
#include "main.h"
/*===============================2020/12/3.map===============================================

    Total RO  Size (Code + RO Data)                 7160 (   6.99kB)
    Total RW  Size (RW Data + ZI Data)             16648 (  16.26kB)
    Total ROM Size (Code + RO Data + RW Data)       7292 (   7.12kB)      

=================================掉电前写入地址=============================================*/
//定义EEPROM的模拟起始地址为FLASH的第255页的起始地址  一页2K == 2047字节
/*  记录DRV_关机前最后位离原点步数,方向 / 记录ST _关机前最后位离原点步数,方向  */
#define FLASH_Presetting       0x0807f800    // 
#define FLASH_Presetting_DRV   0x0807f800    // 注意：步数4字节+方向4字节 = 偏移+8
#define FLASH_Presetting_ST    0x0807f808    //
//#define Record_origin          0x0807f810    //

/*===================================各预置位地址=============================================*/

//  属于大容量 { 0x0800 0000 ~0x0807 F800 }  为了安全从0x0800 4000      128*256 = 32768
//一个预置位偏移8字节 , 例128/1  一圈步数 522240 高4位;   方向 低4位;
#define FLASH_start_ADDRESS  0x08004000        //该芯片FLASH起始地址

#define one_ADDRESS          0x08004000 		   //1号预置位
#define two_ADDRESS          0x08004010        //2号预置位
#define three_ADDRESS        0x08004020        //3号预置位
#define foue_ADDRESS				 0X08004030        //4号预置位
#define five_ADDRESS				 0X08004040        //5号预置位

/* 现5个预置位,增加预置位该成员数即可 */
#define member               5                 /* 预置位数 */

/*
0成员				 ----> 			1号预置位
1成员				 ----> 			2号预置位
2成员				 ----> 			3号预置位
.            ...        .........             
.            ...        .........  
member-1成员 ----> member号预置位
*/

/* FLASH相关参数结构体 */
typedef struct 
{
	uint32_t DRVROM_Step[member];   
	uint32_t DRVROM_Dir[member];
	uint32_t STROM_Step[member];
	uint32_t STROM_Dir[member];
	
}FLASH_PRESET;


extern FLASH_PRESET FLASH_DATA;
extern u8 restart;          //掉电重启标志

u8 STMFLASH_ErasePage(u32 paddr);		/* 擦除页 */

void STMFLASH_Read(u32 ReadAddr);		//读一页预置位的数据  存放到ram中
void STMFLASH_Writ( u32 ReadAddr );	//写一页预置位的数据  存放到rom中

void Write_EEPROM(u32 dir,u32 ADDRESS_X,u32 dat);//写一个预置位值
void Read_DRV_EEPROM(u32 ReadAddr,u16 NumToRead);//DRV读一个预置位值
void Read_ST_EEPROM(u32 ReadAddr,u16 NumToRead); //ST读一个预置位值

#endif


