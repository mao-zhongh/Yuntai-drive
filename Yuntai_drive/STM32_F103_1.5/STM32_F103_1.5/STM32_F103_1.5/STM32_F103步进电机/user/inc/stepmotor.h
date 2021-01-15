#ifndef _STEPMOTOR_H_
#define _STEPMOTOR_H_

#include "stm32f10x.h"
#define DIR_LEFT   0
#define DIR_RIGHT  1

extern u8 dir_mode;

void StepMotor_Lib_Init(void);

#endif