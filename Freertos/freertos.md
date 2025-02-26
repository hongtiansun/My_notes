# FreeRTOS实时操作系统复习

## 一、笔记前言

### 三大要点：

• 讲解FreeRTOS 的常用API：理论、用法 
• 选择合适的硬件模块，展示这些API 的实例 
• 实现合适的小项目，展示工作中的编程方法

### 单片机程序设计模式

#### 裸机程序设计模式：

裸机程序的设计模式可以分为：轮询、前后台、定时器驱动、基于状态机。
前面三种方法都无法解决一个问题：假设有A 、B 两个都很耗时的函数，无法降低它们相互之间的影响。
第4 种方法可以解决这个问题，但是实践起来有难度。

假设一位职场妈妈需要同时解决2 个问题：给小孩喂饭、回复工作信息
各类模式下如何写程序呢？

轮询模式：
![alt](./images/Snipaste_2025-02-10_18-57-07.png)

使用轮询模式编写程序看起来很简单，但是要求while 循环里调用到的函数要执行得非常快，在复杂场景里反而增加了编程难度。

前后台模式：
所谓“前后台”就是使用中断程序。
![alt](./images/Snipaste_2025-02-10_18-58-32.png)
也可以该进程两个中断的形式：
![alt](./images/Snipaste_2025-02-10_18-59-21.png)

定时器驱动：
![alt](./images/Snipaste_2025-02-10_19-00-33.png)
这种模式适合调用周期性的函数，并且每一个函数执行的时间不能超过一个定时器周期。
如果“喂一口饭”很花时间，比如长达10 分钟，那么就会耽误“回一个信息”；
反过来也是一样的，如果“回一个信息”很花时间也会影响到“喂一口饭”；
这种场景下程序遭遇到了轮询模式的缺点：函数相互之间有影响。

基于状态机：
主函数：
![alt](./images/Snipaste_2025-02-10_19-02-34.png)
在main 函数里，还是使用轮询模式依次调用2 个函数。
关键在于这2 个函数的内部实现：使用状态机，每次只执行一个状态的代码，减少每次执行的时间
喂饭：
![alt](./images/Snipaste_2025-02-10_19-03-37.png)
![alt](./images/Snipaste_2025-02-10_19-03-51.png)

回信息：
![alt](./images/Snipaste_2025-02-10_19-04-10.png)
每个函数分成了四部分，每次执行只执行一部分。

#### 多任务系统

![alt](./images/Snipaste_2025-02-10_19-06-05.png)

示例代码：
![alt](./images/Snipaste_2025-02-10_19-06-37.png)
![alt](./images/Snipaste_2025-02-10_19-06-50.png)

多任务系统会依次给这些任务分配时间：你执行一会，我执行一会，如此循环。
只要切换的间隔足够短，用户会“感觉这些任务在同时运行

多任务系统中，多个任务可能会“同时”访问某些资源，需要增加保护措施以防止混乱。
比如任务A 、B 都要使用串口，能否使用一个全局变量让它们独占地、互斥地使用串口？ --> 任务互斥操作

如果任务之间有依赖关系，比如任务A 执行了某个操作之后，需要任务B 进行后续的处理。--> 任务同步操作

## 二、Freertos工程

### FreeRTOS目录结构

![alt](./images/Snipaste_2025-02-10_19-12-44.png)

---

主要涉及两个目录：
Demo：
Demo目录下是工程文件，以"芯片和编译器"组合成一个名字比如：CORTEX_STM32F103_Keil

Source：
根目录下是核心文件，这些文件是通用的
portable目录下是移植时需要实现的文件
目录名为：[compiler]\/[architecture]
比如：RVDS/ARM_CM3，这表示cortexM3架构在RVDS工具上的移植文件

 ---

核心文件：
FreeRTOS的最核心文件只有2个：
FreeRTOS/Source/tasks.c
FreeRTOS/Source/list.c

其他文件的作用也一起列表如下：
![alt](./images/Snipaste_2025-02-10_19-18-05.png)

 ---

移植FreeRTOS时涉及的文件放在FreeRTOS/Source/portable/[compiler]\/[architecture] 目录下，比如：RVDS/ARM_CM3，这表示cortexM3架构在RVDS或Keil工具上的移植文件。
里面有2个文件：port.c portmacro.h

 ---

FreeRTOS需要3个头文件目录：
- FreeRTOS本身的头文件：FreeRTOS/Source/include
- 移植时用到的头文件：FreeRTOS/Source/portable/[compiler]/[architecture]
- 含有配置文件FreeRTOSConfig.h的目录

![alt](./images/Snipaste_2025-02-10_19-21-39.png)

 ---

内存管理：

文件在FreeRTOS/Source/portable/MemMang 下，它也是放在portable 目录下，表示你可以提供自己的函数。
源码中默认提供了5个文件，对应内存管理的5种方法。

![alt](./images/Snipaste_2025-02-10_19-22-39.png)

### FreeRTOS Demo

FreeRTOS 系统文件
![alt](./images/Snipaste_2025-02-10_19-23-35.png)
![alt](./images/Snipaste_2025-02-10_19-24-46.png)

inc：
![alt](./images/Snipaste_2025-02-10_19-25-59.png)
port:
![alt](./images/Snipaste_2025-02-10_19-26-24.png)
src:
![alt](./images/Snipaste_2025-02-10_19-26-53.png)

Demo中必须包含FreeRTOS.h
![alt](./images/Snipaste_2025-02-10_19-28-28.png)

### 编程规范

补充知识：

    typedef定义函数指针类型：
    typedef int (*FuncPtr) (int,int);
    定义了FuncPtr类型的函数指针，返回值为int，参数为int，int
    
    typedef struct 结构体名 *指针名
    定义结构体指针变量,指针名变量是指向struct 结构体的一个指针

![alt](./images/Snipaste_2025-02-10_19-32-36.png)
![alt](./images/Snipaste_2025-02-10_19-37-38.png)
![alt](./images/Snipaste_2025-02-10_19-38-17.png)

#### 数据类型

每个移植的版本都含有自己的portmacro.h 头文件，里面定义了2个数据类型：

TickType_t：时钟终端次数的类型

- FreeRTOS配置了一个周期性的时钟中断：Tick Interrupt
- 每发生一次中断，中断次数累加，这被称为tick count
- tick count这个变量的类型就是TickType_t
- TickType_t可以是16位的，也可以是32位的
- FreeRTOSConfig.h中定义configUSE_16_BIT_TICKS时，TickType_t就是uint16_t
- 否则TickType_t就是uint32_t
- 对于32位架构，建议把TickType_t配置为uint32_t

BaseType_t：架构最高效的返回值类型(架构几位我几位)
- 这是该架构最高效的数据类型
- 32位架构中，它就是uint32_t
- 16位架构中，它就是uint16_t
- 8位架构中，它就是uint8_t
- BaseType_t通常用作简单的返回值的类型，还有逻辑值，比如pdTRUE/pdFALSE

#### 变量名

变量名前缀寓意类型：
![alt](./images/Snipaste_2025-02-10_21-35-18.png)

#### 函数名

函数名的前缀有2部分：返回值类型、在哪个文件定义。
![alt](./images/Snipaste_2025-02-10_21-36-31.png)

#### 宏名

宏的名字是大小，可以添加小写的前缀。
前缀是用来表示：宏在哪个文件中定义。

![alt](./images/Snipaste_2025-02-10_21-37-35.png)

通用宏定义：
![alt](./images/Snipaste_2025-02-10_21-38-01.png)

### Keil模拟器

编译仿真：
![alt](./images/Snipaste_2025-02-10_21-39-30.png)
可以在VIEW中看到各种外设状态：
![alt](./images/Snipaste_2025-02-10_21-40-14.png)
RUN运行即可：
![alt](./images/Snipaste_2025-02-10_21-40-37.png)

再次点击Debug即可退出。

注意：魔术棒工具Debug中选择仿真器。
![alt](./images/Snipaste_2025-02-10_21-45-30.png)

裸机分析仪：

常用的程序输出方式
串口：查看打印信息
逻辑分析仪：观察全局变量的波形，根据波形解析任务调度情况

1. 直接在代码中找到全局变量，右键添加到Analyzer，注意选择Bit观察
2. 也可以Setup创建添加

![alt](./images/Snipaste_2025-02-10_21-47-10.png)
![alt](./images/Snipaste_2025-02-10_21-47-54.png)

![alt](./images/Snipaste_2025-02-10_21-48-33.png)

## 三、内存管理

后续的章节涉及这些内核对象：task、queue、semaphores和event group等。
为了让FreeRTOS更容易使用，这些内核对象一般都是动态分配：用到时分配，不使用时释放。
使用内存的动态管理功能，简化了程序设计：不再需要小心翼翼地提前规划各类对象，简化API函数的涉及，甚至可以减少内存的使用。
内存的动态管理是C程序的知识范畴，并不属于FreeRTOS的知识范畴，但是它跟FreeRTOS关系是如此紧密，所以我们先讲解它。

在C语言的库函数中，有mallc、free等函数，但是在FreeRTOS中，它们不适用：
- 不适合用在资源紧缺的嵌入式系统中
- 这些函数的实现过于复杂、占据的代码空间太大
- 并非线程安全的(thread-safe)
- 运行有不确定性：每次调用这些函数时花费的时间可能都不相同
- 内存碎片化
- 使用不同的编译器时，需要进行复杂的配置
- 有时候难以调试

### 堆与栈

#### 堆

堆就是一块空闲的内存。
堆，heap，就是一块空闲的内存，需要提供管理函数
◼malloc：从堆里划出一块空间给程序使用
◼free：用完后，再把它标记为 空闲 的，可以再次使用
![alt](./images/Snipaste_2025-02-10_21-53-18.png)

堆的一个简单实现
```C
char heap_buf[1024]; //空闲内存
int pos = 0; //空闲内存首地址

void* My_malloc(int size){
    int old_pos = pos;
    pos += size;
    return &heap_buf[old_pos];
}

void my_free(void *buf){
    /*err*/
}

```

#### 栈

栈，stack，函数调用时局部变量保存在栈中，当前程序的环境也是保存在栈中
◼ 可以从堆中分配一块空间用作栈

![alt](./images/Snipaste_2025-02-10_21-56-32.png)

栈的作用：
情景：main 调用 a.fun a.fun 调用 b.fun
1. main调用a.fun时，需要记录函数返回的地址，将下一条指令的地址存入LR(Link Register),然后调用a.fun
2. a.fun 调用b.fun时同理，将下一条指令存入LR，调用b.fun

问题：LR会被覆盖 --> LR地址保存入栈

C函数的执行：
1. 划分栈空间(存入LR，局部变量)
2. LR，存入栈
3. 执行代码

LR存入的是返回地址，SP存入栈顶指针
![alt](./images/Snipaste_2025-02-10_23-29-15.png)

