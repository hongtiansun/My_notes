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

### 简要介绍

OLED（Organic Light Emitting Diode）：有机发光二极管
OLED显示屏：性能优异的新型显示屏，具有功耗低、相应速度快、宽视角、轻薄柔韧等特点
0.96寸OLED模块：小巧玲珑、占用接口少、简单易用，是电子设计中非常常见的显示屏模块
供电：3~5.5V，通信协议：I2C/SPI，分辨率：128*64

![alt](./images/Snipaste_2025-01-04_16-46-13.png)

常见的调试方法:
- 串口调试
- 显示屏调试(OLED)
- Keil 调试
  
补充： 

    有机发光二极管，每个像素都可以自发光，对比需要背光的LCD，其功耗更低，刷新率更大，时角更宽(各个角度看更加清晰)

### 硬件电路

![alt](./images/Snipaste_2025-01-04_16-56-15.png)

### OLED 驱动函数

128 * 64 划分 8 * 16 小格，共64个。

![alt](./images/Snipaste_2025-01-04_16-57-56.png)

| 函数                                  | 作用                 |
| ------------------------------------- | -------------------- |
| OLED_Init();                          | 初始化               |
| OLED_Clear();                         | 清屏                 |
| OLED_ShowChar(1, 1, 'A');             | 显示一个字符         |
| OLED_ShowString(1, 3, "HelloWorld!"); | 显示字符串           |
| OLED_ShowNum(2, 1, 12345, 5);         | 显示十进制数字       |
| OLED_ShowSignedNum(2, 7, -66, 2);     | 显示有符号十进制数字 |
| OLED_ShowHexNum(3, 1, 0xAA55, 4);     | 显示十六进制数字     |
| OLED_ShowBinNum(4, 1, 0xAA55, 16);    | 显示二进制数字       |

### 补充C语言基础

#### C语言中.h 与 .c文件对应

在 C 语言中，.h 文件（头文件）和 .c 文件（源文件）通过包含预处理指令 #include 关联起来。
头文件通常包含函数声明、宏定义、类型定义和全局变量声明，而源文件包含函数定义和具体实现。
通过在源文件中包含头文件，可以使源文件知道头文件中声明的内容。

关联步骤：
1. 创建头文件：
头文件通常包含函数声明、宏定义、类型定义和全局变量声明。
使用包含保护（include guard）防止头文件被多次包含。

2. 创建源文件：
源文件包含头文件，并实现头文件中声明的函数。
定义头文件中声明的全局变量。

3. 在另一个源文件中使用头文件：
通过包含头文件，可以在其他源文件中使用头文件中声明的函数和变量。

4. 编译和链接：
编译器将每个源文件编译成目标文件（.o 或 .obj 文件）。
链接器将所有目标文件链接成一个可执行文件。

在 C 语言中，包含头文件（.h 文件）并不会直接定位到对应的源文件（.c 文件）。
头文件的作用是提供函数声明、宏定义、类型定义和全局变量声明，**使得包含该头文件的源文件能够知道这些声明的存在**。
实际的函数定义和全局变量定义是在 example.c 中实现的。
编译器会分别编译 main.c 和 example.c，生成对应的目标文件（如 main.o 和 example.o）。
链接器会将这些目标文件链接在一起，生成最终的可执行文件。

#### C语言中的const关键字
嵌入式中const修饰变量会开辟在Flash中，减少内部SRAM的空间使用。

在 C 语言中，使用 const 定义的变量在头文件中包含保护（#ifndef、#define、#endif）下仍然会导致重复定义错误，这是因为 const 变量在 C 语言中默认具有内部链接性（internal linkage）。
这意味着每个包含该头文件的源文件都会有一个独立的副本，从而导致重复定义。OLED_Font.h重复定义

要解决这个问题，可以使用 extern 关键字在头文件中声明 const 变量，并在一个源文件中定义它。
这样可以确保 const 变量只在一个源文件中定义，而在其他源文件中引用。

本项目的OLED 字模数组定义在.h文件中，我们只在OLED.c中包含一次即可。

const关键字特点：
1. 只读属性：
const 修饰的变量在初始化后不能被修改。
试图修改 const 变量的值会导致编译错误。

2. 编译时常量：
const 变量的值在编译时确定，并且在程序运行期间保持不变。

3. 作用域和链接性：
在函数内部定义的 const 变量具有自动存储期（automatic storage duration），即局部变量。
在文件作用域（全局作用域）定义的 const 变量具有内部链接性（internal linkage），即仅在定义它的源文件中可见。
可以使用 extern 关键字在头文件中声明 const 变量，并在一个源文件中定义它，以便在多个源文件中共享。

