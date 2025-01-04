接口调用：
```C
//接口定义
//配置GPIO引脚接口
#define EXTIPinRCC RCC_APB2Periph_GPIOB //中断引脚时钟
#define EXTIPin GPIO_Pin_13 //中断引脚
#define EXTIPort GPIOB //中断引脚组别
//配置AFIO引脚接口
#define AFIOPortSourceGPIO GPIO_PortSourceGPIOB //AFIO引脚组别
#define AFIOExtiLineSource GPIO_PinSource13 //AFIO中断引脚
//配置EXTI引脚接口
#define EXTILine EXTI_Line13 //中断线 可选参数 EXTI_Line0 EXTI_Line1 ... EXTI_Line15
#define EXTIMode EXTI_Mode_Interrupt //中断模式 可选参数 EXTI_Mode_Event
#define EXTITRIG EXTI_Trigger_Falling //中断触发方式 可选参数 EXTI_Trigger_Rising EXTI_Trigger_Rising_Falling
//配置NVIC引脚接口
#define NVICPeriphGroup NVIC_PriorityGroup_2 //中断优先级组别
#define NVICPeriphChannel EXTI15_10_IRQn //中断通道 可选参数 EXTI0_IRQn EXTI1_IRQn ... EXTI15_10_IRQn
#define NVICPeriphPreemptionPriority 1 //抢占优先级
#define NVICPeriphSubPriority 1 //响应优先级
//配置中断处理函数
#define EXTIIRQHandler EXTI15_10_IRQHandler //中断处理函数
```

外部中断初始化 时钟使能 配置GPIO 配置 AFIO 配置EXTI 配置NVIC
```C
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
```

中断处理函数
```C
void EXTIIRQHandler(void){//中断处理函数
	
	if(EXTI_GetITStatus(EXTILine)==SET){ //EXTI_GetITStatus(EXTI_Line13)判断是否产生中断
		CountSensor_Count++;
		EXTI_ClearITPendingBit(EXTILine); //清除中断标志
	}
	
}
```