SP一开始执行初始栈顶,LR(main返回地址)，main的局部变量压入栈，栈顶为SP=SP-N，N字节空间分配给main。
执行到a.fun时，SP=SP-M,LR(a.fun返回地址)，a.fun的局部变量压入栈，执行b.fun。M字节的空间分配给a.fun。
执行到b.fun时，栈顶SP=SP-P，LR，b.fun压入栈，执行b.fun，P字节的空间分配给b.fun。

每个任务都有自己的独立的栈空间。(任务即线程)

### FreeRTOS中的内存管理

FreeRTOS中内存管理的接口函数为：pvPortMalloc 、vPortFree，对应于C库的malloc、free。 

文件在FreeRTOS/Source/portable/MemMang下，它也是放在portable目录下，表示你可以提供自己的函数。 
源码中默认提供了5个文件，对应内存管理的5种方法。 

![alt](./images/Snipaste_2025-02-11_13-31-34.png)

#### Heap_1

它只实现了pvPortMalloc，没有实现vPortFree。 
如果你的程序不需要删除内核对象，那么可以使用heap_1： 
⚫ 实现最简单 
⚫ 没有碎片问题 
⚫ 一些要求非常严格的系统里，不允许使用动态内存，就可以使用heap_1 

它的实现原理很简单，首先定义一个大数组,然后，对于pvPortMalloc调用时，从这个数组中分配空间。
![alt](./images/Snipaste_2025-02-11_13-33-52.png)

分配示意图：
![alt](./images/Snipaste_2025-02-11_13-34-29.png)

#### Heap_2

Heap_2之所以还保留，只是为了兼容以前的代码。新设计中不再推荐使用Heap_2。
建议使用Heap_4来替代Heap_2，更加高效。 
Heap_2也是在数组上分配内存，跟Heap_1不一样的地方在于： 
⚫ Heap_2使用**最佳匹配算法(best fit)**来分配内存 
⚫ 它支持vPortFree 

最佳匹配算法：
⚫ 假设heap有3块空闲内存：5字节、25字节、100字节 
⚫ pvPortMalloc 想申请20字节 
⚫ 找出最小的、能满足pvPortMalloc的内存：25字节 
⚫ 把它划分为20字节、5字节 
◼ 返回这20字节的地址 
◼ 剩下的5字节仍然是空闲状态，留给后续的pvPortMalloc使用 

与Heap_4相比，Heap_2不会合并相邻的空闲内存，所以Heap_2会导致严重的"碎片化"问题。 
但是，如果申请、分配内存时大小总是相同的，这类场景下Heap_2没有碎片化的问题。
所以它适合这种场景：频繁地创建、删除任务，但是任务的栈大小都是相同的(创建任务时，需要分配TCB和栈，TCB总是一样的)。 
虽然不再推荐使用heap_2，但是它的效率还是远高于malloc、free。 

分配示意图：
![alt](./images/Snipaste_2025-02-11_13-37-01.png)

#### Heap_3

Heap_3 使用标准C库里的malloc、free函数，所以堆大小由链接器的配置决定，配置项configTOTAL_HEAP_SIZE 不再起作用。 
C库里的malloc、free函数并非线程安全的，Heap_3中先暂停FreeRTOS的调度器，再去调用这些函数，使用这种方法实现了线程安全。 

#### Heap_4

跟Heap_1、Heap_2一样，Heap_4也是使用大数组来分配内存。 
Heap_4使用**首次适应算法(first fit)**来分配内存。
它还会把相邻的空闲内存合并为一个更大的空闲内存，这有助于较少内存的碎片问题。

首次适应算法：
⚫ 假设堆中有3块空闲内存：5字节、200字节、100字节 
⚫ pvPortMalloc 想申请20字节 
⚫ 找出第1个能满足pvPortMalloc的内存：200字节 
⚫ 把它划分为20字节、180字节 
⚫ 返回这20字节的地址 
⚫ 剩下的180字节仍然是空闲状态，留给后续的pvPortMalloc使用 

Heap_4会把相邻空闲内存合并为一个大的空闲内存，可以较少内存的碎片化问题。
适用于这种场景：频繁地分配、释放不同大小的内存。

Heap_4的使用过程举例如下： 
⚫ A：创建了3个任务 
⚫ B：删除了一个任务，空闲内存有2部分： 
⚫ 顶层的 
⚫ 被删除任务的TCB空间、被删除任务的Stack空间合并起来的 
⚫ C：分配了一个Queue，从第1个空闲块中分配空间 
⚫ D：分配了一个User数据，从Queue之后的空闲块中分配 
⚫ E：释放的Queue，User前后都有一块空闲内存 
⚫ F：释放了User数据，User前后的内存、User本身占据的内存，合并为一个大的空闲内存

![alt](./images/Snipaste_2025-02-11_13-42-16.png)

#### Heap_5

Heap_5 分配内存、释放内存的算法跟Heap_4是一样的。 
相比于Heap_4，Heap_5并不局限于管理一个大数组：它可以管理多块、分隔开的内存。 
在嵌入式系统中，内存的地址可能并不连续，这种场景下可以使用Heap_5。 
既然内存时分隔开的，那么就需要进行初始化：确定这些内存块在哪、多大： 
⚫ 在使用pvPortMalloc之前，必须先指定内存块的信息 
⚫ 使用vPortDefineHeapRegions 来指定这些信息

指定一块内存：
![alt](./images/Snipaste_2025-02-11_13-43-17.png)

指定多块内存：
![alt](./images/Snipaste_2025-02-11_13-43-41.png)

#### Os中动态内存管理办法

首次适应算法：
![alt](./images/Snipaste_2025-02-11_13-46-52.png)

最佳适应法：
![alt](./images/Snipaste_2025-02-11_13-47-25.png)

最坏适应：
![alt](./images/Snipaste_2025-02-11_13-48-16.png)

邻近适应算法：
![alt](./images/Snipaste_2025-02-11_13-48-57.png)

### Heap相关函数

---

pvPortMalloc与vPortFree
```C
void * pvPortMalloc( size_t xWantedSize ); 
void vPortFree( void * pv ); 
```
作用：分配内存、释放内存。 
如果分配内存不成功，则返回值为NULL。

---

xPortGetFreeHeapSize
```C
size_t xPortGetFreeHeapSize( void );
```
当前还有多少空闲内存，这函数可以用来优化内存的使用情况。比如当所有内核对象都
分配好后，执行此函数返回2000，那么configTOTAL_HEAP_SIZE就可减小2000。

---

xPortGetMinimumEverFreeHeapSize
```C
size_t xPortGetMinimumEverFreeHeapSize( void );
```
返回：程序运行过程中，空闲内存容量的最小值。 
注意：只有heap_4、heap_5支持此函数。 

---

malloc 失败的钩子函数
```C
void * pvPortMalloc( size_t xWantedSize )vPortDefineHeapRegions 
{ 
...... 
#if ( configUSE_MALLOC_FAILED_HOOK == 1 ) 

{ 
if( pvReturn == NULL ) 
{ 
extern void vApplicationMallocFailedHook( void ); 
vApplicationMallocFailedHook(); 
} 
} 
#endif 
return pvReturn;         
} 
```
所以，如果想使用这个钩子函数： 
⚫ 在FreeRTOSConfig.h 中，把configUSE_MALLOC_FAILED_HOOK 定义为1 
⚫ 提供vApplicationMallocFailedHook 函数 
⚫ pvPortMalloc 失败时，才会调用此函数

## 四、任务管理

简介：
在本章中，会涉及如下内容： 
⚫ FreeRTOS如何给每个任务分配CPU时间 
⚫ 如何选择某个任务来运行 
⚫ 任务优先级如何起作用 
⚫ 任务有哪些状态 
⚫ 如何实现任务 
⚫ 如何使用任务参数 
⚫ 怎么修改任务优先级 
⚫ 怎么删除任务 
⚫ 怎么实现周期性的任务 
⚫ 如何使用空闲任务 

### 任务创建与删除

在FreeRTOS 中，任务就是一个函数，原型如下： 
`void ATaskFunction( void *pvParameters );`

要注意的是： 
⚫ 这个函数不能返回 
⚫ 同一个函数，可以用来创建多个任务；换句话说，多个任务可以运行同一个函数 
⚫ 函数内部，尽量使用局部变量：

◼ 每个任务都有自己的栈 
◼ 每个任务运行这个函数时 

    ◆ 任务A的局部变量放在任务A的栈里、任务B的局部变量放在任务B的栈里 
    ◆ 不同任务的局部变量，有自己的副本 
◼ 函数使用全局变量、静态变量的话 

    ◆ 只有一个副本：多个任务使用的是同一个副本 
    ◆ 要防止冲突(后续会讲)

示例：
```C
void ATaskFunction( void *pvParameters ) 
{ 
    /* 对于不同的任务，局部变量放在任务的栈里，有各自的副本 */ 
    int32_t lVariableExample = 0; 
    /* 任务函数通常实现为一个无限循环 */ 
    for( ;; ) 
    { 
    /* 任务的代码 */ 
    } 
    /* 如果程序从循环中退出，一定要使用vTaskDelete删除自己 
    * NULL表示删除的是自己 
    */ 
    vTaskDelete( NULL ); 
    /* 程序不会执行到这里, 如果执行到这里就出错了 */
}
```

#### 任务创建

创建任务时可以使用2个函数：动态分配内存、静态分配内存。 

使用动态分配内存的函数如下：
![alt](./images/Snipaste_2025-02-11_14-07-30.png)

一字代表32位，4B

参数说明：
![alt](./images/Snipaste_2025-02-11_14-09-37.png)

使用静态分配内存的函数如下：
![alt](./images/Snipaste_2025-02-11_14-11-00.png)

参数说明：
![alt](./images/Snipaste_2025-02-11_14-11-32.png)
静态创建，需要TCB结构体与栈提前分配

采用静态分配需要增加宏开关，configSUPPORT_STATIC_ALLOCATION
还需要加写vApplicationGetIdleTaskMemory()函数

对于xTaskCreateStatic()静态创建任务，若令其回收必须记录其返回值。
动态创建将TCB存入handler参数中。

多个任务可以使用同一个函数，怎么体现它们的差别？ 
⚫ 栈不同 
⚫ 创建任务时可以传入不同的参数 

malloc分配内存：
![alt](./images/Snipaste_2025-02-11_14-27-11.png)

volatile 变量可变关键字，不允许编译器优化。

补充：静态创建与动态创建的区别

    动态创建任务使用 xTaskCreate 函数来创建任务。
    该函数在运行时从堆中分配内存来存储任务控制块（TCB）和任务堆栈。
    优点：
    简单易用，代码更简洁。
    内存分配在运行时进行，灵活性更高。
    缺点：
    依赖于堆内存管理，可能会导致内存碎片问题。
    如果堆内存不足，任务创建会失败。

    静态创建任务使用 xTaskCreateStatic 函数来创建任务。
    该函数需要在编译时分配内存来存储任务控制块（TCB）和任务堆栈。
    优点：
    不依赖于堆内存管理，避免了内存碎片问题。
    更适合内存受限的嵌入式系统。
    缺点：
    需要在编译时确定内存分配，灵活性较低。
    代码相对复杂，需要手动管理内存。

