#ifndef _KEY_H
#define _KEY_H


#include "main.h"
#include "io_bit.h"



#define LED_1  PAout(8)
#define LED_2  PDout(2)


void LED_Iint(void);
void Key_Lib_Pin_Init(void);
#endif