4. 指针和 const：
const 可以修饰指针本身或指针指向的内容，或者两者都修饰。
const int *ptr：指向 const int 的指针，不能通过 ptr 修改指向的值。
int *const ptr：const 指针，指针本身不能修改，但可以修改指向的值。
const int *const ptr：指向 const int 的 const 指针，指针和指向的值都不能修改。

const 修饰局部变量仍然存储在栈区，局部变量的生命周期是从声明开始到所在的代码块结束，不可修改。
const 修饰的全局变量通常存储在只读数据段（.rodata）中。
全局变量的生命周期是整个程序运行期间。
由于 const 修饰的全局变量是只读的，编译器通常会将它们放在只读数据段，以防止在运行时修改它们。(ROM中)

内存分区：
![alt](./images/A微信图片_内存分区.jpg)


## 五、中断系统

### 中断系统介绍

中断：在主程序运行过程中，出现了特定的中断触发条件（中断源），使得CPU暂停当前正在运行的程序，转而去处理中断程序，处理完成后又返回原来被暂停的位置继续运行

中断优先级：当有多个中断源同时申请中断时，CPU会根据中断源的轻重缓急进行裁决，优先响应更加紧急的中断源

中断嵌套：当一个中断程序正在运行时，又有新的更高优先级的中断源申请中断，CPU再次暂停当前中断程序，转而去处理新的中断程序，处理完成后依次进行返回

### 中断流程图解

![alt](./images/Snipaste_2025-01-04_19-45-57.png)

中断源触发中断 --> 中断向量表 --> 保护现场，执行默认程序 --> 中断处理程序 --> 恢复现场 --> 继续执行

### STM32中断介绍

中断类型：
68个可屏蔽中断通道，包含EXTI、TIM、ADC、USART、SPI、I2C、RTC等多个外设

中断管理：
使用NVIC统一管理中断，每个中断通道都拥有16个可编程的优先等级，可对优先级进行分组，进一步设置抢占优先级和响应优先级

中断向量表：
![alt](./images/Snipaste_2025-01-04_19-51-46.png)
![alt](./images/Snipaste_2025-01-04_19-52-40.png)
![alt](./images/Snipaste_2025-01-04_19-53-14.png)
![alt](./images/Snipaste_2025-01-04_19-53-34.png)

灰色部分为内核中断，白色部分为外设中断

### NVIC 控制器

NVIC为内核外设，辅助CPU工作，管理中断系统。
含有多个中断输入，经抉择后，单个输出到CPU。

基本结构：

![alt](./images/Snipaste_2025-01-04_19-56-07.png)

NVIC 优先级分组

NVIC的中断优先级由**优先级寄存器**的4位（0~15）决定，这4位可以进行切分，分为高n位的抢占优先级和低4-n位的响应优先级抢占优先级高的可以中断嵌套，响应优先级高的可以优先排队，抢占优先级和响应优先级均相同的按中断号排队

| 分组方式 | 抢占优先级  | 响应优先级  |
| -------- | ----------- | ----------- |
| 分组0    | 0位 取值为0 | 4位 0\~15   |
| 分组1    | 1位 0\~1    | 3位 0\~7    |
| 分组2    | 2位 0\~3    | 2位 0\~3    |
| 分组3    | 3位 0\~7    | 1位 0\~1    |
| 分组4    | 4位 0\~15   | 0位 取值为0 |

### EXIT简介

EXTI（Extern Interrupt）外部中断
EXTI可以监测指定GPIO口的电平信号，当其指定的GPIO口产生电平变化时，EXTI将立即向NVIC发出中断申请，经过NVIC裁决后即可中断CPU主程序，使CPU执行EXTI对应的中断程序

支持的触发方式：上升沿/下降沿/双边沿/软件触发
支持的GPIO口：所有GPIO口，但相同的Pin不能同时触发中断
通道数：16个GPIO_Pin，外加PVD输出、RTC闹钟、USB唤醒、以太网唤醒
触发响应方式：中断响应/事件响应

示例：
外部中断主要用于检测GPIO变化，触发的方式多种多样
PA0 PB0 PC0不能同时出发中断
PVD输出 RTC USB唤醒 ETH唤醒等主要用于电源电压检测，停止模式恢复，低功耗唤醒等功能