#### 任务删除

删除任务时使用的函数如下： 
`void vTaskDelete( TaskHandle_t xTaskToDelete );`

![alt](./images/Snipaste_2025-02-11_14-28-48.png)

### 任务优先级与Tick

#### 任务优先级

高优先级的任务先运行
优先级的取值范围是：0~(configMAX_PRIORITIES – 1)，数值越大优先级越高

FreeRTOS的调度器可以使用2种方法来快速找出优先级最高的、可以运行的任务。
使用不同的方法时，configMAX_PRIORITIES 的取值有所不同。

通用方法：
使用C函数实现，对所有的架构都是同样的代码。
对configMAX_PRIORITIES的取值没有限制。
但是configMAX_PRIORITIES的取值还是尽量小，因为取值越大越浪费内存，也浪费时间。
configUSE_PORT_OPTIMISED_TASK_SELECTION被定义为0、或者未定义时，使用此方法。

架构相关的优化的方法：
架构相关的汇编指令，可以从一个32位的数里快速地找出为1的最高位。
使用这些指令，可以快速找出优先级最高的、可以运行的任务。
使用这种方法时，configMAX_PRIORITIES的取值不能超过32。
configUSE_PORT_OPTIMISED_TASK_SELECTION被定义为1时，使用此方法。

FreeRTOS会确保最高优先级的、可运行的任务，马上就能执行
对于相同优先级的、可运行的任务，轮流执行

#### tick

FreeRTOS中也有心跳，它使用定时器产生固定间隔的中断。
这叫Tick、滴答，比如每10ms发生一次时钟中断。

假设t1、t2、t3发生时钟中断
两次中断之间的时间被称为时间片(time slice、tick period)
时间片的长度由configTICK_RATE_HZ 决定，假设configTICK_RATE_HZ为100，那么时间片长度就是10ms

![alt](./images/Snipaste_2025-02-24_15-18-57.png)

相同优先级任务的切换：
1. 任务2从t1执行到t2
2. 在t2发生tick中断，进入tick中断处理函数：
    选择下一个要运行的任务
    执行完中断处理函数后，切换到新的任务：任务1
3. 任务1从t2执行到t3
4. 从图中可以看出，任务运行的时间并不是严格从t1,t2,t3哪里开始

![alt](./images/Snipaste_2025-02-24_15-21-38.png)

有了Tick的概念后，我们就可以使用Tick来衡量时间了，比如：
```c
vTaskDelay(2); // 等待2个Tick，假设configTICK_RATE_HZ=100, Tick周期时10ms, 等待20ms

// 还可以使用pdMS_TO_TICKS宏把ms转换为tick
vTaskDelay(pdMS_TO_TICKS(100)); // 等待100ms
```
注意，基于Tick实现的延时并不精确，比如vTaskDelay(2) 的本意是延迟2个Tick周期，有可能经过1个Tick多一点就返回了。

使用vTaskDelay函数时，建议以ms为单位，使用pdMS_TO_TICKS把时间转换为Tick。
这样的代码就与configTICK_RATE_HZ无关，即使配置项configTICK_RATE_HZ改变了，我们也不用去修改代码。

#### 优先级修改

使用uxTaskPriorityGet来获得任务的优先级：
```c
UBaseType_t uxTaskPriorityGet( const TaskHandle_t xTask );
```
使用参数xTask来指定任务，设置为NULL表示获取自己的优先级。

使用vTaskPrioritySet 来设置任务的优先级：
```c
void vTaskPrioritySet( TaskHandle_t xTask,UBaseType_t uxNewPriority );
```
使用参数xTask来指定任务，设置为NULL表示设置自己的优先级；
参数uxNewPriority表示新的优先级，取值范围是0~(configMAX_PRIORITIES – 1)。

### 任务状态 

以前我们很简单地把任务的状态分为2中：运行(Runing)、非运行(Not Running)。
但是非运行态也可以细分
Task3执行vTaskDelay后：处于非运行状态，要过3秒种才能再次运行
Task3运行期间，Task1、Task2也处于非运行状态，但是它们随时可以运行
这两种"非运行"状态就不一样，可以细分为：
阻塞状态(Blocked)
暂停状态(Suspended)
就绪状态(Ready)

#### 阻塞状态

在实际产品中，我们不会让一个任务一直运行，而是使用"事件驱动"的方法让它运行：
任务要等待某个事件，事件发生后它才能运行
在等待事件过程中，它不消耗CPU资源
在等待事件的过程中，这个任务就处于阻塞状态(Blocked) **等待某个资源**

在阻塞状态的任务，它可以等待两种类型的事件：
**时间相关的事件**
可以等待一段时间：我等2分钟
也可以一直等待，直到某个绝对时间：我等到下午3点

**同步事件：这事件由别的任务，或者是中断程序产生**
例子1：任务A等待任务B给它发送数据
例子2：任务A等待用户按下按键

同步事件的来源有很多(这些概念在后面会细讲)：
队列(queue)
二进制信号量(binary semaphores)
计数信号量(counting semaphores)
互斥量(mutexes)
递归互斥量、递归锁(recursive mutexes)
事件组(event groups)
任务通知(task notifications)

在等待一个同步事件时，可以加上超时时间。比如等待队里数据，超时时间设为10ms：
10ms之内有数据到来：成功返回
10ms到了，还是没有数据：超时返回

#### 暂停状态

FreeRTOS中的任务也可以进入暂停状态，唯一的方法是通过vTaskSuspend函数。
```c
void vTaskSuspend( TaskHandle_t xTaskToSuspend );
```
参数xTaskToSuspend表示要暂停的任务，如果为NULL，表示暂停自己。

要退出暂停状态，只能由别人来操作：
    别的任务调用：vTaskResume
    中断程序调用：xTaskResumeFromISR

实际开发中，暂停状态用得不多。

#### 就绪状态

这个任务完全准备好了，随时可以运行：只是还轮不到它。这时，它就处于就绪态(Ready)。

#### 状态转移图

![alt](./images/Snipaste_2025-02-24_16-20-52.png)

操作系统七状态模型：

![alt](./images/Snipaste_2025-02-24_16-26-35.png)

补充函数：获取tickCount函数 t=xTaskGetTockCount()

### Delay函数

有两个Delay函数：
vTaskDelay：至少等待指定个数的Tick Interrupt才能变为就绪状态
vTaskDelayUntil：等待到指定的绝对时刻，才能变为就绪态。

函数原型：
```C
void vTaskDelay( const TickType_t xTicksToDelay );
/* xTicksToDelay: 等待多少给Tick */

/* pxPreviousWakeTime: 上一次被唤醒的时间
* xTimeIncrement: 要阻塞到(pxPreviousWakeTime + xTimeIncrement)
* 单位都是Tick Count
*/
BaseType_t xTaskDelayUntil( TickType_t * const pxPreviousWakeTime,const TickType_t xTimeIncrement );
```

说明：

使用vTaskDelay(n)时，进入、退出vTaskDelay的时间间隔至少是n个Tick中断

使用xTaskDelayUntil(&Pre, n)时，前后两次退出xTaskDelayUntil的时间至少是n个Tick中断
退出xTaskDelayUntil时任务就进入的就绪状态，一般都能得到执行机会
所以可以使用xTaskDelayUntil来让任务周期性地运行

图解：
![alt](./images/Snipaste_2025-02-24_16-34-53.png)
vTaskDelay的单位是tickcount，每次Delay的时间不定，需要转化为ms

![alt](./images/Snipaste_2025-02-24_16-37-00.png)
首个Pre需要自己设定，此后的可以自行更新
![alt](./images/Snipaste_2025-02-24_16-39-04.png)

Demo示例：
![alt](./images/Snipaste_2025-02-24_16-39-59.png)

![alt](./images/Snipaste_2025-02-24_16-42-22.png)

### 空闲任务及其钩子函数

#### 介绍

空闲任务(Idle任务)的作用：释放被删除的任务的内存。
一个良好的程序，它的任务都是事件驱动的：平时大部分时间处于阻塞状态。
有可能我们自己创建的所有任务都无法执行，但是调度器必须能找到一个可以运行的任务：所以，我们要提供空闲任务。
在使用vTaskStartScheduler() 函数来创建、启动调度器时，这个函数内部会创建空闲任务。

**空闲任务优先级为0：它不能阻碍用户任务运行
空闲任务要么处于就绪态，要么处于运行态，永远不会阻塞**

空闲任务的优先级为0，这以为着一旦某个用户的任务变为就绪态，那么空闲任务马上被切换出去，让这个用户任务运行。
在这种情况下，我们说用户任务"抢占"(pre-empt)了空闲任务，这是由调度器实现的。
要注意的是：如果使用vTaskDelete() 来删除任务，那么你就要确保空闲任务有机会执行，否则就无法释放被删除任务的内存。

补充： vTaskDelete(句柄) 不需要IdleTask回收内存，NULL自杀时需要IdleTask回收内存。
(保证IdleTask有执行的机会)

#### 钩子函数

FreeRTOS钩子函数（Hook Functions）是用户可以定义的函数，这些函数在特定事件发生时由**FreeRTOS内核**调用。

常见的钩子函数包括：
空闲钩子函数（Idle Hook）：当系统处于空闲状态时调用。
堆栈溢出钩子函数（Stack Overflow Hook）：当检测到任务堆栈溢出时调用。
内存分配失败钩子函数（Malloc Failed Hook）：当内存分配失败时调用。
任务切换钩子函数（Task Switch Hook）：在任务切换时调用。
这些钩子函数允许用户在特定事件发生时执行自定义代码，从而增强系统的可调试性和可靠性。

可以添加一个空闲任务的钩子函数(Idle Task Hook Functions)，空闲任务的循环没执行一次，就会调用一次钩子函数：
1. 执行一些低优先级的、后台的、需要连续执行的函数
2. 测量系统的空闲时间：空闲任务能被执行就意味着所有的高优先级任务都停止了，所以测量空闲任务占据的时间，就可以算出处理器占用率。
3. 让系统进入省电模式：空闲任务能被执行就意味着没有重要的事情要做，当然可以进入省电模式了。

钩子任务的限制：
不能导致空闲任务进入阻塞状态、暂停状态(idleTask只有就绪态和运行态)
如果你会使用vTaskDelete() 来删除任务，那么钩子函数要非常高效地执行。
如果空闲任务移植卡在钩子函数里的话，它就无法释放内存。
如果 vApplicationIdleHook 中的代码执行时间过长，Idle Task 就会长时间被占用，从而无法及时释放被删除任务的内存。

#### 钩子函数使用的前提

在FreeRTOS\Source\tasks.c 中，可以看到如下代码，所以前提就是：

1. 把这个宏定义为1：configUSE_IDLE_HOOK
2. 实现vApplicationIdleHook 函数

![alt](./images/Snipaste_2025-02-24_17-10-36.png)

### 调度算法

