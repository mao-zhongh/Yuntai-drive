#ifndef _EEPROM_H
#define _EEPROM_H
#include "stm32f10x.h"
#include "main.h"
/*===============================2020/12/3.map===============================================

    Total RO  Size (Code + RO Data)                 7160 (   6.99kB)
    Total RW  Size (RW Data + ZI Data)             16648 (  16.26kB)
    Total ROM Size (Code + RO Data + RW Data)       7292 (   7.12kB)

=================================2020/12/3=============================================*/
//定义EEPROM的模拟起始地址为FLASH的第255页的起始地址  一页2K == 510字节
#define FLASH_PAGE0_ADDR    0x08004000 //0x08007c00 //  属于大容量 { 0x0800 000 ~0x0807 ffff }  为了安全从0x0800 4000
#define FLASH_PAGE1_ADDR    0x08004010
#define FLASH_PAGE2_ADDR    0x08004020
//定于备份区域的大小为50个字节
//可以根据需要调整
#define STMFLASH_SIZE			1024

//void STMFLASH_Read_Backup(void);
//void STMFLASH_Write_NoErase(void);
////void STMFLASH_Read(u16 ReadAddr,u16 *pstr,u16 NumberToRead);

//void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);
//unsigned short IapReadByte(u32 addr,u16 *pstr);/*读一字节 8bit*/




//闪存页擦除
void Erasure_(u32 ADDR_);
u8 STMFLASH_WriteHalfWord(u32 faddr, u32 dat);//write
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);
u32 STMFLASH_ReadHalfWord(u32 faddr);//Read
u8 STMFLASH_ErasePage(u32 paddr);

u8 STMFLASH_Write(u32 ADDRESS_X,u32 *pBuffer[]);

unsigned char Write_EEPROM(u8 dir,u32 ADDRESS_X,u16 dat);


#endif