补充中断和事件区别：

    中断(CPU参与)
    中断是一种机制，当特定的事件发生时，微控制器会暂停当前的执行流程，跳转到一个预定义的中断服务程序（ISR）来处理该事件。
    中断通常用于处理需要立即响应的事件，如外部信号、定时器溢出等。
    优先级：中断具有优先级，可以配置不同中断的优先级，以便在多个中断同时发生时，优先处理高优先级的中断。
    中断向量表：每个中断都有一个对应的中断向量，指向中断服务程序的入口地址。
    中断服务程序（ISR）：当中断发生时，微控制器会跳转到对应的 ISR 执行处理代码。
    中断嵌套：高优先级的中断可以打断低优先级的中断服务程序，实现中断嵌套。

    事件(无CPU参与)
    事件是一种机制，用于在外设之间传递信号，而不需要 CPU 的干预。
    事件通常用于同步外设操作，如定时器、DMA 等。
    事件不会触发中断服务程序，而是直接在硬件层面进行处理。
    无优先级：事件没有优先级的概念，所有事件都是平等的。
    无中断服务程序：事件不会触发中断服务程序，而是直接在硬件层面进行处理。
    外设同步：事件通常用于外设之间的同步操作，如定时器触发 ADC 转换等。
    低功耗：事件机制可以在不唤醒 CPU 的情况下进行外设操作，有助于降低功耗。

EXTI 基本结构：

![alt](./images/Snipaste_2025-01-04_20-15-41.png)

图解：
1. 5~9 分配一个通道 10~15分配一个通道，这些引脚触发中断，对应的中断处理程序相同，可以在函数内部通过标志位区分
2. 触发事件(不经过CPU直接通往其他外设)
3. 数据选择器的Pin只有16个引脚，GPIOA/B/C均有16个引脚，故而三个公用一个

AFIO数据选择器：

作用一： 选择使用外部中断引脚
作用二：设置引脚复用

AFIO主要用于引脚复用功能的选择和重定义
在STM32中，AFIO主要完成两个任务：复用功能引脚重映射、中断引脚选择

![alt text](./images/image.png)

AFIO 寄存器：
相关EXTI选择的AFIO_EXTICR1\~4
每个寄存器配置四个EXTI 
EXTI0 \~ EXTI15
![alt](./images/Snipaste_2025-01-04_20-23-58.png)

故而共16个Pin通道，到达NVIC时5\~9 10\~15会发生合并

引脚复用功能图：
![alt](./images/Snipaste_2025-01-04_20-29-42.png)
![alt](./images/Snipaste_2025-01-04_20-29-53.png)

EXTI框图：

事件触发框图：
![alt](./images/Snipaste_2025-01-04_20-36-54.png)

中断触发图：
![alt](./images/Snipaste_2025-01-04_20-39-59.png)

### 旋转编码器介绍

旋转编码器：用来测量位置、速度或旋转方向的装置，当其旋转轴旋转时，其输出端可以输出与旋转速度和方向对应的方波信号，读取方波信号的频率和相位信息即可得知旋转轴的速度和方向
类型：机械触点式/霍尔传感器式/光栅式
![alt](./images/Snipaste_2025-01-04_20-51-52.png)

硬件电路：
![alt](./images/Snipaste_2025-01-04_20-55-17.png)

工作原理：
AB 相旋转编码器是一种常见的增量式编码器，用于测量旋转轴的位置、速度和方向。
它通过两个相位相差 90 度的方波信号（A 相和 B 相）来实现这些测量。

位置测量：
编码器的旋转轴每旋转一定角度，会产生一对 A 相和 B 相的方波信号。
通过计数 A 相或 B 相的脉冲数，可以确定旋转轴的位置。

速度测量：
通过测量单位时间内产生的脉冲数，可以计算旋转轴的速度。
速度 = 脉冲数 / 时间间隔。

方向测量：
通过比较 A 相和 B 相的相位关系，可以确定旋转方向。
如果 A 相领先 B 相，则表示顺时针旋转；如果 B 相领先 A 相，则表示逆时针旋转

![alt](./images/Snipaste_2025-01-04_21-05-38.png)