#### 基本概念

正在运行的任务，被称为"正在使用处理器"，它处于运行状态。
在单处理系统中，任何时间里只能有一个任务处于运行状态。
非运行状态的任务，它处于这3中状态之一：阻塞(Blocked)、暂停(Suspended)、就绪(Ready)。
就绪态的任务，可以被调度器挑选出来切换为运行状态，调度器永远都是挑选最高优先级的就绪态任务并让它进入运行状态。

阻塞状态的任务，它在等待"事件"，当事件发生时任务就会进入就绪状态。
事件分为两类：时间相关的事件、同步事件。
所谓时间相关的事件，就是设置超时时间：在指定时间内阻塞，时间到了就进入就绪状态。使用时间相关的事件，可以实现周期性的功能、可以实现超时功能。
同步事件就是：某个任务在等待某些信息，别的任务或者中断服务程序会给它发送信息。
怎么"发送信息"？方法很多，有：任务通知(task notification)、队列(queue)、事件组(event group)、信号量(semaphoe)、互斥量(mutex)等。
这些方法用来发送同步信息，比如表示某个外设得到了数据。

#### 配置调度算法

所谓调度算法，就是怎么确定哪个就绪态的任务可以切换为运行状态。
通过配置文件FreeRTOSConfig.h的两个配置项来配置调度算法
configUSE_PREEMPTION、
configUSE_TIME_SLICING

还有第三个配置项：configUSE_TICKLESS_IDLE，它是一个高级选项，用于关闭Tick中断来实现省电，后续单独讲解。
现在我们假设configUSE_TICKLESS_IDLE被设为0，先不使用这个功能。

调度算法的行为主要体现在两方面：
高优先级的任务先运行、同优先级的就绪态任务如何被选中。
调度算法要确保同优先级的就绪态任务，能"轮流"运行，策略是"轮转调度"(Round Robin Scheduling)。
轮转调度并不保证任务的运行时间是公平分配的，我们还可以细化时间的分配方法。

三个角度来配置调度算法：
可否抢占？高优先级的任务能否优先执行(配置项: **configUSE_PREEMPTION**)
1. 可以：被称作"可抢占调度"(Pre-emptive)，高优先级的就绪任务马上执行，下面再细化。
2. 不可以：不能抢就只能协商了，被称作"合作调度模式"(Co-operative Scheduling)
    当前任务执行时，更高优先级的任务就绪了也不能马上运行，只能等待当前任务主动让出CPU资源。
    其他同优先级的任务也只能等待：更高优先级的任务都不能抢占，平级的更应该老实点

可抢占的前提下，同优先级的任务是否轮流执行(配置项：**configUSE_TIME_SLICING**)
1. 轮流执行：被称为"时间片轮转"(Time Slicing)，同优先级的任务轮流执行，你执行一个时间片、我再执行一个时间片
2. 不轮流执行：英文为"without Time Slicing"，当前任务会一直执行，直到主动放弃、或者被高优先级任务抢占

在"可抢占"+"时间片轮转"的前提下，进一步细化：
空闲任务是否让步于用户任务(配置项：**configIDLE_SHOULD_YIELD**)
1. 空闲任务低人一等，每执行一次循环，就看看是否主动让位给用户任务
2. 空闲任务跟用户任务一样，大家轮流执行，没有谁更特殊
实现逻辑：
![alt](./images/Snipaste_2025-02-25_09-00-08.png)

各种配置列表：
![alt](./images/Snipaste_2025-02-25_08-57-32.png)

效果对比：

--- 

抢占时：高优先级任务就绪时，就可以马上执行
不抢占时：优先级失去意义了，既然不能抢占就只能协商了，任务1(优先级为0)一直在运行(没有主动放弃CPU)，其他任务都无法执行。
即使任务3(优先级2)的vTaskDelay 已经超时、即使它的优先级更高，都没办法执行。

---

时间片轮转：在Tick中断中会引起任务切换
时间片不轮转：高优先级任务就绪时会引起任务切换，高优先级任务不再运行时也会引起任务切换。
可以看到任务3就绪后可以马上执行，它运行完毕后导致任务切换。
其他时间没有任务切换，可以看到任务1、任务2都运行了很长时间。

---

让步时：在空闲任务的每个循环中，会主动让出处理器，从图中可以看到flagIdelTaskrun的波形很小
不让步时：空闲任务跟任务1、任务2同等待遇，它们的波形宽度是差不多的

#### OS调度算法整理

评价指标：
1. CPU利用率
2. 周转时间
3. 等待时间
4. 响应时间
5. 系统吞吐量

##### 先来先服务

角度：公平角度
规则：按照进程/作业的到达顺序进行服务
作用：
 1. 作业调度
 2. 进程调度

方式：非抢占时，只能主动放弃CPU
缺点：对排在长作业后的短作业，带权周转时间很大，对短作业体验非常不好，FCFS对长作业有利，对短作业不利
会导致饥饿问题

##### 短作业优先
角度：
- 追求最少得平均等待时间 
- 追求最少得平均周转时间 
- 追求最少得平均带权周转时间

规则：最短的作业/进程先得到服务

方式：
- 默认非抢占式，只能等待主动放弃CPU
- 抢占式，按照最短剩余时间排序

缺点：对短作业有利，长作业不利，会产生饥饿
优点：最短的平均等待时间和平均周转时间

##### 高响应比优先算法
角度：综合考虑作业/进程的等待时间和要求服务的时间
规则：调度时先计算各个作业的响应比，选择响应比最高的作业/进程进行服务
$\text{响应比} = \frac{\text{等待时间}+\text{要求服务时间}}{\text{要求服务时间}}$
作用：进程调度 作业调度
方式：非抢占式，只能等待主动放弃CPU
优点：
1. 综合考虑了等待时间和运行时间(要求服务时间)
2. 等待时间相同时，要求服务时间短的优先
3. 要求服务时间一样的话，等待时间短的优先
4. 避免了饥饿问题

以上三种方式，只适合早期的批处理系统
不适合于交互系统

##### 时间片轮转

常用于分时操作系统，注重响应时间(不关注周转时间)
公平，轮流为各个进程服务
规则：按照进程到达就绪队列的顺序，轮流执行一个时间片。若没有执行完,剥夺处理机，重新放到就绪队列重新排队
只适合于进程调度(作业调度不合适)
抢占式调度，通过时钟中断
优缺点：
优点：公平，响应快，适合分时操作系统
缺点：高频率的切换有一定开销 不区分任务的紧急程度
不会造成饥饿问题

##### 优先调度算法

适用于实时操作系统，按照任务的紧急程度决定处理顺序
规则：每个作业/任务有自己的优先级，调度时选择优先级最高的作业/进程(存在就绪队列)
作用：作业调度 进程调度 IO调度
方式：抢占式 非抢占式
优点：优先级区分紧急程度，重要程度。灵活调整对各种作业的偏好程度
可能造成饥饿问题

非抢占式：主动放弃CPU后，发生调度
抢占式：主动放弃CPU和就绪队列改变后发生调度 

就绪队列顺序：先看优先级再看到达顺序

补充：
1. 就绪队列不一定只有一个，可以按照不同的优先级组织就绪队列，也可把优先级更高的放在对头位置
2. 优先级可以分分成
	静态优先级：创建后确定，一直不变
	动态优先级：创建后初始值，之后情况动态调整
3. 优先级设置
   1. 系统进程高于用户进程
   2. 前台进程高于后台进程

##### 多级反馈队列调度算法

会导致饥饿问题

![alt](./images/Snipaste_2025-02-25_09-44-16.png)

综合了先来先服务，短作业优先，时间片轮转和优先调度算法的优点。

## 五、同步互斥通信

可以把多任务系统当做一个团队，里面的每一个任务就相当于团队里的一个人。
团队成员之间要协调工作进度(同步)、争用会议室(互斥)、沟通(通信)。
多任务系统中所涉及的概念，都可以在现实生活中找到例子。
各类RTOS都会涉及这些概念：任务通知(task notification)、队列(queue)、事件组(event group)、信号量(semaphoe)、互斥量(mutex)等。
我们先站在更高角度来讲解这些概念。

### 同步与互斥

假设有A、B两人早起抢厕所，A先行一步占用了；B慢了一步，于是就眯一会；
当A用完后叫醒B，B也就愉快地上厕所了。
在这个过程中，A、B是互斥地访问“厕所”，“厕所”被称之为临界资源。
我们使用了“休眠-唤醒”的同步机制实现了“临界资源”的“互斥访问”。
同一时间只能有一个人使用的资源，被称为临界资源。
比如任务A、B都要使用串口来打印，串口就是临界资源。
如果A、B同时使用串口，那么打印出来的信息就是A、B混杂，无法分辨。
所以使用串口时，应该是这样：A用完，B再用；B用完，A再用。

同步示例：
![alt](./images/Snipaste_2025-02-25_09-53-40.png)

互斥示例：
![alt](./images/Snipaste_2025-02-25_09-55-05.png)

FreeRTOS中的线程通信： 可以通过全局变量通信。

### 方法对比

能实现同步、互斥的内核方法有：
任务通知(task notification)、队列(queue)、事件组(event group)、信号量(semaphoe)、互斥量(mutex)。
它们都有类似的操作方法：获取/释放、阻塞/唤醒、超时。
比如：
- A获取资源，用完后A释放资源
- A获取不到资源则阻塞，B释放资源并把A唤醒
- A获取不到资源则阻塞，并定个闹钟；A要么超时返回，要么在这段时间内因为B释放资源而被唤醒。
![alt](./images/Snipaste_2025-02-25_10-22-27.png)

队列：
里面可以放任意数据，可以放多个数据
任务、ISR都可以放入数据；任务、ISR都可以从中读出数据

事件组：
一个事件用一bit表示，1表示事件发生了，0表示事件没发生
可以用来表示事件、事件的组合发生了，不能传递数据
有广播效果：事件或事件的组合发生了，等待它的多个任务都会被唤醒

信号量：
核心是"计数值"
任务、ISR释放信号量时让计数值加1
任务、ISR获得信号量时，让计数值减1

任务通知：
核心是任务的TCB里的数值
会被覆盖
发通知给谁？必须指定接收任务
只能由接收任务本身获取该通知

互斥量：
数值只有0或1
谁获得互斥量，就必须由谁释放同一个互斥量

![alt](./images/A01.jpg)

## 六、队列

队列(queue)可以用于"任务到任务"、"任务到中断"、"中断到任务"直接传输信息。

本章涉及如下内容：
- 怎么创建、清除、删除队列
- 队列中消息如何保存
- 怎么向队列发送数据、怎么从队列读取数据、怎么覆盖队列的数据
- 在队列上阻塞是什么意思
- 怎么在多个队列上阻塞
- 读写队列时如何影响任务的优先级

### 队列特性

#### 基本操作

