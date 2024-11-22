# STM32标准库整理

## 一、综述

### STM32简介

STM32 是ST公司基于ARMCortex-M内核开发的32位微控制器
STM32常应用在嵌入式领域，如智能车、无人机、机器人、无线通信、物联网、工业控制、娱乐电子产品等
STM32功能强大、性能优异、片上资源丰富、功耗低，是一款经典的嵌入式微控制器

STM32系列：

![alt](./images/Snipaste_2024-11-21_21-16-16.png)

### ARM介绍

ARM既指ARM公司，也指ARM处理器内核
- ARM公司是全球领先的半导体知识产权（IP）提供商，全世界超过95%的智能手机和平板电脑都采用ARM架构
- ARM公司设计ARM内核，半导体厂商完善内核周边电路并生产芯片

ARM架构系列：

![alt](./images/Snipaste_2024-11-21_21-19-12.png)

### 芯片介绍

#### 简介

- 系列：主流系列STM32F1
- 内核：ARM Cortex-M3
- 主频：72MHz
- RAM：20K（SRAM）
- ROM：64K（Flash）
- 供电：2.0~3.6V（标准3.3V）
- 封装：LQFP48

#### 外设资源：

![alt](./images/Snipaste_2024-11-21_21-21-33.png)

#### 芯片命名规则：

![alt](./images/Snipaste_2024-11-21_21-22-47.png)

#### 系统结构图：

![alt](./images/Snipaste_2024-11-21_21-23-56.png)

#### 引脚定义图

![alt](./images/Snipaste_2024-11-21_21-25-34.png)

#### 启动配置

![alt](./images/Snipaste_2024-11-21_21-27-19.png)

#### 最小系统电路原理图

![alt](./images/Snipaste_2024-11-21_21-28-25.png)

## 二、软件安装MDK

### 安装Keil5
拒绝解释，详细请网上教程

- 安装Keil5 MDK
- 安装器件支持包
- 软件注册
- 安装STLINK驱动
- 安装USB转串口驱动

### 工程架构

型号分类：

![alt](./images/Snipaste_2024-11-21_21-30-54.png)

新建工程：

- 建立工程文件夹，Keil中新建工程，选择型号
- 工程文件夹里建立Start、Library、User等文件夹，复制固件库里面的文件到工程文件夹
- 工程里对应建立Start、Library、User等同名称的分组，然后将文件夹内的文件添加到工程分组里
- 工程选项，C/C++，Include Paths内声明所有包含头文件的文件夹
- 工程选项，C/C++，Define内定义USE_STDPERIPH_DRIVER
- 工程选项，Debug，下拉列表选择对应调试器，Settings，Flash Download里勾选Reset and Run

工程架构：

![alt](./images/Snipaste_2024-11-21_21-33-00.png)

补充： 

    SystemInit 函数在嵌入式系统中非常重要，特别是对于像 STM32 系列的微控制器。
    它通常定义在 system_<device>.c 文件中，负责在主应用程序启动之前设置系统时钟和其他低级硬件配置。
    主要功能有：

    时钟配置：设置系统时钟源（如 HSI、HSE、PLL）并配置时钟树，包括 AHB、APB1 和 APB2 预分频器。
    外设初始化：初始化系统正常运行所需的基本外设。
    向量表重定位：设置向量表基地址，这对于中断处理非常重要。
    内存配置：如果使用外部内存，SystemInit 可能会配置内存控制器

SystemInit示例代码:
```C
void SystemInit(void) {
    /* 重置 RCC 时钟配置为默认复位状态 */
    RCC->CR |= (uint32_t)0x00000001; /* 设置 HSION 位 */
    RCC->CFGR &= (uint32_t)0xF8FF0000; /* 重置 SW、HPRE、PPRE1、PPRE2、ADCPRE 和 MCO 位 */
    RCC->CR &= (uint32_t)0xFEF6FFFF; /* 重置 HSEON、CSSON 和 PLLON 位 */
    RCC->CR &= (uint32_t)0xFFFBFFFF; /* 重置 HSEBYP 位 */
    RCC->CFGR &= (uint32_t)0xFF80FFFF; /* 重置 PLLSRC、PLLXTPRE、PLLMUL 和 USBPRE/OTGFSPRE 位 */
    RCC->CIR = 0x009F0000; /* 禁用所有中断并清除挂起位 */
    
    /* 配置系统时钟频率、HCLK、PCLK2 和 PCLK1 预分频器 */
    SetSysClock();
    
    /* 内部 FLASH 中的向量表重定位 */
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
}
```

STM32启动历程：

STM32 微控制器的启动历程是一个系统化的过程，涉及从复位到执行主程序的一系列步骤。以下是详细的启动过程：

