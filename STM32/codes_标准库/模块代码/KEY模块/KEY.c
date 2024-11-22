#include "stm32f10x.h"
#include "Delay.h"
void Key_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}

uint8_t Key_GetNum(){
	uint8_t KeyNum=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0){
		Delay_ms(20);//按下消抖
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);//等待松手
		Delay_ms(20);//松开消抖
		KeyNum=1;//表示按下
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0){
		Delay_ms(20);//按下消抖
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);//等待松手
		Delay_ms(20);//松开消抖
		KeyNum=2;//表示按下
	}
	return KeyNum;
}

