#include "Buzzer.h"

//定时器接口
//PWM接口设置
#define PWMRCC RCC_APB1Periph_TIM2 //定时器3
#define PWMTIM TIM2
#define PWMGPIO GPIOA
#define PWMGPIORCC RCC_APB2Periph_GPIOA
#define PWMpin GPIO_Pin_3 //TIM3_CH4
//TIM设置
#define TIMClockDivision TIM_CKD_DIV1
#define TIMCounterMode TIM_CounterMode_Up
#define TIMARR 100-1
#define TIMPsc 720-1 //频率计算公式：F=72000000/(TIMARR+1)/(TIMPsc+1) ARR代表了占空比100%时的计数值
//OC设置
#define TIMOCMode TIM_OCMode_PWM2 //PWM模式2 向上计数时，小于CRR时为无效电平 大于CRR时为有效电平 PWM模式1则相反
#define TIMOCPolarity TIM_OCPolarity_High //高电平有效
#define TIMOCCRR 80 //占空比计数值初始为0
#define TIMOCInit TIM_OC4Init //TIM3_CH4

void Buzzer_Init(){
	//开启时钟 TIM2 挂载在APB1总线上
	RCC_APB1PeriphClockCmd(PWMRCC,ENABLE);
	//选择时基时钟 内部时钟 (时钟源)
	TIM_InternalClockConfig(PWMTIM);

	//配置GPIOA引脚 PWM输出
	RCC_APB2PeriphClockCmd(PWMGPIORCC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin=PWMpin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(PWMGPIO,&GPIO_InitStructure);
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIMClockDivision;//采样频率分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIMCounterMode;//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=TIMARR;//ARR自动重装寄存器
	TIM_TimeBaseInitStructure.TIM_Prescaler=TIMPsc;//PSC预分频器值 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数器的值 高级定时器才有
	TIM_TimeBaseInit(PWMTIM,&TIM_TimeBaseInitStructure);
	TIM_ClearFlag(PWMTIM,TIM_FLAG_Update);//初始化完对中断标志位清零，防止复位时立即进入中断
	//配置输出比较单元
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);///变量太多 赋初值 
	TIM_OCInitStructure.TIM_OCMode=TIMOCMode; //设置模式
	TIM_OCInitStructure.TIM_OCPolarity=TIMOCPolarity;  //设置极性
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse=TIMOCCRR;  // 设置CRR
	TIMOCInit(PWMTIM,&TIM_OCInitStructure);
}
/*
@brief:设置蜂鸣器的音量大小
@param:value:音量大小 0-100
@retval:无
*/
void Buzzer_Set(uint8_t value){
    TIM_SetCompare4(PWMTIM,value); //设置CCR的值占空比 表示为音量大小 0-100
}
/*
@brief:播放音符
@param:note:音符
@retval:无
*/
void Buzzer_Play(uint16_t note){
	if(note==0){
		TIM_Cmd(PWMTIM,DISABLE);//关闭定时器
		return;
	}
	uint16_t Psc;
	TIM_Cmd(PWMTIM,DISABLE);//关闭定时器
	Psc = 72000000/(note*100)-1;// 72 000 000 / Psc+1 / 100 = note
	TIM_PrescalerConfig(PWMTIM,Psc,TIM_PSCReloadMode_Immediate);//设置预分频器
	TIM_Cmd(PWMTIM,ENABLE);//开启定时器
}

void Buzzer_PlayMusic(uint16_t* Music,uint8_t length,uint16_t Speed){
	uint8_t i = 0;
	for(i=0;i<length;i++){
		Buzzer_Play(Music[i]);
		Delay_ms(Speed);
	}
}