1. 上电或复位
当 STM32 上电或复位时，系统首先会读取 BOOT 引脚（BOOT0 和 BOOT1）的状态。
这些引脚决定了启动模式，有三种主要启动模式：

从主 Flash 启动：这是最常见的启动模式，系统从内置 Flash 中的用户代码开始执行。
从系统存储器启动：用于 ISP 编程，通过内置的引导加载程序（Bootloader）从 UART、USB 等接口下载固件。
从嵌入式 SRAM 启动：用于调试，代码从 SRAM 中执行，但掉电后数据会丢失。

2. 向量表重定位
根据启动模式，向量表的基地址会被重定位到 0x00000000 地址。
对于从主 Flash 启动，向量表基地址为 Flash 的起始地址；
对于从系统存储器启动，基地址为系统存储器的起始地址。

3. 执行启动代码
处理器从 0x00000000 地址开始执行代码，这段代码通常是启动文件（bootloader）。
启动文件负责初始化系统，包括设置时钟、初始化堆栈和中断向量表等。

4. 系统初始化
启动文件中的 SystemInit 函数会配置系统时钟、初始化外设和设置向量表地址。例如：
```C
void SystemInit(void) {
    /* 配置系统时钟 */
    SetSysClock();
    /* 向量表重定位 */
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
}
```
5. 跳转到主程序
启动文件完成初始化后，会跳转到主程序的入口点（通常是 main 函数）。主程序开始执行用户代码。

详细步骤
复位向量：复位后，处理器会从向量表中读取复位向量，跳转到复位处理函数（Reset_Handler）。
堆栈指针初始化：从向量表中读取初始堆栈指针（SP）地址，并将其加载到 SP 寄存器中。
复位处理函数：Reset_Handler 函数会调用 SystemInit 进行系统初始化，然后调用 main 函数。
启动模式选择
BOOT0 = 0：从主 Flash 启动。
BOOT0 = 1：根据 BOOT1 的状态选择从系统存储器或 SRAM 启动。
启动文件的作用
启动文件的主要作用是初始化系统，包括：

配置时钟系统。
初始化堆栈和中断向量表。
初始化全局变量和静态变量。
清空未初始化的变量区（.bss 段）。
总结
STM32 的启动过程确保微控制器在每次上电或复位后都能以一致的方式启动和运行，提供了一个稳定的运行环境1。

STM32内嵌的Bootloader存放在系统存储区，由ST在生产线上写入，用于通过可用的串行接口对闪存存储器进行重新编程
STM32上电后也会执行一个汇编程序用于硬件的初始化。如`startup_stm32f4xx.s`

## 三、GPIO

### GPIO简介

- GPIO（General Purpose Input Output）通用输入输出口
- 可配置为8种输入输出模式
- 引脚电平：0V~3.3V，部分引脚可容忍5V
- 输出模式下可控制端口输出高低电平，用以驱动LED、控制蜂鸣器、模拟通信协议输出时序等
- 输入模式下可读取端口的高低电平或电压，用于读取按键输入、外接模块电平信号输入、ADC电压采集、模拟通信协议接收数据等

### 基本结构

![alt](./images/Snipaste_2024-11-22_12-24-01.png)

PA0~PA15 16个引脚 但是寄存器是32位寄存器 故而只有低16位有效
GPIOA/GPIOB均挂载在APB2外设总线上

### GPIO 位结构

输入结构分析：

![alt](./images/Snipaste_2024-11-22_12-30-44.png)

模块1：保护机制

当输入电压大于VDD3.3V时，打通保护二极管，钳位为0.7V故输入保持为4V
当输入电压小于0V时，下二极管导通，钳位为0.7，故保持为-0.7V维持稳定
当输入电压在0~3.3V之间，二极管处于截止区，断开，正常输入

模块2：默认电平设计

上开关闭合，默认输入会被上拉直VDD
下开关闭合，默认输入被下来至VSS
开关均断开，配置为浮空输入模式

模块3：

施密特触发器，对输入电平进行整形，提高边缘性

施密特触发器：V正与V负并不相同，边沿性好，不影响逻辑功能
常用于：
    
    1. 波形转化
    2. 检测过高幅或者过低幅
    3. 脉冲整形，得到干净方波

模块4：输入数据寄存器

输入信号由寄存器记录，最终单片机进行读取

模块5：片上外设

输入信号不流入寄存器，流入外设，实现复用功能
如硬件IIC

输出结构分析：

![alt](./images/Snipaste_2024-11-22_12-45-39.png)

模块1：单片机输出

通过设置寄存器直接输出到控制端控制
位寄存器可以直接设计某个位控制
而直接寄存器可以使用|=设置1 或者&=设置0

模块2：片上外设输出

片上外设输出信号，实现复用功能

模块3：输出控制 

可以控制输出为推挽输出还是开漏输出
又或者是关闭模式(输入模式)