补充：

    我的工程中有主程序main.c以及用到的OLED.c模块，生成的main.o和OLED.o最终链接在一起，main.c中定义的宏，对OLED.c有效吗
    答：
    不可以。main.c 中定义的宏对 OLED.c 无效。
    每个源文件在编译时都是独立处理的，main.c 中定义的宏不会自动传递到 OLED.c 中。
    要使宏在多个文件中有效，可以在头文件中定义宏，并在需要使用宏的源文件中包含该头文件。

    条件编译发生在预处理阶段。预处理是编译过程的第一个阶段，在这个阶段，编译器会处理所有的预处理指令，例如 #include、#define、#ifdef、#ifndef 等。
    在main.c中没有定义宏 定义宏 包含了头文件，最后生成main.o 。
    在time.c中main.c的宏不会起到作用，故而又定义了宏包含了头文件，链接阶段时，main.o与time.o链接在一起。
    注意宏，条件编译只在预处理阶段起作用，达到main.o文件时，已经为二进制文件了。include 宏 只是声明作用，不起定义作用。故而可以重复包含，单个文件中不能重复包含，故而需要头文件守卫。
    宏、条件编译等均为编译器解析动作，汇编中没有体现。

    综上，.h文件只能含义声明语句，不能含有定义语句。

变量定义
`int a;`分配空间
变量声明
`extern int a;`声明a 不分配空间

函数定义
函数体；
函数声明
`int func (int,int);` 包含返回值类型 函数名 参数，只声明不分配空间。extern修饰表示外部函数。
## 六、定时器系统

### 定时器系统简介

- TIM（Timer）定时器
- 定时器可以对输入的时钟进行计数，并在计数值达到设定值时触发中断
- 16位计数器、预分频器、自动重装寄存器的时基单元，在72MHz计数时钟下可以实现最大59.65s的定时
- 不仅具备基本的定时中断功能，而且还包含内外时钟源选择、输入捕获、输出比较、编码器接口、主从触发模式等多种功能
- 根据复杂度和应用场景分为了高级定时器、通用定时器、基本定时器三种类型

定时器类型

| 类型       | 编号                   | 总线 | 功能                                                                                                 |
| ---------- | ---------------------- | ---- | ---------------------------------------------------------------------------------------------------- |
| 高级定时器 | TIM1、TIM8             | APB2 | 拥有通用定时器全部功能，并额外具有重复计数器、死区生成、互补输出、刹车输入等功能                     |
| 通用定时器 | TIM2、TIM3、TIM4、TIM5 | APB1 | 拥有基本定时器全部功能，并额外具有内外时钟源选择、输入捕获、输出比较、编码器接口、主从触发模式等功能 |
| 基本定时器 | TIM6、TIM7             | APB1 | 拥有定时中断、主模式触发DAC的功能                                                                    |

STM32C8T6 定时器资源有 TIM1\~4（一个高级三个通用）

### 定时器结构

#### 基本定时器

![alt](./images/Snipaste_2025-01-05_13-56-34.png)

PSC 预分频器： 16位，最大65536分频
CNT 计数器： 16位，最大0\~65535,只能向上计数
ARR ：16位，自动重装载

产生的定时器中断，进入NVIC选择，进入CPU中断系统
产生事件，可以出发其他外设工作

主从触发模式：
![alt](./images/Snipaste_2025-01-05_14-01-30.png)

#### 通用寄存器

时钟控制部分电路
![alt](./images/Snipaste_2025-01-05_14-16-11.png)
捕获比较中断电路
![alt](./images/Snipaste_2025-01-05_14-18-42.png)

主从触发模式：

主模式：
主模式（Master Mode）是指定时器作为主设备，控制其他定时器或外设的运行。
主模式下，定时器可以生成触发信号，触发其他定时器或外设的操作。
主模式通过配置定时器的主模式控制寄存器（TIMx_CR2）中的主模式选择位（MMS）来实现。常见的主模式包括：
- 复位模式（Reset Mode）：当定时器的计数器达到更新事件时，生成复位信号。
- 使能模式（Enable Mode）：当定时器的计数器使能时，生成使能信号。
- 更新模式（Update Mode）：当定时器的计数器达到更新事件时，生成更新信号。
- 比较脉冲模式（Compare Pulse Mode）：当定时器的输出比较事件发生时，生成比较脉冲信号。
```C
#include "stm32f10x.h"
void TIM2_MasterMode_Config(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 配置 TIM2 基本时间基
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置 TIM2 为主模式
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

    // 启动 TIM2
    TIM_Cmd(TIM2, ENABLE);
}

int main(void) {
    TIM2_MasterMode_Config();
    while (1) {
        // 主循环
    }
}
```
从模式：
从模式（Slave Mode）是指定时器作为从设备，受其他定时器或外设的控制。
定时器在从模式下，可以根据外部触发信号进行同步操作。
从模式通过配置定时器的从模式控制寄存器（TIMx_SMCR）中的从模式选择位（SMS）和触发选择位（TS）来实现。
常见的从模式包括：
- 关闭模式（Disabled Mode）：从模式关闭，定时器独立运行。
- 复位模式（Reset Mode）：当触发信号到来时，复位定时器的计数器。
- 门控模式（Gated Mode）：当触发信号有效时，定时器计数器运行；当触发信号无效时，定时器计数器停止。
- 触发模式（Trigger Mode）：当触发信号到来时，启动定时器的计数器。
- 外部时钟模式1（External Clock Mode 1）：使用外部触发信号作为定时器的时钟源。

