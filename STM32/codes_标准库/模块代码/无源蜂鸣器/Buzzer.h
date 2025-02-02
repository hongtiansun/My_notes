#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f10x.h"
#include "Delay.h"
#include "Music.h"

void Buzzer_Init(void);
void Buzzer_Set(uint8_t value);
void Buzzer_Play(uint16_t note);
void Buzzer_PlayMusic(uint16_t* Music,uint8_t length,uint16_t Speed);
#endif