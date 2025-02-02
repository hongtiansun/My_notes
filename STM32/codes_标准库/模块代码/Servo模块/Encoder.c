#include "Encoder.h"

float Encoder_Count=0; //编码器计数变量

void Encoder_Init(){ //初始化编码器函数
	//配置时钟
	RCC_APB2PeriphClockCmd(ENCODERCC,ENABLE); // 编码器引脚时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); // AFIO时钟使能

	//配置GPIO
	GPIO_InitTypeDef GPIO_Structure; // GPIO结构体
	GPIO_Structure.GPIO_Mode=GPIO_Mode_IPU; // 上拉输入
	GPIO_Structure.GPIO_Pin=ENCODEPinA|ENCODEPinB; // A、B两个引脚
	GPIO_Structure.GPIO_Speed=GPIO_Speed_50MHz;// 50MHz
	GPIO_Init(ENCODEGPIO,&GPIO_Structure);// 初始化GPIO

	//配置AFIO
	GPIO_EXTILineConfig(AFIOPortSourceGPIO,AFIOPinASourceGPIO);
	GPIO_EXTILineConfig(AFIOPortSourceGPIO,AFIOPinBSourceGPIO);


	//配置EXTI
	EXTI_InitTypeDef EXTI_Structure;
	EXTI_Structure.EXTI_Line=EXTILineA|EXTILineB;
	EXTI_Structure.EXTI_LineCmd=ENABLE;
	EXTI_Structure.EXTI_Mode=EXTIMode;//中断模式
	EXTI_Structure.EXTI_Trigger=EXTITrigger;//下降沿触发
	EXTI_Init(&EXTI_Structure);

	//配置NVIC
	NVIC_PriorityGroupConfig(NVICGroup);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=NVICIRQChannelA;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVICPreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVICSubPriority;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVICGroup);
	NVIC_InitStructure.NVIC_IRQChannel=NVICIRQChannelB;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=NVICPreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=NVICSubPriority;
	NVIC_Init(&NVIC_InitStructure);
}

float Get_Encoder_Count(){
	return Encoder_Count;
}

void EXTIAIRQ(void){
	
	if(EXTI_GetITStatus(EXTILineA)==SET){
		if(GPIO_ReadInputDataBit(ENCODEGPIO,ENCODEPinB)==0){ // A下降沿 B为低电平 逆时针
			Encoder_Count=Encoder_Count-20;
			if(Encoder_Count<0)
				Encoder_Count=0;
			Delay_ms(10);
		}
		EXTI_ClearITPendingBit(EXTILineA);
	}
	
}
void EXTIBIRQ(void){
	
	if(EXTI_GetITStatus(EXTILineB)==SET){
		if(GPIO_ReadInputDataBit(ENCODEGPIO,ENCODEPinA)==0){ // B下降沿 A为低电平 顺时针
			Encoder_Count=Encoder_Count+20;
			if(Encoder_Count>180)
				Encoder_Count=180;
			Delay_ms(10);
		}
		EXTI_ClearITPendingBit(EXTILineB);
	}
	
}