队列的简化操如入下图所示，从此图可知：
1. 队列可以包含若干个数据：队列中有若干项，这被称为"长度"(length)
2. 每个数据大小固定
3. 创建队列时就要指定长度、数据大小
4. 数据的操作采用先进先出的方法(FIFO，First In First Out)：写数据时放到尾部，读数据时从头部读
5. 也可以强制写队列头部：覆盖头部数据

![alt](./images/Snipaste_2025-02-25_10-41-23.png)

详细操作如下图：
![alt](./images/A02.jpg)

#### 传输数据两种方式

使用队列传输数据时有两种方法：
- 拷贝：把数据、把变量的值复制进队列里
- 引用：把数据、把变量的地址复制进队列里

FreeRTOS拷贝值的方式：
- 局部变量的值可以发送到队列中，后续即使函数退出、局部变量被回收，也不会影响队列中的数据(**复制**)
- 无需分配buffer来保存数据，队列中有buffer
- 局部变量可以马上再次使用
- 发送任务、接收任务解耦：接收任务不需要知道这数据是谁的、也不需要发送任务来释放数据
- 如果数据实在太大，你还是可以使用队列传输它的地址(**传输结构体数组等**)
- 队列的空间有FreeRTOS内核分配，无需任务操心
- 对于有内存保护功能的系统，如果队列使用引用方法，也就是使用地址，必须确保双方任务对这个地址都有访问权限。使用拷贝方法时，则无此限制：内核有足够的权限，把数据复制进队列、再把数据复制出队列。

#### 队列的阻塞访问

只要知道队列的句柄，谁都可以读、写该队列。
任务、ISR都可读、写队列。可以多个任务读写队列。

任务读写队列时，简单地说：如果读写不成功，则阻塞；可以指定超时时间。
口语化地说，就是可以定个闹钟：如果能读写了就马上进入就绪态，否则就阻塞直到超时。

比如：某个任务读队列时，如果队列没有数据，则该任务可以进入阻塞状态：还可以指定阻塞的时间。
如果队列有数据了，则该阻塞的任务会变为就绪态。
如果一直都没有数据，则时间到之后它也会进入就绪态。

既然读取队列的任务个数没有限制，那么当多个任务读取空队列时，这些任务都会进入阻塞状态：有多个任务在等待同一个队列的数据。
当队列中有数据时，哪个任务会进入就绪态？
分配的两个原则:
- 优先级最高的任务
- 如果大家的优先级相同，那等待时间最久的任务会进入就绪态

跟读队列类似，一个任务要写队列时，如果队列满了，该任务也可以进入阻塞状态：还可以指定阻塞的时间。
如果队列有空间了，则该阻塞的任务会变为就绪态。
如果一直都没有空间，则时间到之后它也会进入就绪态。

既然写队列的任务个数没有限制，那么当多个任务写"满队列"时，这些任务都会进入阻塞状态：有多个任务在等待同一个队列的空间。
当队列中有空间时，哪个任务会进入就绪态？
- 优先级最高的任务
- 如果大家的优先级相同，那等待时间最久的任务会进入就绪态

### 队列函数

使用队列的流程：创建队列、写队列、读队列、删除队列。

#### 创建队列

队列的创建有两种方法：动态分配内存、静态分配内存，
动态分配内存：xQueueCreate，队列的内存在函数内部动态分配
```C
QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );
```
![alt](./images/Snipaste_2025-02-25_10-57-01.png)

| 参数          | 说明                                                                 |
| ------------- | -------------------------------------------------------------------- |
| uxQueueLength | 队列长度，最多能存放多少个数据(item)                                 |
| uxItemSize    | 每个数据(item)的大小：以字节为单位                                   |
| 返回值        | 非0：成功，返回句柄，以后使用句柄来操作队列 NULL：失败，因为内存不足 |

静态分配内存：xQueueCreateStatic，队列的内存要事先分配好
```C
QueueHandle_t xQueueCreateStatic(UBaseType_t uxQueueLength,
UBaseType_t uxItemSize,
uint8_t *pucQueueStorageBuffer,
StaticQueue_t *pxQueueBuffer
);
```
![alt](./images/A03.png)

| 参数                  | 说明                                                                                                          |
| --------------------- | ------------------------------------------------------------------------------------------------------------- |
| uxQueueLength         | 队列长度，最多能存放多少个数据(item)                                                                          |
| uxItemSize            | 每个数据(item)的大小：以字节为单位                                                                            |
| pucQueueStorageBuffer | 如果uxItemSize非0，pucQueueStorageBuffer必须指向一个uint8_t数组，此数组大小至少为"uxQueueLength * uxItemSize" |
| pxQueueBuffer         | 必须执行一个StaticQueue_t结构体，用来保存队列的数据结构                                                       |
| 返回值                | 非0：成功，返回句柄，以后使用句柄来操作队列 NULL：失败，因为pxQueueBuffer为NULL                               |

![](images/APi-2025-02-25-11-17-34.png)

#### 队列复位

队列刚被创建时，里面没有数据；使用过程中可以调用xQueueReset() 把队列恢复为初始状态，此函数原型为：

```C
/* pxQueue : 复位哪个队列;
* 返回值: pdPASS(必定成功)
*/
BaseType_t xQueueReset( QueueHandle_t pxQueue);
```

#### 队列删除
删除队列的函数为vQueueDelete() ，只能删除使用动态方法创建的队列，它会释放内存。原型如下：
```C
void vQueueDelete( QueueHandle_t xQueue );
```

#### 写队列
可以把数据写到队列头部，也可以写到尾部。
这些函数有两个版本：在任务中使用、在ISR中使用。
函数原型如下：
```C
/* 等同于xQueueSendToBack
* 往队列尾部写入数据，如果没有空间，阻塞时间为xTicksToWait
*/
BaseType_t xQueueSend(QueueHandle_t xQueue,
const void *pvItemToQueue,
TickType_t xTicksToWait
);

/*
* 往队列尾部写入数据，如果没有空间，阻塞时间为xTicksToWait
*/
BaseType_t xQueueSendToBack(QueueHandle_t xQueue,
const void *pvItemToQueue,
TickType_t xTicksToWait
);

/*
* 往队列尾部写入数据，此函数可以在中断函数中使用，不可阻塞
*/
BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue,
const void *pvItemToQueue,
BaseType_t *pxHigherPriorityTaskWoken
);
/*
* 往队列头部写入数据，如果没有空间，阻塞时间为xTicksToWait
*/
BaseType_t xQueueSendToFront(QueueHandle_t xQueue,
const void *pvItemToQueue,
TickType_t xTicksToWait
);
/*
* 往队列头部写入数据，此函数可以在中断函数中使用，不可阻塞
*/
BaseType_t xQueueSendToFrontFromISR(QueueHandle_t xQueue,
const void *pvItemToQueue,
BaseType_t *pxHigherPriorityTaskWoken
);
```

| 参数          | 说明                                                                                                                           |
| ------------- | ------------------------------------------------------------------------------------------------------------------------------ |
| xQueue        | 队列句柄，要写哪个队列                                                                                                         |
| pvItemToQueue | 数据指针，这个数据的值会被复制进队列，复制多大的数据？在创建队列时已经指定了数据大小                                           |
| xTicksToWait  | 如果队列满则无法写入新数据，可以让任务进入阻塞状态，                                                                           |
| xTicksToWait  | 表示阻塞的最大时间(Tick Count)。如果被设为0，无法写入数据时函数会立刻返回；如果被设为portMAX_DELAY，则会一直阻塞直到有空间可写 |
| 返回值        | pdPASS：数据成功写入了队列 errQUEUE_FULL：写入失败，因为队列满了。                                                             |

#### 读队列
使用xQueueReceive() 函数读队列，读到一个数据后，队列中该数据会被移除。
这个函数有两个版本：在任务中使用、在ISR中使用。
函数原型如下：
```C
BaseType_t xQueueReceive( QueueHandle_t xQueue,
void * const pvBuffer,
TickType_t xTicksToWait );

BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue,
void *pvBuffer,
BaseType_t *pxTaskWoken
);
```
| 参数         | 说明                                                                                                                                                                                     |
| ------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| xQueue       | 队列句柄，要读哪个队列                                                                                                                                                                   |
| pvBuffer     | bufer指针，队列的数据会被复制到这个buffer 复制多大的数据？在创建队列时已经指定了数据大小                                                                                                 |
| xTicksToWait | 队列空则无法读出数据，可以让任务进入阻塞状态，xTicksToWait表示阻塞的最大时间(Tick Count)。如果被设为0，无法读出数据时函数会立刻返回；如果被设为portMAX_DELAY，则会一直阻塞直到有数据可写 |
| 返回值       | pdPASS：从队列读出数据入 errQUEUE_EMPTY：读取失败，因为队列空了。                                                                                                                        |
#### 查询

可以查询队列中有多少个数据、有多少空余空间。
函数原型如下：

```C
/*
* 返回队列中可用数据的个数
*/
UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue );
/*
* 返回队列中可用空间的个数
*/
UBaseType_t uxQueueSpacesAvailable( const QueueHandle_t xQueue );
```

#### 覆盖/偷看

当队列长度为1时，可以使用xQueueOverwrite() 或xQueueOverwriteFromISR() 来覆盖数据。
注意，队列长度必须为1。如果队列长度大于 1，xQueueOverwrite() 将覆盖队列的最后一个值。
当队列满时，这些函数会覆盖里面的数据，这也意味着这些函数不会被阻塞。
```C
/* 覆盖队列
* xQueue: 写哪个队列
* pvItemToQueue: 数据地址
* 返回值: pdTRUE表示成功, pdFALSE表示失败
*/
BaseType_t xQueueOverwrite(QueueHandle_t xQueue,
const void * pvItemToQueue
);

BaseType_t xQueueOverwriteFromISR(QueueHandle_t xQueue,
const void * pvItemToQueue,
BaseType_t *pxHigherPriorityTaskWoken
);
```

如果想让队列中的数据供多方读取，也就是说读取时不要移除数据，要留给后来人。
那么可以使用"窥视"，也就是xQueuePeek() 或xQueuePeekFromISR() 。
这些函数会从队列中复制出数据，但是不移除数据。
这也意味着，如果队列中没有数据，那么"偷看"时会导致阻塞；
一旦队列中有数据，以后每次"偷看"都会成功。
```C
/* 偷看队列
* xQueue: 偷看哪个队列
* pvItemToQueue: 数据地址, 用来保存复制出来的数据
* xTicksToWait: 没有数据的话阻塞一会
* 返回值: pdTRUE表示成功, pdFALSE表示失败
*/
BaseType_t xQueuePeek(QueueHandle_t xQueue,
void * const pvBuffer,
TickType_t xTicksToWait
);

BaseType_t xQueuePeekFromISR(QueueHandle_t xQueue,
void *pvBuffer,
);
```

### 队列基本使用

本程序会创建一个队列，然后创建2个发送任务、1个接收任务：
发送任务优先级为1，分别往队列中写入100、200
接收任务优先级为2，读队列、打印数值
![](images/APi-2025-02-25-16-06-10.png)

