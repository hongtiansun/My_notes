#include "CountSensor.h"

uint16_t CountSensor_Count;

void CountSensor_Init(){
	//配置时钟
	RCC_APB2PeriphClockCmd(EXTIPinRCC,ENABLE); //产生中断引脚使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //中断选择引脚使能
	//配置GPIO
	GPIO_InitTypeDef GPIO_Structure; //配置GPIO结构体
	GPIO_Structure.GPIO_Mode=GPIO_Mode_IPU; //输入上拉
	GPIO_Structure.GPIO_Pin=EXTIPin; //引脚
	GPIO_Structure.GPIO_Speed=GPIO_Speed_50MHz;//速度
	GPIO_Init(EXTIPort,&GPIO_Structure);//初始化

	//配置AFIO
	GPIO_EXTILineConfig(AFIOPortSourceGPIO,AFIOExtiLineSource);//配置中断线

	//配置EXTI
	EXTI_InitTypeDef EXTI_Structure; //配置EXTI结构体
	EXTI_Structure.EXTI_Line=EXTILine; //中断线
	EXTI_Structure.EXTI_LineCmd=ENABLE;//使能中断线
	EXTI_Structure.EXTI_Mode=EXTIMode;//中断模式
	EXTI_Structure.EXTI_Trigger=EXTITRIG;//下降沿触发
	EXTI_Init(&EXTI_Structure);

	//配置NVIC
	NVIC_PriorityGroupConfig(NVICPeriphGroup); //配置中断优先级组别
	NVIC_InitTypeDef NVIC_InitStructure; //配置NVIC结构体
	NVIC_InitStructure.NVIC_IRQChannel=NVICPeriphChannel;//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVICPeriphPreemptionPriority;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVICPeriphSubPriority;//响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t Get_CountSensor_Count(){
	return CountSensor_Count;
}

void EXTIIRQHandler(void){//中断处理函数
	
	if(EXTI_GetITStatus(EXTILine)==SET){ //EXTI_GetITStatus(EXTI_Line13)判断是否产生中断
		CountSensor_Count++;
		EXTI_ClearITPendingBit(EXTILine); //清除中断标志
	}
	
}