```C
#include "stm32f10x.h"

void TIM3_SlaveMode_Config(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    // 配置 TIM3 基本时间基
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置 TIM3 为从模式，触发源为 TIM2
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1); // ITR1 对应 TIM2

    // 启动 TIM3
    TIM_Cmd(TIM3, ENABLE);
}

int main(void) {
    TIM2_MasterMode_Config();
    TIM3_SlaveMode_Config();
    while (1) {
        // 主循环
    }
}
```
主从触发：自己的触发事件触发自己复位，实现自动复位。

内部触发连接：
![alt](./images/Snipaste_2025-01-05_14-32-41.png)

#### 高级定时器

框图：
![alt](./images/Snipaste_2025-01-05_14-39-23.png)

### 定时器中断

![alt](./images/Snipaste_2025-01-05_14-42-10.png)

### 预分频器时序

![alt](./images/Snipaste_2025-01-05_14-49-19.png)

计数器计数频率： CK_CNT = CK_PSC / (PSC + 1) ;

### 计数器时序

![alt](./images/Snipaste_2025-01-05_14-52-45.png)

计数器溢出频率：CK_CNT_OV = CK_CNT / (ARR +1) = CK_PSC / (PSC + 1) / (ARR + 1);

### 计数器ARR 无预装寄存器时序

![alt](./images/Snipaste_2025-01-05_14-55-40.png)

写入新值后，立即起作用。没有影子寄存器

### 计数器ARR 有预装寄存器时序

![alt](./images/Snipaste_2025-01-05_14-57-23.png)

写入新值后，下一个时期才会起作用。
主要目的是为了保证时序的同步

### RCC 时钟数

main 函数在执行前，会运行SystemInit函数，主要作用是配置时钟树。

时钟信号产生电路
![alt](./images/Snipaste_2025-01-05_15-06-23.png)

CSS 时钟安全系统
8MHZ外部晶振 经锁相环倍频后得到721MHZ系统时钟，分配给AHB、APB1、APB2总线

当外部时钟损坏后，系统使用内部8MHZ的时钟弥补，在感官上，时钟慢了近十倍

时钟信号分配：
![alt](./images/Snipaste_2025-01-05_15-14-06.png)

APB1最大36MHZ APB2最大72MHZ
TIM固定72MHZ

## 七、定时器使用

### 定时器中断

![alt](./images/Snipaste_2025-01-05_15-18-29.png)

配置步骤：
- 配置RCC使能
- 配置内部时钟模式
- 配置时基单元
- 配置中断输出控制
- 配置NVIC 定时器中断
- 运行控制

配置函数：
```C
void TIM_DeInit(TIM_TypeDef* TIMx); //默认设置
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);//时基单元初始化
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);//使能控制
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);//结构体变量赋给默认值

void TIM_InternalClockConfig(TIM_TypeDef* TIMx);//内部时钟
void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource);//选择ITR定时器
void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter); //选择捕获通道时钟
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter);//选择外部时钟1 输入时钟
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, 
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter);//选择外部时钟2 输入时钟
void TIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter); //单独配置ETR引脚预分频器极性滤波器等

void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode);//设置实际单元分频值
void TIM_CounterModeConfig(TIM_TypeDef* TIMx, uint16_t TIM_CounterMode);//设置计数模式
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);//设置ARR模式 有预装 无预装
void TIM_SetCounter(TIM_TypeDef* TIMx, uint16_t Counter);//设置计数器值
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint16_t Autoreload);//设置ARR值
uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);//获取计数器值
uint16_t TIM_GetPrescaler(TIM_TypeDef* TIMx);//获取预分频器值
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);//获取标志位
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);//清楚标志位
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);//中断中获取
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);//中断中清除
```
TIM_IT 中断标志位：
TIM_IT_Update: 定时器更新中断源。当定时器计数器溢出或更新事件发生时触发。
TIM_IT_CC1: 定时器捕获比较1中断源。当捕获/比较寄存器1匹配计数器值时触发。
TIM_IT_CC2: 定时器捕获比较2中断源。当捕获/比较寄存器2匹配计数器值时触发。
TIM_IT_CC3: 定时器捕获比较3中断源。当捕获/比较寄存器3匹配计数器值时触发。
TIM_IT_CC4: 定时器捕获比较4中断源。当捕获/比较寄存器4匹配计数器值时触发。
TIM_IT_COM: 定时器换向中断源。用于高级控制定时器的换向事件。
TIM_IT_Trigger: 定时器触发中断源。当触发输入事件发生时触发。
TIM_IT_Break: 定时器断裂中断源。用于高级控制定时器的紧急停止事件。