```C
/* 队列句柄, 创建队列时会设置这个变量 */
QueueHandle_t xQueue;
int main( void )
{
    prvSetupHardware();
    /* 创建队列: 长度为5，数据大小为4字节(存放一个整数) */
    xQueue = xQueueCreate( 5, sizeof( int32_t ) );
    if( xQueue != NULL )
    {
    /* 创建2个任务用于写队列, 传入的参数分别是100、200
    * 任务函数会连续执行，向队列发送数值100、200
    * 优先级为1
    */
        xTaskCreate( vSenderTask, "Sender1", 1000, ( void * ) 100, 1, NULL );
        xTaskCreate( vSenderTask, "Sender2", 1000, ( void * ) 200, 1, NULL );
    /* 创建1个任务用于读队列
    * 优先级为2, 高于上面的两个任务
    * 这意味着队列一有数据就会被读走
    */
        xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );
    /* 启动调度器 */
        vTaskStartScheduler();
    }
    else
    {
    /* 无法创建队列 */
    }
    /* 如果程序运行到了这里就表示出错了, 一般是内存不足 */
    return 0;
}
```
发送任务
```C
static void vSenderTask( void *pvParameters )
{
    int32_t lValueToSend;
    BaseType_t xStatus;
    /* 我们会使用这个函数创建2个任务
    * 这些任务的pvParameters不一样
    */
    lValueToSend = ( int32_t ) pvParameters;
    /* 无限循环 */
    for( ;; )
    {
    /* 写队列
    * xQueue: 写哪个队列
    * &lValueToSend: 写什么数据? 传入数据的地址, 会从这个地址把数据复制进队列
    * 0: 不阻塞, 如果队列满的话, 写入失败, 立刻返回
    */
    xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );
    if( xStatus != pdPASS )
    {
    printf( "Could not send to the queue.\r\n" );
    }
    }
}
```
接收任务
```C
static void vReceiverTask( void *pvParameters )
{
    /* 读取队列时, 用这个变量来存放数据 */
    int32_t lReceivedValue;
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
    /* 无限循环 */
    for( ;; )
    {
    /* 读队列
    * xQueue: 读哪个队列
    * &lReceivedValue: 读到的数据复制到这个地址
    * xTicksToWait: 如果队列为空, 阻塞一会
    */
    xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );
    if( xStatus == pdPASS )
    {
        /* 读到了数据 */
        printf( "Received = %d\r\n", lReceivedValue );
    }
    else
    {
        /* 没读到数据 */
    printf( "Could not receive from the queue.\r\n" );
    }
    }
}
```
结果如下：
![](images/APi-2025-02-25-16-10-27.png)

### 分辨数据源

当有多个发送任务，通过同一个队列发出数据，接收任务如何分辨数据来源？
数据本身带有"来源"信息，比如写入队列的数据是一个结构体，结构体中的lDataSouceID用来表示数据来源：
```C
typedef struct {
    ID_t eDataID;
    int32_t lDataValue;
}Data_t;
```
不同的发送任务，先构造好结构体，填入自己的eDataID ，再写队列；
接收任务读出数据后，根据eDataID 就可以知道数据来源了，如下图所示：
![](images/APi-2025-02-25-16-12-45.png)

示例代码：
```C
/* 定义2种数据来源(ID) */
typedef enum
{
    eMotorSpeed,
    eSpeedSetPoint
} ID_t;

/* 定义在队列中传输的数据的格式 */
typedef struct {
    ID_t eDataID;
    int32_t lDataValue;
}Data_t;

/* 定义2个结构体 */
static const Data_t xStructsToSend[ 2 ] =
{
    { eMotorSpeed, 10 }, /* CAN任务发送的数据 */
    { eSpeedSetPoint, 5 } /* HMI任务发送的数据 */
};

/* vSenderTask被用来创建2个任务，用于写队列
* vReceiverTask被用来创建1个任务，用于读队列
*/
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/* 队列句柄, 创建队列时会设置这个变量 */
QueueHandle_t xQueue;

int main( void )
{
    prvSetupHardware();
    /* 创建队列: 长度为5，数据大小为4字节(存放一个整数) */
    xQueue = xQueueCreate( 5, sizeof( Data_t ) );
    if( xQueue != NULL )
    {
        /* 创建2个任务用于写队列, 传入的参数是不同的结构体地址
        * 任务函数会连续执行，向队列发送结构体
        * 优先级为2
        */
        xTaskCreate(vSenderTask, "CAN Task", 1000, (void *) &(xStructsToSend[0]), 2, NULL);
        xTaskCreate(vSenderTask, "HMI Task", 1000, (void *) &(xStructsToSend[1]), 2, NULL);
        /* 创建1个任务用于读队列
        * 优先级为1, 低于上面的两个任务
        * 这意味着发送任务优先写队列，队列常常是满的状态
        */
        xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 1, NULL );
        /* 启动调度器 */
        vTaskStartScheduler();
    }
    else
    {
        /* 无法创建队列 */
    }
    /* 如果程序运行到了这里就表示出错了, 一般是内存不足 */
    return 0;
}
```
发送
```C
static void vSenderTask( void *pvParameters )
{
    BaseType_t xStatus;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
    /* 无限循环 */
    for( ;; )
    {
    /* 写队列
    * xQueue: 写哪个队列
    * pvParameters: 写什么数据? 传入数据的地址, 会从这个地址把数据复制进队列
    * xTicksToWait: 如果队列满的话, 阻塞一会
    */
    xStatus = xQueueSendToBack( xQueue, pvParameters, xTicksToWait ); //结构体的地址传输
    if( xStatus != pdPASS )
    {
        printf( "Could not send to the queue.\r\n" );
    }
    }
}
```
接收任务的函数：
```C
static void vReceiverTask( void *pvParameters )
{
    /* 读取队列时, 用这个变量来存放数据 */
    Data_t xReceivedStructure;
    BaseType_t xStatus;
    /* 无限循环 */
    for( ;; )
    {
    /* 读队列
    * xQueue: 读哪个队列
    * &xReceivedStructure: 读到的数据复制到这个地址
    * 0: 没有数据就即刻返回，不阻塞
    */
        xStatus = xQueueReceive( xQueue, &xReceivedStructure, 0 );
        if( xStatus == pdPASS )
        {
        /* 读到了数据 */
            if( xReceivedStructure.eDataID == eMotorSpeed )
            {
                printf( "From CAN, MotorSpeed = %d\r\n",xReceivedStructure.lDataValue );
            }
            else if( xReceivedStructure.eDataID == eSpeedSetPoint )
            {
            printf( "From HMI, SpeedSetPoint = %d\r\n",xReceivedStructure.lDataValue );
            }
        }
        else
        {
            /* 没读到数据 */
            printf( "Could not receive from the queue.\r\n" );
        }   
    }
}
```
![](images/APi-2025-02-25-16-28-26.png)
结果分析：
![](images/APi-2025-02-25-16-29-20.png)

t1：HMI是最后创建的最高优先级任务，它先执行，一下子向队列写入5个数据，把队列都写满了
t2：队列已经满了，HMI任务再发起第6次写操作时，进入阻塞状态。这时CAN任务是最高优先级的就绪态任务，它开始执行
t3：CAN任务发现队列已经满了，进入阻塞状态；接收任务变为最高优先级的就绪态任务，它开始运行
t4：现在，HMI任务、CAN任务的优先级都比接收任务高，它们都在等待队列有空闲的空间；一旦接收任务读出1个数据，会马上被抢占。被谁抢占？谁等待最久？HMI任务！所以在t4时刻，切换到HMI任务。
t5：HMI任务向队列写入第6个数据，然后再次阻塞，这是CAN任务已经阻塞很久了。接收任务变为最高优先级的就绪态任务，开始执行。
t6：现在，HMI任务、CAN任务的优先级都比接收任务高，它们都在等待队列有空闲的空间；一旦接收任务读出1个数据，会马上被抢占。被谁抢占？谁等待最久？CAN任务！所以在t6时刻，切换到CAN任务。
t7：CAN任务向队列写入数据，因为仅仅有一个空间供写入，所以它马上再次进入阻塞状态。这时HMI任务、CAN任务都在等待空闲空间，只有接收任务可以继续执行。

### 大数据块传输

FreeRTOS的队列使用拷贝传输，也就是要传输uint32_t时，把4字节的数据拷贝进队列；要传输一个8字节的结构体时，把8字节的数据拷贝进队列。
如果要传输1000字节的结构体呢？写队列时拷贝1000字节，读队列时再拷贝1000字节？不建议这么做，影响效率！
这时候，我们要传输的是这个巨大结构体的地址：把它的地址写入队列，对方从队列得到这个地址，使用地址去访问那1000字节的数据。

使用地址来间接传输数据时，这些数据放在RAM里，对于这块RAM，要保证这几点：
1. RAM的所有者、操作者，必须清晰明了
    这块内存，就被称为"共享内存"。要确保不能同时修改RAM。比如，在写队列之前只有由发送者修改这块RAM，在读队列之后只能由接收者访问这块RAM。
2. RAM要保持可用
    这块RAM应该是全局变量，或者是动态分配的内存。对于动然分配的内存，要确保它不能提前释放：要等到接收者用完后再释放。
    另外，不能是局部变量。

代码示例：
```C
/* 定义一个字符数组 */
static char pcBuffer[100];
/* vSenderTask被用来创建2个任务，用于写队列
* vReceiverTask被用来创建1个任务，用于读队列
*/
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/* 队列句柄, 创建队列时会设置这个变量 */
QueueHandle_t xQueue;

int main( void )
{
    prvSetupHardware();
    /* 创建队列: 长度为1，数据大小为4字节(存放一个char指针) */
    xQueue = xQueueCreate( 1, sizeof(char *) );
    if( xQueue != NULL )
    {
        /* 创建1个任务用于写队列
        * 任务函数会连续执行，构造buffer数据，把buffer地址写入队列
        * 优先级为1
        */
        xTaskCreate( vSenderTask, "Sender", 1000, NULL, 1, NULL );
        /* 创建1个任务用于读队列
        * 优先级为2, 高于上面的两个任务
        * 这意味着读队列得到buffer地址后，本任务使用buffer时不会被打断
        */
        xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );
        /* 启动调度器 */
        vTaskStartScheduler();
    }
    else
    {
    /* 无法创建队列 */
    }
    /* 如果程序运行到了这里就表示出错了, 一般是内存不足 */
    return 0;
}
```
发送任务的函数中，现在全局大数组pcBuffer中构造数据，然后把它的地址写入队列，代码如下：
```C
static void vSenderTask( void *pvParameters )
{
    BaseType_t xStatus;
    static int cnt = 0;
    char *buffer;
    /* 无限循环 */
    for( ;; )
    {
        sprintf(pcBuffer, "www.100ask.net Msg %d\r\n", cnt++);
        buffer = pcBuffer; // buffer变量等于数组的地址, 下面要把这个地址写入队列
        /* 写队列
        * xQueue: 写哪个队列
        * pvParameters: 写什么数据? 传入数据的地址, 会从这个地址把数据复制进队列
        * 0: 如果队列满的话, 即刻返回
        */
        xStatus = xQueueSendToBack( xQueue, &buffer, 0 ); /* 只需要写入4字节, 无需写入整个buffer */
        if( xStatus != pdPASS )
        {
            printf( "Could not send to the queue.\r\n" );
        }
    }
}
```
接收任务的函数中，读取队列、得到buffer的地址、打印，代码如下：
```C
static void vReceiverTask( void *pvParameters )
{
    /* 读取队列时, 用这个变量来存放数据 */
    char *buffer;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );
    BaseType_t xStatus;
    /* 无限循环 */
    for( ;; )
    {
        /* 读队列
        * xQueue: 读哪个队列
        * &xReceivedStructure: 读到的数据复制到这个地址
        * xTicksToWait: 没有数据就阻塞一会
        */
        xStatus = xQueueReceive( xQueue, &buffer, xTicksToWait); /* 得到buffer地址，只是4字节 */
        if( xStatus == pdPASS )
        {
            /* 读到了数据 */
            printf("Get: %s", buffer);
        }
        else
        {
            /* 没读到数据 */
            printf( "Could not receive from the queue.\r\n" );
        }
    }
}
```
![](images/APi-2025-02-25-16-51-30.png)

