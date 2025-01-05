#include "PWM.h"

void PWM_Init(){
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

	//配置引脚重映射
	/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//复用到A15
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭原ATAG调试功能*/
	
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
	//启动定时器
	TIM_Cmd(PWMTIM,ENABLE);
}

void PWM_SetCompare1(uint16_t Compare){ //设置通道1比较值
	TIM_SetCompare1(PWMTIM,Compare);
}
void PWM_SetCompare2(uint16_t Compare){//设置通道2比较值
	TIM_SetCompare2(PWMTIM,Compare);
}