时基单元结构体：
```C
typedef struct
{
  uint16_t TIM_Prescaler; //预分频器值
  uint16_t TIM_CounterMode;//计数模式 
  uint16_t TIM_Period; //ARR 自动重装载 
  uint16_t TIM_ClockDivision; //采样频率分频 
  uint8_t TIM_RepetitionCounter;  //重复计数器值（高级计数器才有）
} TIM_TimeBaseInitTypeDef;    
```

补充：滤波器

    滤波原理：
    固定时钟信号F采样，值相同，则稳定输出
    值并不相同，输出为0或者保持原样输出

    采样频率越高，采样点越多，滤波效果越好，对应的延迟也越大
    采样频率的来源：时钟分频 TIM_ClockDivision决定

static 修饰作用：

修饰局部变量： 内存申请在静态区，出函数后不会被销毁。
作用域不变，生命周期被延长

修饰全局变量： 全局变量本身有外部链接属性，可用extern 声明外部文件变量。static修饰以后，成为文件内部属性，不可被外联。
生命周期不变，作用域改变。

修饰函数： 同全局变量

补充： 刚一上电可能立即进入中断

    TIM_TimeBaseInit 函数中，会自动生成一个更新事件。
    来重装预分频器，更新事件与更新中断，同时产生。
    生成更新事件的同时，中断标志位置位1，因而复位时会立即进入中断，
    初始化时，需要清楚标志位。
    TIM_ClearFlag(TIM,TIM_FLAG_Update);

### 输出比较功能

#### 输出比较简介

OC（Output Compare）输出比较
输出比较可以通过比较CNT与CCR寄存器值的关系，来对输出电平进行置1、置0或翻转的操作，用于输出一定频率和占空比的PWM波形(输出比较)
每个高级定时器和通用定时器都拥有4个输出比较通道 (四个通道)
高级定时器的前3个通道额外拥有死区生成和互补输出的功能(用于驱动三相电机)

会对比CNT 与 CCR的值大小，通过大小比较输出0/1

#### PWM波形

PWM（Pulse Width Modulation）脉冲宽度调制
在具有惯性的系统中，可以通过对一系列脉冲的宽度进行调制，来等效地获得所需要的模拟参量，常应用于电机控速等领域
PWM参数：
- 周期 = TS
- 频率 = 1 / TS  频率越大，性能越平稳，功耗也越大。
- 占空比 = TON / TS 高电平占比，表示等效的模拟量
- 分辨率 = 占空比变化步距 表示占空比变化（调节步频）

图解：
![alt](./images/Snipaste_2025-01-05_17-21-19.png)

#### 输出比较输出PWM

输出比较通道
![alt](./images/Snipaste_2025-01-05_17-25-54.png)

输出比较模式
![alt](./images/Snipaste_2025-01-05_17-27-06.png)

PWM结构
![alt](./images/Snipaste_2025-01-05_17-30-37.png)

f频率可调，占空比可调

#### PWM参数计算

![alt](./images/Snipaste_2025-01-05_17-37-07.png)
PWM频率: Freq = CK_PSC / (PSC + 1) / (ARR + 1) (时钟频率)
PWM占空比： Duty = CCR / (ARR + 1) 
PWM分辨率： Reso = 1 / (ARR + 1) 

补充一下高级定时器输出比较：

![alt](./images/Snipaste_2025-01-05_17-39-11.png)

#### 舵机

舵机是一种根据输入PWM信号占空比来控制输出角度的装置
输入PWM信号要求：周期为20ms，高电平宽度为0.5ms~2.5ms

![alt](./images/Snipaste_2025-01-05_17-42-14.png)

硬件电路：
![alt](./images/Snipaste_2025-01-05_18-54-00.png)