### 邮箱

FreeRTOS的邮箱概念跟别的RTOS不一样，这里的邮箱称为"橱窗"也许更恰当：
- 它是一个队列，队列长度只有1
- 写邮箱：新数据覆盖旧数据，在任务中使用xQueueOverwrite() ，在中断中使用xQueueOverwriteFromISR() 。既然是覆盖，那么无论邮箱中是否有数据，这些函数总能成功写入数据。
- 读邮箱：读数据时，数据不会被移除；在任务中使用xQueuePeek() ，在中断中使用xQueuePeekFromISR() 。

这意味着，第一次调用时会因为无数据而阻塞，一旦曾经写入数据，以后读邮箱时总能成功。

### 队列集

![](images/APi-2025-02-25-17-22-18.png)
队列集合本身也是队列
![](images/APi-2025-02-25-17-23-47.png)
![](images/APi-2025-02-25-17-24-14.png)

注意点：
![](images/APi-2025-02-25-17-24-53.png)

相关函数
创建队列集合：xQueueCreateSet()
![](images/APi-2025-02-26-09-10-18.png)
简历Queue与QueueSet联系：xQueueAddToSet()
![](images/APi-2025-02-26-09-17-10.png)
读取队列集：xQueueSelectFromSet()
![](images/APi-2025-02-26-09-20-30.png)


代码示例：
![](images/APi-2025-02-25-17-26-08.png)
![](images/APi-2025-02-25-17-26-20.png)

### 队列实现同步互斥

**队列结构体：**
![](images/APi-2025-02-25-16-54-55.png)
![](images/APi-2025-02-25-17-00-17.png)

队列的本质是一个环型队列：
![](images/APi-2025-02-25-17-01-10.png)

写队列过程：
![](images/APi-2025-02-25-17-02-02.png)

读队列过程：
![](images/APi-2025-02-25-17-03-38.png)

运行中指针的实际位置：
![](images/APi-2025-02-25-17-05-24.png)

若头部写入：
![](images/APi-2025-02-25-17-05-54.png)


**队列实现同步：**
![](images/APi-2025-02-25-17-07-19.png)
在队列中读取到数据后，再进行后一步操作，实现同步操作

**队列实现互斥：**
![](images/APi-2025-02-25-17-09-17.png)

补充：主动放弃CPU方式

    vTaskDelay()阻塞
    taskYIELD() 主动发起任务调度

![](images/APi-2025-02-25-17-10-40.png)
![](images/APi-2025-02-25-17-11-41.png)
xQueueCreate内存申请在堆区，不会在函数结束后释放。

## 七、信号量的使用

前面介绍的队列(queue)可以用于传输数据：在任务之间、任务和中断之间。
有时候我们只需要传递状态，并不需要传递具体的信息，比如：
- 我的事做完了，通知一下你
- 卖包子了、卖包子了，做好了1个包子！做好了2个包子！做好了3个包子！
- 这个停车位我占了，你们只能等着

在这种情况下我们可以使用信号量(semaphore)，它更节省内存。

本章涉及如下内容：
- 怎么创建、删除信号量
- 怎么发送、获得信号量
- 什么是计数型信号量？什么是二进制信号量？

### 信号量的特性

#### 常规操作

信号量本质上也是一个队列，拥有队列结构体，但是没有存储数据的buffer

信号量这个名字很恰当：
- 信号：起通知作用
- 量：还可以用来表示资源的数量
    当"量"没有限制时，它就是"计数型信号量"(Counting Semaphores)
    当"量"只有0、1两个取值时，它就是"二进制信号量"(Binary Semaphores)
- 支持的动作："give"给出资源，计数值加1；"take"获得资源，计数值减1

计数型信号量的典型场景是：

- 计数：事件产生时"give"信号量，让计数值加1；处理事件时要先"take"信号量，就是获得信号量，让计数值减1。
- 资源管理：要想访问资源需要先"take"信号量，让计数值减1；用完资源后"give"信号量，让计数值加1

信号量的"give"、"take"双方并不需要相同，可以用于生产者-消费者场合：
- 生产者为任务A、B，消费者为任务C、D
- 一开始信号量的计数值为0，如果任务C、D想获得信号量，会有两种结果：
    阻塞：买不到东西咱就等等吧，可以定个闹钟(超时时间)
    即刻返回失败：不等
- 任务A、B可以生产资源，就是让信号量的计数值增加1，并且把等待这个资源的顾客唤醒
- 唤醒谁？谁优先级高就唤醒谁，如果大家优先级一样就唤醒等待时间最长的人

二进制信号量跟计数型的唯一差别，就是计数值的最大值被限定为1。

![](images/APi-2025-02-26-09-41-02.png)

#### 信号量与队列

![](images/APi-2025-02-26-09-42-14.png)

#### 两种信号量

信号量的计数值都有限制：限定了最大值。
如果最大值被限定为1，那么它就是二进制信号量；如果最大值不是1，它就是计数型信号量。

| 二进制信号量      | 计数型信号量           |
| ----------------- | ---------------------- |
| 被创建时初始值为0 | 被创建时初始值可以设定 |
| 其他操作是一样的  | 其他操作是一样的       |

### 信号量函数

使用信号量时，先创建、然后去添加资源、获得资源。
使用句柄来表示一个信号量。

#### 创建信号量

使用信号量之前，要先创建，得到一个句柄；
使用信号量时，要使用句柄来表明使用哪个信号量。

![](images/APi-2025-02-26-09-46-41.png)

二进制信号量函数原型：
```c
/* 创建一个二进制信号量，返回它的句柄。
* 此函数内部会分配信号量结构体
* 返回值: 返回句柄，非NULL表示成功
*/
SemaphoreHandle_t xSemaphoreCreateBinary( void );

/* 创建一个二进制信号量，返回它的句柄。
* 此函数无需动态分配内存，所以需要先有一个StaticSemaphore_t结构体，并传入它的指针
* 返回值: 返回句柄，非NULL表示成功
*/
SemaphoreHandle_t xSemaphoreCreateBinaryStatic( StaticSemaphore_t *pxSemaphoreBuffer );
```

计数型信号量函数原型：
```c
/* 创建一个计数型信号量，返回它的句柄。
* 此函数内部会分配信号量结构体
* uxMaxCount: 最大计数值
* uxInitialCount: 初始计数值
* 返回值: 返回句柄，非NULL表示成功
*/
SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t uxMaxCount, UBaseType_t uxInitialCount);

/* 创建一个计数型信号量，返回它的句柄。
* 此函数无需动态分配内存，所以需要先有一个StaticSemaphore_t结构体，并传入它的指针
* uxMaxCount: 最大计数值
* uxInitialCount: 初始计数值
* pxSemaphoreBuffer: StaticSemaphore_t结构体指针
* 返回值: 返回句柄，非NULL表示成功
*/
SemaphoreHandle_t xSemaphoreCreateCountingStatic( UBaseType_t uxMaxCount,
UBaseType_t uxInitialCount,
StaticSemaphore_t *pxSemaphoreBuffer );
```
结构体为：
![](images/APi-2025-02-26-09-51-23.png)

#### 删除信号量

对于动态创建的信号量，不再需要它们时，可以删除它们以回收内存。
vSemaphoreDelete可以用来删除二进制信号量、计数型信号量，函数原型如下：

```C 
/*
* xSemaphore: 信号量句柄，你要删除哪个信号量
*/
void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );
```

#### give/take

二进制信号量、计数型信号量的give、take操作函数是一样的。
这些函数也分为2个版本：给任务使用，给ISR使用。
列表如下：

|      | 在任务中使用   | 在ISR中使用           |
| ---- | -------------- | --------------------- |
| give | xSemaphoreGive | xSemaphoreGiveFromISR |
| take | xSemaphoreTake | xSemaphoreTakeFromISR |

xSemaphoreGive的函数原型如下：
```c
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );
```
函数的参数与返回值列表如下：

| 参数       | 说明                                                                                                                                     |
| ---------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| xSemaphore | 信号量句柄，释放哪个信号量                                                                                                               |
| 返回值     | pdTRUE表示成功,如果二进制信号量的计数值已经是1，再次调用此函数则返回失败；如果计数型信号量的计数值已经是最大值，再次调用此函数则返回失败 |

xSemaphoreGiveFromISR的原型
```C
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,
BaseType_t *pxHigherPriorityTaskWoken
);
```

| 参数                      | 说明                                                                                                                                     |
| ------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| xSemaphore                | 信号量句柄，释放哪个信号量                                                                                                               |
| pxHigherPriorityTaskWoken | 如果释放信号量导致更高优先级的任务变为了就绪态，则*pxHigherPriorityTaskWoken = pdTRUE                                                    |
| 返回值                    | pdTRUE表示成功,如果二进制信号量的计数值已经是1，再次调用此函数则返回失败；如果计数型信号量的计数值已经是最大值，再次调用此函数则返回失败 |

xSemaphoreTake的函数原型如下：
```c
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,
TickType_t xTicksToWait
);
```
xSemaphoreTake函数的参数与返回值列表如下：
![](images/APi-2025-02-26-10-05-38.png)

xSemaphoreTakeFromISR的函数原型如下：
```c
BaseType_t xSemaphoreTakeFromISR(SemaphoreHandle_t xSemaphore,
BaseType_t *pxHigherPriorityTaskWoken
);
```
参数列表：
![](images/APi-2025-02-26-10-06-28.png)

### 二进制信号量实现同步/互斥

代码示例：
主逻辑：
![](images/APi-2025-02-26-10-19-15.png)

