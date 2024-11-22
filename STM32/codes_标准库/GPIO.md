## GPIO输出初始化

```c
    //时钟初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    //GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure  GPIO_Mode=GPIO_Mode_Out_OD;//推挽输出
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;/    端口
    GPIO_InitStructure  GPIO_Speed=GPIO_Speed_50MHz;//速度
    GPIO_Init(GPIOA,&GPIO_InitStructure);
	
    //闪烁
    while(1){
    	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
    	Delay_ms(10);
    	GPIO_SetBits(GPIOA,GPIO_Pin_0);
    	Delay_ms(10);
    	GPIO_WriteBit(GPIOA,GPIO_Pin_0  Bit_RESET);
    	Delay_ms(10);
    	GPIO_WriteBit(GPIOA,GPIO_Pin_0  (BitAction)1);
    	Delay_ms(10);
    }   
    //流水灯
    while(1){
    	GPIO_Write(GPIOA,~0x0001);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0002);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0004);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0008);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0010);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0020);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0040);
    	Delay_ms(500);
    	GPIO_Write(GPIOA,~0x0080);
    	Delay_ms(500);
    }
```

## GPIO输入初始化

```c

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//时钟初始化
	
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_11;//端口
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;//速度
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);
```

整理出KEY与LED模块