#### 直流电机

直流电机是一种将电能转换为机械能的装置，有两个电极，当电极正接时，电机正转，当电极反接时，电机反转
直流电机属于大功率器件，GPIO口无法直接驱动，需要配合电机驱动电路来操作
TB6612是一款双路H桥型的直流电机驱动芯片，可以驱动两个直流电机并且控制其转速和方向

![alt](./images/Snipaste_2025-01-05_18-56-51.png)

硬件驱动电路：

![alt](./images/Snipaste_2025-01-05_19-01-15.png)

#### PWM 相关函数

```C
void TIM_ForcedOC1Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);//强制输出0/1
void TIM_ForcedOC2Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC3Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
void TIM_ForcedOC4Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);

void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);//设置CCR寄存器 带有影子寄存器
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);

void TIM_OC1FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast); //快速配置使能
void TIM_OC2FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC3FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);
void TIM_OC4FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);

void TIM_ClearOC1Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);//清楚REF信号
void TIM_ClearOC2Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC3Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);
void TIM_ClearOC4Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);

void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct); //输出比较初始化
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);

void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct); //结构体默认初始化

void TIM_OC1PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity); //配置输出有效极性
void TIM_OC1NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity); //互补通道设置

void TIM_CCxCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx);// 修改使能参数
void TIM_CCxNCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN);

void TIM_SelectOCxM(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode);//修改输出比较模式

void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);//设置占空比

```

输出比较结构体
```C
typedef struct
{
  uint16_t TIM_OCMode;        /*!< Specifies the TIM mode.
                                   This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  uint16_t TIM_OutputState;   /*!< Specifies the TIM Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_state */

  uint16_t TIM_OutputNState;  /*!< Specifies the TIM complementary Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_state
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint16_t TIM_Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register. 
                                   This parameter can be a number between 0x0000 and 0xFFFF */

  uint16_t TIM_OCPolarity;    /*!< Specifies the output polarity.
                                   This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  uint16_t TIM_OCNPolarity;   /*!< Specifies the complementary output polarity.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint16_t TIM_OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                                   This parameter can be a value of @ref TIM_Output_Compare_Idle_State
                                   @note This parameter is valid only for TIM1 and TIM8. */

  uint16_t TIM_OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
                                   @note This parameter is valid only for TIM1 and TIM8. */
} TIM_OCInitTypeDef;   
```

