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

    解答了OLED.c中重复定义问题 ，头文件只能声明变量，不能定义变量。