补充：三极管与MOS管 与CMOS工艺

二极管：

![alt](./images/A微信图片_20241122140241.png)

![alt](./images/A微信图片_20241122140325.png)

三极管与MOS管：

![alt](./images/A微信图片_20241122140407.png)

![alt](./images/A微信图片_20241122140429.png)

![alt](./images/A微信图片_20241122140444.png)

CMOS反相器:

![alt](./images/A微信图片_20241122140500.png)

### GPIO模式

配置GPIO的端口配置寄存器，可以有以下八种模式：

![alt](./images/Snipaste_2024-11-22_14-08-33.png)

浮空/上拉/下拉输入

![alt](./images/Snipaste_2024-11-22_14-11-05.png)

模拟输入：

![alt](./images/Snipaste_2024-11-22_14-11-44.png)

如ADC配置

推挽开漏输出：

![alt](./images/Snipaste_2024-11-22_14-12-31.png)

复用开漏推挽：

![alt](./images/Snipaste_2024-11-22_14-13-10.png)

输入模式下，输出无效。同时输出模式下，输入也无效。

各个控制寄存器的外设地址可以查询STM32的数据手册

### LED和蜂鸣器介绍

- LED：发光二极管，正向通电点亮，反向通电不亮
- 有源蜂鸣器：内部自带振荡源，将正负极接上直流电压即可持续发声，频率固定
- 无源蜂鸣器：内部不带振荡源，需要控制器提供振荡脉冲才可发声，调整提供振荡脉冲的频率，可发出不同频率的声音

蜂鸣器需要驱动电路，因为单片机的引脚电流小，没有太强的驱动能力

![alt](./images/Snipaste_2024-11-22_14-14-59.png)

三极管增大驱动电流

驱动电路示例：

![alt](./images/Snipaste_2024-11-22_14-16-23.png)

### 按键及传感器模块

- 按键：常见的输入设备，按下导通，松手断开
- 按键抖动：由于按键内部使用的是机械式弹簧片来进行通断的，所以在按下和松手的瞬间会伴随有一连串的抖动

![alt](./images/Snipaste_2024-11-22_14-17-58.png)

存在按键抖动

传感器模块：
传感器元件（光敏电阻/热敏电阻/红外接收管等）的电阻会随外界模拟量的变化而变化，通过与定值电阻分压即可得到模拟电压输出，再通过电压比较器进行二值化即可得到数字电压输出

结构示意图：

![alt](./images/Snipaste_2024-11-22_14-25-57.png)
LED1为电源指示灯
LED2为DO指示灯 DO=0时，亮 DO=1时，灭
R5上拉电阻，可以保证默认输入为高电平

硬件电路：

![alt](./images/Snipaste_2024-11-22_14-29-01.png)

### 语言补充

类型：

![alt](./images/Snipaste_2024-11-22_14-32-12.png)

枚举：

![alt](./images/Snipaste_2024-11-22_14-32-50.png)

### 调试方式

- 串口调试：通过串口通信，将调试信息发送到电脑端，电脑使用串口助手显示调试信息

- 显示屏调试：直接将显示屏连接到单片机，将调试信息打印在显示屏上

- Keil调试模式：借助Keil软件的调试模式，可使用单步运行、设置断点、查看寄存器及变量等功能

### 代码函数：

三种外设RCC启动使能AHB / APB2 / APB1 总线
```C
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
```

GPIO库函数
```c
void GPIO_DeInit(GPIO_TypeDef* GPIOx); //复位
void GPIO_AFIODeInit(void); //AFIO复位 AFIO实现端口重映射以及中断引脚选择

void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);//结构体初始化
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);// 利用结构体赋给默认值

uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);// 读取某一位输入下
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);//读取某一组输入下
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//读取一位输出模式下
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);//读取一组输出模式下
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//设置某一位为1
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//设置某一位为0
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);//指定端口指定位 BitVal为一个枚举类型
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);//指定写入某一组
//解释见后文
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_EventOutputCmd(FunctionalState NewState);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_ETH_MediaInterfaceConfig(uint32_t GPIO_ETH_MediaInterface);
```

GPIO_InitTypeDef结构体
```c
typedef struct
{
  uint16_t GPIO_Pin;             /*!< Specifies the GPIO pins to be configured.
                                      This parameter can be any value of @ref GPIO_pins_define */

  GPIOSpeed_TypeDef GPIO_Speed;  /*!< Specifies the speed for the selected pins.
                                      This parameter can be a value of @ref GPIOSpeed_TypeDef */

  GPIOMode_TypeDef GPIO_Mode;    /*!< Specifies the operating mode for the selected pins.
                                      This parameter can be a value of @ref GPIOMode_TypeDef */
}GPIO_InitTypeDef;
```

## 四、OLED调试工具