引脚复用：
![alt](./images/Snipaste_2025-01-05_19-13-08.png)
倘若PA0已经占用，可以重映射到替他位置。
![alt](./images/Snipaste_2025-01-05_19-16-16.png)
重映射需要利用AFIO
重映射：
```C
//配置引脚重映射
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //开启AFIO
GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);//TIM2CH1复用功能开启 复用到A15
GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭A15原本的ATAG调试功能使之成为普通io
```
函数描述：
```C
/**
  * @brief  Changes the mapping of the specified pin.
  * @param  GPIO_Remap: selects the pin to remap.
  *   This parameter can be one of the following values:
  *     @arg GPIO_Remap_SPI1             : SPI1 Alternate Function mapping
  *     @arg GPIO_Remap_I2C1             : I2C1 Alternate Function mapping
  *     @arg GPIO_Remap_USART1           : USART1 Alternate Function mapping
  *     @arg GPIO_Remap_USART2           : USART2 Alternate Function mapping
  *     @arg GPIO_PartialRemap_USART3    : USART3 Partial Alternate Function mapping
  *     @arg GPIO_FullRemap_USART3       : USART3 Full Alternate Function mapping
  *     @arg GPIO_PartialRemap_TIM1      : TIM1 Partial Alternate Function mapping
  *     @arg GPIO_FullRemap_TIM1         : TIM1 Full Alternate Function mapping
  *     @arg GPIO_PartialRemap1_TIM2     : TIM2 Partial1 Alternate Function mapping
  *     @arg GPIO_PartialRemap2_TIM2     : TIM2 Partial2 Alternate Function mapping
  *     @arg GPIO_FullRemap_TIM2         : TIM2 Full Alternate Function mapping
  *     @arg GPIO_PartialRemap_TIM3      : TIM3 Partial Alternate Function mapping
  *     @arg GPIO_FullRemap_TIM3         : TIM3 Full Alternate Function mapping
  *     @arg GPIO_Remap_TIM4             : TIM4 Alternate Function mapping
  *     @arg GPIO_Remap1_CAN1            : CAN1 Alternate Function mapping
  *     @arg GPIO_Remap2_CAN1            : CAN1 Alternate Function mapping
  *     @arg GPIO_Remap_PD01             : PD01 Alternate Function mapping
  *     @arg GPIO_Remap_TIM5CH4_LSI      : LSI connected to TIM5 Channel4 input capture for calibration
  *     @arg GPIO_Remap_ADC1_ETRGINJ     : ADC1 External Trigger Injected Conversion remapping
  *     @arg GPIO_Remap_ADC1_ETRGREG     : ADC1 External Trigger Regular Conversion remapping
  *     @arg GPIO_Remap_ADC2_ETRGINJ     : ADC2 External Trigger Injected Conversion remapping
  *     @arg GPIO_Remap_ADC2_ETRGREG     : ADC2 External Trigger Regular Conversion remapping
  *     @arg GPIO_Remap_ETH              : Ethernet remapping (only for Connectivity line devices)
  *     @arg GPIO_Remap_CAN2             : CAN2 remapping (only for Connectivity line devices)
  *     @arg GPIO_Remap_SWJ_NoJTRST      : Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
  *     @arg GPIO_Remap_SWJ_JTAGDisable  : JTAG-DP Disabled and SW-DP Enabled
  *     @arg GPIO_Remap_SWJ_Disable      : Full SWJ Disabled (JTAG-DP + SW-DP)
  *     @arg GPIO_Remap_SPI3             : SPI3/I2S3 Alternate Function mapping (only for Connectivity line devices)
  *                                        When the SPI3/I2S3 is remapped using this function, the SWJ is configured
  *                                        to Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST.   
  *     @arg GPIO_Remap_TIM2ITR1_PTP_SOF : Ethernet PTP output or USB OTG SOF (Start of Frame) connected
  *                                        to TIM2 Internal Trigger 1 for calibration (only for Connectivity line devices)
  *                                        If the GPIO_Remap_TIM2ITR1_PTP_SOF is enabled the TIM2 ITR1 is connected to 
  *                                        Ethernet PTP output. When Reset TIM2 ITR1 is connected to USB OTG SOF output.    
  *     @arg GPIO_Remap_PTP_PPS          : Ethernet MAC PPS_PTS output on PB05 (only for Connectivity line devices)
  *     @arg GPIO_Remap_TIM15            : TIM15 Alternate Function mapping (only for Value line devices)
  *     @arg GPIO_Remap_TIM16            : TIM16 Alternate Function mapping (only for Value line devices)
  *     @arg GPIO_Remap_TIM17            : TIM17 Alternate Function mapping (only for Value line devices)
  *     @arg GPIO_Remap_CEC              : CEC Alternate Function mapping (only for Value line devices)
  *     @arg GPIO_Remap_TIM1_DMA         : TIM1 DMA requests mapping (only for Value line devices)
  *     @arg GPIO_Remap_TIM9             : TIM9 Alternate Function mapping (only for XL-density devices)
  *     @arg GPIO_Remap_TIM10            : TIM10 Alternate Function mapping (only for XL-density devices)
  *     @arg GPIO_Remap_TIM11            : TIM11 Alternate Function mapping (only for XL-density devices)
  *     @arg GPIO_Remap_TIM13            : TIM13 Alternate Function mapping (only for High density Value line and XL-density devices)
  *     @arg GPIO_Remap_TIM14            : TIM14 Alternate Function mapping (only for High density Value line and XL-density devices)
  *     @arg GPIO_Remap_FSMC_NADV        : FSMC_NADV Alternate Function mapping (only for High density Value line and XL-density devices)
  *     @arg GPIO_Remap_TIM67_DAC_DMA    : TIM6/TIM7 and DAC DMA requests remapping (only for High density Value line devices)
  *     @arg GPIO_Remap_TIM12            : TIM12 Alternate Function mapping (only for High density Value line devices)
  *     @arg GPIO_Remap_MISC             : Miscellaneous Remap (DMA2 Channel5 Position and DAC Trigger remapping, 
  *                                        only for High density Value line devices)     
  * @param  NewState: new state of the port pin remapping.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
 ```

输出GPIO时，由定时器输出，故而配置成AF_PP功能。复用推挽输出

示例：
1KHZPWM波，占空比为50% 分辨率为1%
1KHZ = 72 000 000 / (PSC + 1) / (ARR + 1)
50% = CRR / (ARR + 1)
1% = 1 / (ARR + 1)

得PSC = 720
CRR = 50
ARR = 99



### 输入比较