发送，接收任务：
- A：发送任务优先级高，先执行。连续3次释放二进制信号量，只有第1次成功
- B：发送任务进入阻塞态
- C：接收任务得以执行，得到信号量，打印OK；再次去获得信号量时，进入阻塞状态
- 在发送任务的vTaskDelay退出之前，运行的是空闲任务：现在发送任务、接收任务都阻塞了
- D：发送任务再次运行，连续3次释放二进制信号量，只有第1次成功
- E：发送任务进入阻塞态
- F：接收任务被唤醒，得到信号量，打印OK；再次去获得信号量时，进入阻塞状态

![](images/APi-2025-02-26-10-21-41.png)

结果：
![](images/APi-2025-02-26-10-23-44.png)

同步：先V后P (前一个任务先释放，后一个任务再申请)
互斥：先P后V (先申请资源，再释放资源)

### 防止数据丢失

发送任务发出3次"提醒"，但是接收任务只接收到1次"提醒"，其中2次"提醒"丢失了。
这种情况很常见，比如每接收到一个串口字符，串口中断程序就给任务发一次"提醒"，假设收到多个字符、发出了多次"提醒"。
当任务来处理时，它只能得到1次"提醒"。

故而，需要一些方式来防止数据的丢失：
- 在串口中断中，把数据放入缓冲区
- 在任务中，一次性把缓冲区中的数据都读出
- 简单地说，就是：你提醒了我多次，我太忙只响应你一次，但是我一次性拿走所有数据

示例：
![](images/APi-2025-02-26-10-30-29.png)

- A：发送任务优先级高，先执行。连续写入3个数据、释放3个信号量：只有1个信号量起作用
- B：发送任务进入阻塞态
- C：接收任务得以执行，得到信号量
- D：接收任务一次性把所有数据取出
- E：接收任务再次尝试获取信号量，进入阻塞状态
- 在发送任务的vTaskDelay退出之前，运行的是空闲任务：现在发送任务、接收任务都阻塞了
- F：发送任务再次运行，连续写入3个数据、释放3个信号量：只有1个信号量起作用
- G：发送任务进入阻塞态
- H：接收任务被唤醒，得到信号量，一次性把所有数据取出

![](images/APi-2025-02-26-10-30-38.png)

### 计数型信号量使用

使用计数型信号量时，可以多次释放信号量；当信号量的技术值达到最大时，再次释放信号量就会出错。
如果信号量计数值为n，就可以连续n次获取信号量，第(n+1)次获取信号量就会阻塞或失败。

示例：
主逻辑：
![](images/APi-2025-02-26-10-34-27.png)
发送任务、接收任务的代码和执行流程如下：
- A：发送任务优先级高，先执行。连续释放4个信号量：只有前面3次成功，第4次失败
- B：发送任务进入阻塞态
- CDE：接收任务得以执行，得到3个信号量
- F：接收任务试图获得第4个信号量时进入阻塞状态
- 在发送任务的vTaskDelay退出之前，运行的是空闲任务：现在发送任务、接收任务都阻塞了
- G：发送任务再次运行，连续释放4个信号量：只有前面3次成功，第4次失败
- H：发送任务进入阻塞态
- IJK：接收任务得以执行，得到3个信号量
- L：接收任务再次获取信号量时进入阻塞状态
![](images/APi-2025-02-26-10-34-48.png)

结果：
![](images/APi-2025-02-26-10-36-43.png)

## 八、互斥量(Mutex)

使用队列、信号量，都可以实现互斥访问，以信号量为例：

- 信号量初始值为1
- 任务A想上厕所，"take"信号量成功，它进入厕所
- 任务B也想上厕所，"take"信号量不成功，等待
- 任务A用完厕所，"give"信号量；轮到任务B使用

这需要有2个前提：
- 任务B很老实，不撬门(一开始不"give"信号量)
- 没有坏人：别的任务不会"give"信号量
（自己give，自己take就很无语）

使用互斥量可以解决这个问题，互斥量的名字取得很好：
- 量：值为0、1
- 互斥：用来实现互斥访问
它的核心在于：谁上锁，就只能由谁开锁。

很奇怪的是，FreeRTOS的互斥锁，并没有在代码上实现这点：
- 即使任务A获得了互斥锁，任务B竟然也可以释放互斥锁。
- 谁上锁、谁释放：只是约定。

互斥锁和二进制信号量的对比：
- 互斥锁解决了优先级反转的问题
- 解决了递归上锁解锁的问题

### 互斥量的使用

在多任务系统中，任务A正在使用某个资源，还没用完的情况下任务B也来使用的话，就可能导致问题。
比如对于串口，任务A正使用它来打印，在打印过程中任务B也来打印，客户看到的结果就是A、B的信息混杂在一起。

这种现象很常见：
- 访问外设：刚举的串口例子
- 读、修改、写操作导致的问题
    对于同一个变量，比如int a ，如果有两个任务同时写它就有可能导致问题。
    对于变量的修改，C代码只有一条语句，比如： a=a+8; ，它的内部实现分为3步：读出原值、修改、写入。
    ![](images/APi-2025-02-26-10-47-57.png)
    我们想让任务A、B都执行add_a函数，a的最终结果是1+8+8=17 。
    假设任务A运行完代码①，在执行代码②之前被任务B抢占了：现在任务A的R0等于1。
    任务B执行完add_a函数，a等于9。
    任务A继续运行，在代码②处R0仍然是被抢占前的数值1，执行完②③的代码，a等于9，这跟预期的17不符合。
- 对变量的非原子化访问
    修改变量、设置结构体、在16位的机器上写32位的变量，这些操作都是非原子的。也就是它们的操作过程都可能被打断，如果被打断的过程有其他任务来操作这些变量，就可能导致冲突。
- 函数重入
    "可重入的函数"是指：多个任务同时调用它、任务和中断同时调用它，函数的运行也是安全的。
    可重入的函数也被称为"线程安全"(thread safe)。
    每个任务都维持自己的栈、自己的CPU寄存器，如果一个函数只使用局部变量，那么它就是线程安全的。
    函数中一旦使用了全局变量、静态变量、其他外设，它就不是"可重入的"，如果改函数正在被调用，就必须阻止其他任务、中断再次调用它。

上述问题的解决方法是：
任务A访问这些全局变量、函数代码时，独占它，就是上个锁。
这些全局变量、函数代码必须被独占地使用，它们被称为临界资源。

互斥量也被称为互斥锁，使用过程如下：
- 互斥量初始值为1
- 任务A想访问临界资源，先获得并占有互斥量，然后开始访问
- 任务B也想访问临界资源，也要先获得互斥量：被别人占有了，于是阻塞
- 任务A使用完毕，释放互斥量；任务B被唤醒、得到并占有互斥量，然后开始访问临界资源
- 任务B使用完毕，释放互斥量

正常来说：在任务A占有互斥量的过程中，任务B、任务C等等，都无法释放互斥量。
但是FreeRTOS未实现这点：任务A占有互斥量的情况下，任务B也可释放互斥量。

### 互斥量函数

#### 创建

互斥量是一种特殊的二进制信号量。
使用互斥量时，先创建、然后去获得、释放它。使用句柄来表示一个互斥量。
创建互斥量的函数有2种：动态分配内存，静态分配内存。
函数原型如下：
```C
/* 创建一个互斥量，返回它的句柄。
* 此函数内部会分配互斥量结构体
* 返回值: 返回句柄，非NULL表示成功
*/
SemaphoreHandle_t xSemaphoreCreateMutex( void );
/* 创建一个互斥量，返回它的句柄。
* 此函数无需动态分配内存，所以需要先有一个StaticSemaphore_t结构体，并传入它的指针
* 返回值: 返回句柄，非NULL表示成功
*/
SemaphoreHandle_t xSemaphoreCreateMutexStatic( StaticSemaphore_t *pxMutexBuffer);
```

要想使用互斥量，需要在配置文件FreeRTOSConfig.h中定义：
```C
#define configUSE_MUTEXES 1
```

#### 其他函数

要注意的是，在FreeRTOS中，互斥量（Mutex）不能在中断服务例程（ISR）中使用。
互斥量是设计用于任务之间的同步机制，而不是用于中断上下文。

如果需要在ISR中进行同步，可以使用FreeRTOS提供的其他机制，例如二值信号量（Binary Semaphore）或队列（Queue）。
FreeRTOS提供了专门的API函数用于在ISR中操作这些同步机制，例如xSemaphoreGiveFromISR和xQueueSendFromISR

各类操作函数，比如删除、give/take，跟一般是信号量是一样的。
```C
/*
* xSemaphore: 信号量句柄，你要删除哪个信号量, 互斥量也是一种信号量
*/
void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );

/* 释放 */
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );

/* 释放(ISR版本) */
BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,BaseType_t *pxHigherPriorityTaskWoken
);

/* 获得 */
BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore,TickType_t xTicksToWait
);

/* 获得(ISR版本) */
xSemaphoreGiveFromISR(SemaphoreHandle_t xSemaphore,BaseType_t  
*pxHigherPriorityTaskWoken
);
```

### 互斥量基本使用

使用互斥量时有如下特点：
- 刚创建的互斥量可以被成功"take"
- "take"互斥量成功的任务，被称为"holder"，只能由它"give"互斥量；别的任务"give"不成功
- 在ISR中不能使用互斥量

示例：
main逻辑
![](images/APi-2025-02-26-11-02-25.png)
发送
![](images/APi-2025-02-26-11-03-23.png)
保留互斥量和去除互斥量的差别如下：
![](images/APi-2025-02-26-11-03-56.png)

### FreeRTOS中互斥量的缺陷

互斥量、互斥锁，本来的概念确实是：谁上锁就得由谁解锁。
但是FreeRTOS并没有实现这点，只是要求程序员按照这样的惯例写代码。

### 优先级反转与优先级继承

假设任务A、B都想使用串口，A优先级比较低：
- 任务A获得了串口的互斥量
- 任务B也想使用串口，它将会阻塞、等待A释放互斥量
- 高优先级的任务，被低优先级的任务延迟，这被称为"优先级反转"(priority inversion)

图解：
![](images/APi-2025-02-26-11-11-14.png)
- A先上锁，执行临界区代码
- B进入就绪队列，优先级高于A，B先执行
- C进入就绪队列，优先级高于B，C先执行，获取锁失败，阻塞
- B开始执行执行，C优先级大于B，但是需要等待B的执行

互斥量可以通过"优先级继承"，可以很大程度解决"优先级反转"的问题，这也是FreeRTOS中互斥量和二级制信号量的差别。
优先级继承图解：

![](images/APi-2025-02-26-11-13-41.png)
- A上锁，执行临界区代码
- B进入就绪队列，优先级高于A，B先执行
- C进入就绪队列，优先级高于B，C先执行，获取锁失败，阻塞，并将自己的优先级继承给A。
- A获得了更高的优先级，先执行，解锁，退出临界区，恢复优先级
- C获得锁，运行
- C运行完毕B运行
- B运行完毕，A继续运行

优先级继承是二进制信号量所没有的
但是二进制信号量可以在ISR中断服务程序中进行。

### 递归锁

