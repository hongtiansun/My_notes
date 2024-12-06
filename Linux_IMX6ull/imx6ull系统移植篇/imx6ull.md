# 系统移植篇

在上一篇中我们学习了如何进行 I.MX6U 的裸机开发，通过 21 个裸机例程我们掌握了I.MX6U 的常用外设。
通过裸机的学习我们掌握了外设的底层原理，这样在以后进行 Linux 驱动开发的时候就只需要将精力放到 Linux 驱动框架上，在进行 Linux 驱动开发之前肯定需要先将Linux 系统移植到开发板上去。

如果学习过 UCOS/FreeRTOS 应该知道，UCOS/FreeRTOS 移植就是在官方的 SDK 包里面找一个和自己所使用的芯片一样的工程编译一下，然后下载到开发板就可以了。
那么 Linux 的移植是不是也是这样的，下载 Linux 源码，然后找个和我们所使用的芯片一样的工程编译一下就可以了？很明显不是的！Linux 的移植要复杂的多！！！

在移植 Linux之前我们需要先移植一个 bootloader 代码，这个 bootloader 代码用于启动 Linux 内核，bootloader有很多，常用的就是 U-Boot。
移植好 U-Boot 以后再移植 Linux 内核，移植完 Linux 内核以后Linux 还不能正常启动，还需要再移植一个根文件系统(rootfs)，根文件系统里面包含了一些最常用的命令和文件。

所以 U-Boot、Linux kernel 和 rootfs 这三者一起构成了一个完整的 Linux 系统，一个可以正常使用、功能完善的 Linux 系统。
在本篇我们就来讲解 U-Boot、Linux Kernel 和rootfs 的移植，与其说是“移植”，倒不如说是“适配”，因为大部分的移植工作都由 NXP 完成了，我们这里所谓的“移植”主要是使其能够在 I.MX6U-ALPHA 开发板上跑起来。

## 第三十章 U-Boot使用实验

在移植 U-Boot 之前，我们肯定要先使用一下 U-Boot，得先体验一下 U-Boot 是个什么东西。
I.MX6U-ALPHA 开发板光盘资料里面已经提供了一个正点原子团队已经移植好的 U-Boot，本章我们就直接编译这个移植好的 U-Boot，然后烧写到 SD 卡里面启动，启动 U-Boot 以后就可以学习使用 U-Boot 的命令。

### 30.1 U-Boot简介

Linux 系统要启动就必须需要一个 bootloader 程序，也就说芯片上电以后先运行一段bootloader程序。
这段bootloader程序会先初始化DDR等外设，然后将Linux内核从flash(NAND，NOR FLASH，SD，MMC 等)拷贝到 DDR 中，最后启动 Linux 内核。
当然了，bootloader 的实际工作要复杂的多，但是它最主要的工作就是启动 Linux 内核，bootloader 和 Linux 内核的关系就跟 PC 上的 BIOS 和 Windows 的关系一样，bootloader 就相当于 BIOS。
所以我们要先搞定bootloader，很庆幸，有很多现成的 bootloader 软件可以使用，比如 U-Boot、vivi、RedBoot 等等，其中以 U-Boot 使用最为广泛，为了方便书写，本书会将 U-Boot 写为 uboot。

uboot 的全称是 Universal Boot Loader，uboot 是一个遵循 GPL 协议的开源软件，uboot 是一个裸机代码，可以看作是一个裸机综合例程。
现在的 uboot 已经支持液晶屏、网络、USB 等高级功能。

[uboot官网](http://www.denx.de/wiki/U-Boot/)

![alt](./images/Snipaste_2024-12-05_19-55-53.png)

我们可以在 uboot 官网下载 uboot 源码

但是我们一般不会直接用 uboot 官方的 U-Boot 源码的。
uboot 官方的 uboot 源码是给半导体厂商准备的，半导体厂商会下载 uboot 官方的 uboot 源码，然后将自家相应的芯片移植进去。
也就是说半导体厂商会自己维护一个版本的 uboot，这个版本的 uboot 相当于是他们定制的。
既然是定制的，那么肯定对自家的芯片支持会很全，虽然 uboot 官网的源码中一般也会支持他们的芯片，但是绝对是没有半导体厂商自己维护的 uboot 全面。

NXP就维护的2016.03这个版本的uboot
[下载地址](http://git.freescale.com/git/cgit.cgi/imx/uboot-imx.git/tag/?h=imx_v2016.03_4.1.15_2.0.0_ga&id=rel_imx_4.1.15_2.1.0_ga)
（下载地址已经失效）

我们可以使用开发板自带的资料中的NXP官方uboot
路径为：开发板光盘->1、程序源码->4、NXP 官方原版 Uboot和Linux->uboot-imx-rel_imx_4.1.15_2.1.0_ga.tar.bz2。

NVP官方维护的uboot基本支持了 NXP 当前所有可以跑 Linux 的芯片，而且支持各种启动方式，比如 EMMC、NAND、NOR FLASH 等等，这些都是 uboot 官方所不支持的。但是此uboot 是针对NXP 自家评估板的，如果是我们自己做的板子就需要修改 NXP 官方的 uboot，使其支持我们自己做的板子，正点原子的 I.MX6U 开发板就是自己做的板子，虽然大部分都参考了 NXP 官方的I.MX6ULLEVK 开发板，但是还是有很多不同的地方，所以需要修改 NXP 官方的 uboot，使其适配正点原子的 I.MX6U 开发板。

所以当我们拿到开发板以后，是有三种 uboot 的，这三种 uboot的区别如表：
![alt](./images/Snipaste_2024-12-05_20-09-02.png)

那么这三种 uboot 该如何选择呢？首先 uboot 官方的基本是不会用的，因为支持太弱了。
最常用的就是半导体厂商或者开发板厂商的 uboot，如果你用的半导体厂商的评估板，那么就使用半导体厂商的 uboot，如果你是购买的第三方开发板，比如正点原子的 I.MX6ULL 开发板，那么就使用正点原子提供的 uboot 源码（也是在半导体厂商的 uboot 上修改的）。
当然了，你也可以在购买了第三方开发板以后使用半导体厂商提供的 uboot，只不过有些外设驱动可能不支持，需要自己移植，这个就是我们常说的 uboot 移植。

本节是 uboot 的使用，所以就直接使用正点原子已经移植好的 uboot，这个已经放到了开发板光盘中了。
路径为：开发板光盘->1、程序源码->3、正点原子 Uboot 和 Linux 出厂源码->ubootimx-2016.03-2.1.0-ge468cdc-v1.5.tar.bz2。

### 30.2 U-boot初次编译

首先在 Ubuntu 中安装 ncurses 库，否则编译会报错，安装命令如下：
`sudo apt-get install libncurses5-dev`

安装如下：
![alt](./images/Snipaste_2024-12-05_20-14-13.png)

在 Ubuntu 中创建存放 uboot 的目录，比如我的是/home/sun/linux/uboot，然后在此目录下新建一个名为“alientek_uboot”的文件夹用于存放正点原子提供的 uboot 源码。
alientek_uboot文件夹创建成功以后使用 FileZilla 软件将正点原子提供的 uboot 源码拷贝到此目录中。

![alt](./images/Snipaste_2024-12-05_20-18-01.png)

使用如下命令进行解压缩：
`tar -vxjf uboot-imx-2016.03-2.1.0-g8b546e4.tar.bz2`

![alt](./images/Snipaste_2024-12-05_20-18-58.png)

除了 uboot-imx-2016.03-2.1.0-g8b546e4.tar.bz2 这个正点原子提供的 uboot 源码压缩包以外，其他的文件和文件夹都是解压出来的 uboot 源码。

1. **512MB(DDR3)+8GB(EMMC)核心板**

如果使用的是 512MB+8GB 的 EMMC 核心板，使用如下命令来编译对应的 uboot：

```sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- (加空格)
                                    mx6ull_14x14_ddr512_emmc_defconfig
make V=1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j12
```

![alt](./images/Snipaste_2024-12-05_20-22-17.png)

这三条命令中 ARCH=arm 设置目标为 arm 架构，CROSS_COMPILE 指定所使用的交叉编译器。
第一条命令相当于“make distclean”，目的是清除工程，一般在第一次编译的时候最好清理一下工程。
第二条指令相当于“make mx6ull_14x14_ddr512_emmc_defconfig”，用于配置 uboot，配置文件为 mx6ull_14x14_ddr512_emmc_defconfig。
最后一条指令相当于 “make -j12”也就是使用 12 核来编译 uboot

编译完成以后的 alentek_uboot 文件夹内容如图所示：
![alt](./images/Snipaste_2024-12-05_20-26-04.png)

可以看出，编译完成以后 uboot 源码多了一些文件，其中 u-boot.bin 就是编译出来的 uboot二进制文件。uboot是个裸机程序，因此需要在其前面加上头部(IVT、DCD等数据)才能在I.MX6U上执行。
图中的 u-boot.imx 文件就是添加头部以后的 u-boot.bin，u-boot.imx 就是我们最终要烧写到开发板中的 uboot 镜像文件。

每次编译 uboot 都要输入一长串命令，为了简单起见，我们可以新建一个 shell 脚本文件，将这些命令写到 shell 脚本文件里面，然后每次只需要执行 shell 脚本即可完成编译工作。
新建名为 mx6ull_alientek_emmc.sh 的 shell 脚本文件，然后在里面输入如下内容：

```sh
1 #!/bin/bash
2 make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
3 make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- (加空格)
                        mx6ull_14x14_ddr512_emmc_defconfi
4 make V=1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j12
```

第 1 行是 shell 脚本要求的，必须是“#!/bin/bash”或者“#!/bin/sh”。

第 2 行使用了 make 命令，用于清理工程，也就是每次在编译 uboot 之前都清理一下工程。
这里的 make 命令带有三个参数，第一个是 ARCH，也就是指定架构，这里肯定是 arm；第二个参数 CROSS_COMPILE 用于指定编译器，只需要指明编译器前缀就行了，比如 arm-linuxgnueabihf-gcc 编译器的前缀就是“arm-linux-gnueabihf-”；
最后一个参数 distclean 就是清除工程。

第 3 行也使用了 make 命令，用于配置 uboot。
同样有三个参数，不同的是，最后一个参数是mx6ull_14x14_ddr512_emmc_defconfig。
前面说了 uboot 是 bootloader 的一种，可以用来引导Linux，但是 uboot 除了引导 Linux 以外还可以引导其它的系统，而且 uboot 还支持其它的架构和外设，比如 USB、网络、SD 卡等。
这些都是可以配置的，需要什么功能就使能什么功能。
所以在编译 uboot 之前，一定要根据自己的需求配置。
uboot.mx6ull_14x14_ddr512_emmc_defconfig就是正点原子针对 I.MX6U-ALPHA 的 EMMC 核心板编写的配置文件，这个配置文件在 uboot源码的 configs 目录中。
在 uboot 中，通过“make xxx_defconfig”来配置 uboot，xxx_defconfig就是不同板子的配置文件，这些配置文件都在 uboot/configs 目录中。

第 4 行有 4 个参数，用于编译 uboot，通过第 3 行配置好 uboot 以后就可以直接“make”编译 uboot 了。
其中 V=1 用于设置编译过程的信息输出级别；-j 用于设置主机使用多少线程编译uboot，最好设置成我们虚拟机所设置的核心数，如果在 VMware 里面给虚拟就分配了 4 个核，那么使用-j4 是最合适的，这样 4 个核都会一起编译。

使用 chmod 命令给予 mx6ull_alientek_emmc.sh 文件可执行权限，然后就可以使用这个 shell脚本文件来重新编译 uboot。

2. **256MB(DDR3)+ 512MB(NAND)核心板**

```sh
1 #!/bin/bash
2 make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean  
3 make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- (加空格)  
                            mx6ull_14x14_ddr256_nand_defconfig  
4 make V=1 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j12  
```

### 30.3 U-boot烧写与启动

uboot 编译好以后就可以烧写到板子上使用了，这里我们跟前面裸机例程一样，将 uboot烧写到 SD 卡中，然后通过 SD 卡来启动来运行 uboot。
使用 imxdownload 软件来进行烧写。

等待烧写完成，完成以后将 SD 卡插到 I.MX6U-ALPHA 开发板上，BOOT 设置从 SD 卡启动，使用 USB 线将 USB_TTL 和电脑连接，也就是将开发板的串口 1 连接到电脑上。
打开SecureCRT，设置好串口参数并打开，最后复位开发板。
在串口shell上出现“Hit any key to stop autoboot: ”倒计时的时候按下键盘上的回车键，默认是 3 秒倒计时，在 3 秒倒计时结束以后如果没有按下回车键的话 uboot 就会使用默认参数来启动 Linux 内核了。
如果在 3 秒倒计时结束之前按下回车键，那么就会进入 uboot 的命令行模式，如图所示：

![alt](./images/Snipaste_2024-12-05_20-45-43.png)

从图可以看出，当进入到 uboot 的命令行模式以后，左侧会出现一个“=>”标志。
uboot 启动的时候会输出一些信息，这些信息如下所示：

![alt](./images/Snipaste_2024-12-05_20-46-36.png)

第 1 行是 uboot 版本号和编译时间，可以看出，当前的 uboot 版本号是 2016.03，编译时间是 2020 年 8 月 7 日凌晨 20 点 47 分。

第 3 和第 4 行是 CPU 信息，可以看出当前使用的 CPU 是飞思卡尔的 I.MX6ULL（I.MX 以前属于飞思卡尔，然而飞思卡尔被 NXP 收购了），频率为 792MHz，但是此时运行在 396MHz。这颗芯片是工业级的，结温为-40°C~105°C。

第 5 行是复位原因，当前的复位原因是 POR。I.MX6ULL 芯片上有个 POR_B 引脚，将这个引脚拉低即可复位 I.MX6ULL。

第 6 行是板子名字，当前的板子名字为“I.MX6U ALPHA|MINI”。

第 7 行提示 I2C 准备就绪。

第 8 行提示当前板子的 DRAM（内存）为 512MB，如果是 NAND 版本的话内存为 256MB。

第 9 行提示当前有两个 MMC/SD 卡控制器：FSL_SDHC(0)和 FSL_SDHC(1)。
I.MX6ULL支持两个 MMC/SD，正点原子的 I.MX6ULL EMMC 核心板上 FSL_SDHC(0)接的 SD(TF)卡，FSL_SDHC(1)接的 EMMC。

第 10 和第 11 行是 LCD 型号，当前的 LCD 型号是 ATK-LCD-7-1024x600 (1024x600)，分辨率为 1024x600，格式为 RGB888(24 位)。

第 12~14 是标准输入、标准输出和标准错误所使用的终端，这里都使用串口(serial)作为终端。

第 15 和 16 行是切换到 emmc 的第 0 个分区上，因为当前的 uboot 是 emmc 版本的，也就是从 emmc 启动的。我们只是为了方便将其烧写到了 SD 卡上，但是它的“内心”还是 EMMC的。所以 uboot 启动以后会将 emmc 作为默认存储器，当然了，你也可以将 SD 卡作为 uboot 的存储器，这个我们后面会讲解怎么做。

第 17 行是网口信息，提示我们当前使用的 FEC1 这个网口，I.MX6ULL 支持两个网口。

第 18 行提示 FEC1 网卡地址没有设置，后面我们会讲解如何在 uboot 里面设置卡地址。

第 20 行提示正常启动，也就是说 uboot 要从 emmc 里面读取环境变量和参数信息启动 Linux内核了。

第 21 行是倒计时提示，默认倒计时 3 秒，倒计时结束之前按下回车键就会进入 Linux 命令行模式。
如果在倒计时结束以后没有按下回车键，那么 Linux 内核就会启动，Linux 内核一旦启动，uboot 就会寿终正寝。

这个就是 uboot 默认输出信息的含义，NAND 版本的 uboot 也是类似的，只是 NAND 版本的就没有 EMMC/SD 相关信息了，取而代之的就是 NAND 的信息，比如 NAND 容量大小信息。

uboot 是来干活的，我们现在已经进入 uboot 的命令行模式了，进入命令行模式以后就可以给 uboot 发号施令了。当然了，不能随便发号施令，得看看 uboot 支持哪些命令，然后使用这些uboot 所支持的命令来做一些工作。下一节就讲解 uboot 命令的使用。

### 30.4 U-Boot命令使用

进入 uboot 的命令行模式以后输入“help”或者“？”，然后按下回车即可查看当前 uboot 所支持的命令，如图：
![alt](./images/Snipaste_2024-12-05_20-54-49.png)

图中的命令并不是 uboot 所支持的所有命令，前面说过 uboot 是可配置的，需要什么命令就使能什么命令。
所以图中的命令是正点原子提供的 uboot 中使能的命令，uboot 支持的命令还有很多，而且也可以在 uboot 中自定义命令。
这些命令后面都跟有命令说明，用于描述此命令的作用，但是命令具体怎么用呢？
我们输入“help(或?) 命令名”既可以查看命令的详细用法，以“bootz”这个命令为例，我们输入如下命令即可查看“bootz”这个命令的用法：

```sh
? bootz
help bootz
```

![alt](/images/Snipaste_2024-12-05_20-57-06.png)

图中就详细的列出了“bootz”这个命令的详细，其它的命令也可以使用此方法查询
具体的使用方法。
接下来我们学习一下一些常用的 uboot 命令。

#### 30.4.1 信息查询命令

常用的和信息查询有关的命令有 3 个：bdinfo、printenv 和 version。

先来看一下 bdinfo 命令，此命令用于查看板子信息，直接输入“bdinfo”即可：

![alt](/images/Snipaste_2024-12-05_21-05-51.png)

从图中可以得出 DRAM 的起始地址和大小、启动参数保存起始地址、波特率、
sp(堆栈指针)起始地址等信息。

命令“printenv”用于输出环境变量信息，uboot 也支持 TAB 键自动补全功能，输入“print”然后按下 TAB 键就会自动补全命令，直接输入“print”也可以。
输入“print”，然后按下回车键，环境变量如图:

![alt](./images/Snipaste_2024-12-05_21-07-59.png)

这里有很多的环境变量，比如 baudrate、board_name、board_rec、boot_fdt、bootcmd等等。
uboot 中的环境变量都是字符串，既然叫做环境变量，那么它的作用就和“变量”一样。
比如 bootdelay 这个环境变量就表示 uboot 启动延时时间，默认 bootdelay=3，也就默认延时 3秒。前面说的 3 秒倒计时就是由 bootdelay 定义的，如果将 bootdelay 改为 5 的话就会倒计时 5s了。
uboot 中的环境变量是可以修改的，有专门的命令来修改环境变量的值，稍后我们会讲解。

命令 version 用于查看 uboot 的版本号，输入“version”，uboot 版本号如图：
![alt](./images/Snipaste_2024-12-05_21-09-30.png)

从图可以看出，当前 uboot 版本号,编译时间，编译器为arm-poky-linux-gnueabi-gcc，这是 NXP 官方提供的编译器，正点原子出厂系统用的此编译器编译的，但是本教程我们统一使用 arm-linux-gnueabihf-gcc。

#### 30.4.2 环境变量操作命令

1. **修改环境变量**

环境变量的操作涉及到两个命令：setenv 和 saveenv，命令 setenv 用于设置或者修改环境变量的值。
命令 saveenv 用于保存修改后的环境变量，一般环境变量是存放在外部 flash 中的，uboot 启动的时候会将环境变量从 flash 读取到 DRAM 中。
所以使用命令 setenv 修改的是 DRAM中的环境变量值，修改以后要使用 saveenv 命令将修改后的环境变量保存到 flash 中，否则的话uboot 下一次重启会继续使用以前的环境变量值。

比如我们要将环境变量 bootdelay 改为 5，就可以使用如下所示命令：

```sh
setenv bootdelay 5
saveenv
```

![alt](./images/Snipaste_2024-12-05_21-13-42.png)

当我们使用命令 saveenv 保存修改后的环境变量的话会有保存过程提示信息，根据提示可以看出环境变量保存到了 MMC(0)中，也就是 SD 卡中。
因为我们现在将 uboot烧写到了 SD 卡里面，所以会保存到 MMC(0)中。
如果烧写到 EMMC 里面就会提示保存到MMC(1)，也就是 EMMC 设备，同理，如果是 NAND 版本核心板的话就会提示保存到 NAND中。

修改 bootdelay 以后，重启开发板，uboot 就是变为 5 秒倒计时.
![alt](./images/Snipaste_2024-12-05_21-18-45.png)

有时候我们修改的环境变量值可能会有空格，比如 bootcmd、bootargs 等，这个时候环境变量值就得用单引号括起来，比如下面修改环境变量 bootargs 的值：

```sh
setenv bootargs 'console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw'
saveenv
```

上面命令设置 bootargs 的值为“console=ttymxc0,115200 root=/dev/mmcblk1p2 rootwait rw”。
其中“console=ttymxc0,115200”、“root=/dev/mmcblk1p2”、“rootwait”和“rw”相当于四组“值”，这四组“值”之间用空格隔开，所以需要使用单引号‘’将其括起来，表示这四组“值”都属于环境变量 bootargs

2. **新建环境变量**

命令 setenv 也可以用于新建命令，用法和修改环境变量一样，比如我们新建一个环境变量author，author 的值为我的名字拼音：zuozhongkai，那么就可以使用如下命令：

```sh
setenv author zuozhongkai
saveenv
```

新建命令 author 完成以后重启 uboot，然后使用命令 printenv 查看当前环境变量:

![alt](./images/Snipaste_2024-12-05_21-19-25.png)

3. **删除环境变量**

既然可以新建环境变量，肯定也可以删除环境变量，删除环境变量也是使用命令 setenv，要删除一个环境变量只要给这个环境变量赋空值即可，比如我们删除掉上面新建的 author 这个环境变量，命令如下：

```sh
setenv author 
saveenv
```

通过 setenv 给 author 赋空值，也就是什么都不写来删除环境变量 author。
重启uboot 就会发现环境变量 author 没有了。

#### 30.4.3 内存操作指令

内存操作命令就是用于直接对 DRAM 进行读写操作的，常用的内存操作命令有 md、nm、mm、mw、cp 和 cmp。
我们依次来看一下这些命令都是做什么的。

1. **md指令**

md 命令用于显示内存值，格式如下：
`md[.b, .w, .l] address [# of objects]`
命令中的[.b .w .l]对应 byte、word 和 long，也就是分别以 1 个字节、2 个字节、4 个字节来显示内存值。
address 就是要查看的内存起始地址，[# of objects]表示要查看的数据长度，这个数据长度单位不是字节，而是跟你所选择的显示格式有关。
比如你设置要查看的内存长度为20(十六进制为 0x14)，如果显示格式为.b 的话那就表示 20 个字节；如果显示格式为.w 的话就表示 20 个 word，也就是 20\*2=40 个字节；如果显示格式为.l 的话就表示 20 个 long，也就是20\*4=80 个字节。
另外要注意：
uboot 命令中的数字都是十六进制的！不是十进制的！

比如你想查看以 0X80000000 开始的 20 个字节的内存值，显示格式为.b 的话，应该使用
如下所示命令：
`md.b 80000000 14`
而不是
`md.b 80000000 20`

uboot 命令里面的数字都是十六进制的，所以可以不用写“0x”前缀，十进制的 20 其十六进制为 0x14，所以命令 md 后面的个数应该是 14，如果写成 20 的话就表示查看32(十六进制为 0x20)个字节的数据。
分析下面三个命令的区别：

```sh
md.b 80000000 10
md.w 80000000 10
md.l 80000000 10
```

上面这三个命令都是查看以 0X80000000 为起始地址的内存数据，第一个命令以.b 格式显
示，长度为 0x10，也就是 16 个字节；第二个命令以.w 格式显示，长度为 0x10，也就是 16\*2=32个字节；最后一个命令以.l 格式显示，长度也是 0x10，也就是 16\*4=64 个字节。
这三个命令的执行结果如图所示：
![alt](./images/Snipaste_2024-12-05_21-28-12.png)

2. **nm命令**

nm 命令用于修改指定地址的内存值，命令格式如下：
`nm [.b, .w, .l] address`

nm 命令同样可以以.b、.w 和.l 来指定操作格式，比如现在以.l 格式修改 0x80000000 地址的数据为 0x12345678。
输入命令：
`nm.l 80000000`

![alt](./images/Snipaste_2024-12-05_21-29-57.png)

示例图：
![alt](./images/Snipaste_2024-12-05_21-30-34.png)

80000000 表示现在要修改的内存地址，0500e031 表示地址 0x80000000 现在的数据，？后面就可以输入要修改后的数据 0x12345678，输入完成以后按下回车，然后再输q即可推出。

修改完成以后在使用命令 md 来查看一下有没有修改成功
![alt](./images/Snipaste_2024-12-05_21-31-41.png)

3. **mm命令**

![alt](./images/Snipaste_2024-12-05_21-34-59.png)
80000000地址下，对应8\*4\*4 = 128 bit = 16B
对应于80000010 跨越了16个地址，可见按字节编制！

mm 命令也是修改指定地址内存值的，使用 mm 修改内存值的时候地址会自增，而使用命令 nm 的话地址不会自增。
比如以.l 格式修改从地址 0x80000000 开始的连续 3 个内存块(3\*4=12个字节)的数据为 0X05050505，操作如图:

![alt](./images/Snipaste_2024-12-05_21-37-27.png)

4. **mw命令**

命令 mw 用于使用一个指定的数据填充一段内存，命令格式如下：

`mw [.b, .w, .l] address value [count]`

mw 命令同样可以以.b、.w 和.l 来指定操作格式，address 表示要填充的内存起始地址，value为要填充的数据，count 是填充的长度。
比如使用.l 格式将以 0X80000000 为起始地址的 0x10 个内存块(0x10 * 4=64 字节)填充为 0X0A0A0A0A，命令如下：
`mw.l 80000000 0A0A0A0A 10`

查看如图：
![alt](./images/Snipaste_2024-12-05_21-41-40.png)

5. **cp命令**

cp 是数据拷贝命令，用于将 DRAM 中的数据从一段内存拷贝到另一段内存中，或者把 Nor Flash 中的数据拷贝到 DRAM 中。
命令格式如下：
`cp [.b, .w, .l] source target count`

cp 命令同样可以以.b、.w 和.l 来指定操作格式，source 为源地址，target 为目的地址，count为拷贝的长度。
我们使用.l 格式将 0x80000000 处的地址拷贝到 0X80000100 处，长度为 0x10 个内存块(0x10 * 4=64 个字节)，命令如下所示：
`cp.l 80000000 80000100 10`

![alt](./images/Snipaste_2024-12-05_21-43-52.png)

先使用 md.l 命令打印出地址 0x80000000 和 0x80000100 处的数据，然后使用命令cp.l将0x80000100处的数据拷贝到0x80000100处。最后使用命令md.l查看0x80000100处的数据有没有变化，检查拷贝是否成功.

6. **cmp命令**

cmp 是比较命令，用于比较两段内存的数据是否相等，命令格式如下：
`cmp [.b, .w, .l] addr1 addr2 count`

cmp 命令同样可以以.b、.w 和.l 来指定操作格式，addr1 为第一段内存首地址，addr2 为第二段内存首地址，count 为要比较的长度。
我们使用.l 格式来比较 0x80000000 和 0X80000100 这两个地址数据是否相等，比较长度为 0x10 个内存块(16 * 4=64 个字节)，命令如下所示：
`cmp.l 80000000 80000100 10`

![alt](./images/Snipaste_2024-12-05_21-45-44.png)

可以看出两段内存的数据相等。我们再随便挑两段内存比较一下，比如地址0x80002000 和 0x800003000，长度为 0X10:
![alt](./images/Snipaste_2024-12-05_21-46-44.png)

可以看出，0x80002000 处的数据和 0x80003000 处的数据就不一样。

#### 30.4.4 网络操作命令(*)

需要链接网线，暂时略过

#### 30.4.5 EMMC与SD卡命令(*)

uboot 支持 EMMC 和 SD 卡，因此也要提供 EMMC 和 SD 卡的操作命令。
一般认为 EMMC和 SD 卡是同一个东西，所以没有特殊说明，本教程统一使用 MMC 来代指 EMMC 和 SD 卡。
uboot 中常用于操作 MMC 设备的命令为“mmc”。

mmc 是一系列的命令，其后可以跟不同的参数，输入“？mmc”即可查看 mmc 有关的命令，如图：

![alt](./images/Snipaste_2024-12-06_16-50-38.png)

从图可以看出，mmc 后面跟不同的参数可以实现不同的功能，如表所示：

| 命令            | 描述                                          |
| --------------- | --------------------------------------------- |
| mmc info        | 输出 MMC 设备信息                             |
| mmc read        | 读取 MMC 中的数据。                           |
| mmc wirte       | 向 MMC 设备写入数据。                         |
| mmc rescan      | 扫描 MMC 设备。                               |
| mmc part        | 列出 MMC 设备的分区。                         |
| mmc dev         | 切换 MMC 设备。                               |
| mmc list        | 列出当前有效的所有 MMC 设备。                 |
| mmc hwpartition | 设置 MMC 设备的分区。                         |
| mmc bootbus……   | 设置指定 MMC 设备的 BOOT_BUS_WIDTH 域的值。   |
| mmc bootpart……  | 设置指定 MMC 设备的 boot 和 RPMB 分区的大小。 |
| mmc partconf……  | 设置指定 MMC 设备的 PARTITION_CONFG 域的值。  |
| mmc rst         | 复位 MMC 设备                                 |
| mmc setdsr      | 设置 DSR 寄存器的值。                         |

1. **mmc info命令**

mmc info 命令用于输出当前选中的 mmc info 设备的信息，输入命令“mmc info”即可，如图所示：

![alt](./images/Snipaste_2024-12-06_16-58-12.png)

参考图：
![alt](./images/Snipaste_2024-12-06_17-00-04.png)

从图可以看出，当前选中的 MMC设备是 EMMC，版本为 5.0，容量为 7.1GiB(EMMC
为 8GB)，速度为 52000000Hz=52MHz，8 位宽的总线。
还有一个与 mmc info 命令相同功能的命令：mmcinfo，“mmc”和“info”之间没有空格。
实际量产的 EMMC 核心板所使用的 EMMC芯片是多厂商供应的，因此 EMMC 信息以实际为准，但是容量都为 8GB 的！

mmc info 只能展示当前的mmc设备
自行设置，当前设备为SD卡，故而为SD卡信息。

2. **mmc rescan 命令**

mmc rescan 命令用于扫描当前开发板上所有的 MMC 设备。
包括 EMMC 和 SD 卡，输入“mmc rescan”即可

3. **mmc list**

mmc list 命令用于来查看当前开发板一共有几个 MMC 设备，输入“mmc list”，结果如图所示：

![alt](./images/Snipaste_2024-12-06_17-04-18.png)

可以看出当前开发板有两个 MMC 设备：FSL_SDHC:0 (SD)和 FSL_SDHC:1，这是因为我现在用的是 EMMC 版本的核心板，加上 SD 卡一共有两个 MMC 设备，FSL_SDHC:0 是 SD卡，FSL_SDHC:1(eMMC)是 EMMC。
输入“mmc info”查询到的是当前设备信息。要想查看其他设备信息，就要使用命令“mmc dev”来将 EMMC设置为当前的 MMC 设备。

4. **mmc dev 命令**

mmc dev 命令用于切换当前 MMC 设备，命令格式如下：
`mmc dev [dev] [part]`

[dev]用来设置要切换的 MMC 设备号，[part]是分区号。
如果不写分区号的话默认为分区 0。
如图：
![alt](./images/Snipaste_2024-12-06_17-08-47.png)
通过这种方式可以轻松的实现mmc设备的切换

5. **mmc part命令**

有时候 SD 卡或者 EMMC 会有多个分区，可以使用命令“mmc part”来查看其分区，比如查看 EMMC 的分区情况，输入如下命令：

```sh
mmc dev 1 //切换到 EMMC
mmc part //查看 EMMC 分区
```

![alt](./images/Snipaste_2024-12-06_17-13-40.png)

可以看出，此时 EMMC 有两个分区，第一个分区起始扇区为 20480，长度为 262144 个扇区；第二个分区起始扇区为 282624，长度为 14594048 个扇区。
如果 EMMC 里面烧写了 Linux 系统的话，EMMC 是有 3 个分区的，第 0 个分区存放 uboot，第 1 个分区存放Linux 镜像文件和设备树，第 2 个分区存放根文件系统。
但是在图中只有两个分区，那是因为第 0 个分区没有格式化，所以识别不出来，实际上第 0 个分区是存在的。
一个新的 SD卡默认只有一个分区，那就是分区 0，所以前面讲解的 uboot 烧写到 SD 卡，其实就是将 u-boot.bin烧写到了 SD 卡的分区 0 里面。

后面学习 Linux 内核移植的时候再讲解怎么在 SD 卡中创建并格式化第二个分区，并将 Linux 镜像文件和设备树文件存放到第二个分区中。

如果要将 EMMC 的分区 2 设置为当前 MMC 设备，可以使用如下命令：
`mmc dev 1 2`
如图：
![alt](./images/Snipaste_2024-12-06_17-17-23.png)

mmcinfo #0 显示容量为7.3GB
![alt](./images/Snipaste_2024-12-06_17-25-05.png)

65536\*15202304 \* 512 B /1024/1024/1024 ≈ 7.28GB

6. **mmc read命令**

mmc read 命令用于读取 mmc 设备的数据，命令格式如下：
`mmc read addr blk# cnt`

addr 是数据读取到 DRAM 中的地址，blk 是要读取的块起始地址(十六进制)，一个块是 512字节，这里的块和扇区是一个意思，在 MMC 设备中我们通常说扇区，cnt 是要读取的块数量(十六进制)。
比如从 EMMC 的第 1536(0x600)个块开始，读取 16(0x10)个块的数据到 DRAM 的0X80800000 地址处，命令如下：

```sh
mmc dev 1 0 //切换到 MMC 分区 0
mmc read 80800000 600 10 //读取数据
```

这里我们还看不出来读取是否正确，通过 md.b 命令查看 0x80800000 处的数据就行了，查看 16\*512=8192(0x2000)个字节的数据，命令如下：
`md.b 80800000 2000`

![alt](./images/Snipaste_2024-12-06_17-30-15.png)

可以看到“baudrate=115200.board_name=EVK.board_rev=14X14.”等字样，这个就是 uboot 中的环境变量。
EMMC 核心板 uboot 环境变量的存储起始地址就是1536*512=786432。

7. **mmc write命令**

要将数据写到 MMC 设备里面，可以使用命令“mmc write”，格式如下：
`mmc write addr blk# cnt`

addr 是要写入 MMC 中的数据在 DRAM 中的起始地址，blk 是要写入 MMC 的块起始地址(十六进制)，cnt 是要写入的块大小，一个块为 512 字节。
我们可以使用命令“mmc write”来升级 uboot，也就是在 uboot 中更新 uboot。

这里要用到 nfs 或者 tftp 命令，通过 nfs 或者 tftp 命令将新的 u-boot.bin 下载到开发板的 DRAM 中，然后再使用命令“mmc write”将其写入到 MMC设备中。
我们就来更新一下 SD 中的 uboot，先查看一下 SD 卡中的 uboot 版本号，注意编译时间，输入命令：

```sh
mmc dev 0 //切换SD卡
version //1版本号
```

![alt](./images/Snipaste_2024-12-06_17-46-26.png)

可以看出当前 SD 卡中的 uboot 的编译时间。

我们现在重新编译一下 uboot，然后将编译出来的 u-boot.imx(u-boot.bin 前面加了一些头文件)拷贝到 Ubuntu 中的tftpboot 目录下。
最后使用 tftp 命令将其下载到 0x80800000 地址处，命令如下：
`tftp 80800000 u-boot.imx`

如图：
![alt]("略")

u-boot.imx 大小为 379904 字节，379904/512=742，所以我们要向 SD 卡中写入742 个块，如果有小数的话就要加 1 个块。

使用命令“mmc write”从 SD 卡分区 0 第 2 个块(扇区)开始烧写，一共烧写 742(0x2E6)个块，命令如下：

```sh
mmc dev 0 0
mmc write 80800000 2 32E
```

烧写过程如图：
![alt]("略")

烧写成功，重启开发板(从 SD 卡启动)，重启以后再输入 version 来查看版本号，结果如图：
![alt](略)

可以看出，此时的 uboot 的编译时间，说明 uboot更新成功。
这里我们就学会了如何在 uboot 中更新 uboot 了，如果要更新 EMMC 中的 uboot 也是一样的。
同理，如果要在 uboot 中更新 EMMC 对应的 uboot，可以使用如下所示命令：

```sh
mmc dev 1 0 //切换到 EMMC 分区 0
tftp 80800000 u-boot.imx //下载 u-boot.imx 到 DRAM
mmc write 80800000 2 32E //烧写 u-boot.imx 到 EMMC 中
mmc partconf 1 1 0 0 //分区配置，EMMC 需要这一步！
```

<span style="color:red"><b>千万不要写 SD 卡或者 EMMC 的前两个块(扇区)，里面保存着分区表！</b></span>

由于没有网线，本小节验证代码后续补充！

8. **mmc erase命令**  

如果要擦除 MMC 设备的指定块就是用命令“mmc erase”，命令格式如下：
`mmc erase blk# cnt`

blk 为要擦除的起始块，cnt 是要擦除的数量。没事不要用 mmc erase 来擦除 MMC 设备！！！

关于 MMC 设备相关的命令就讲解到这里，表中还有一些跟 MMC 设备操作有关的命令，但是很少用到，这里就不讲解了，感兴趣的可以上网查一下，或者在 uboot 中查看这些命令的使用方法。

#### 30.4.6 FAT格式文件系统操作命令(*)

有时候需要在 uboot 中对 SD 卡或者 EMMC 中存储的文件进行操作，这时候就要用到文件操作命令，跟文件操作相关的命令有：fatinfo、fatls、fstype、fatload 和 fatwrite，但是这些文件操作命令只支持 FAT 格式的文件系统！！

1. **fatinfo命令**

fatinfo 命令用于查询指定 MMC 设备分区的文件系统信息，格式如下：

`fatinfo <interface> [<dev[:part]>]`

interface 表示接口，比如 mmc，dev 是查询的设备号，part 是要查询的分区。
比如我们要查询 EMMC 分区 1 的文件系统信息，命令如下：
`fatinfo mmc 1:1`

![alt](./images/Snipaste_2024-12-06_19-02-23.png)

从上图可以看出，EMMC 分区 1 的文件系统为 FAT32 格式的。

2. **fatls命令**

fatls 命令用于查询 FAT 格式设备的目录和文件信息，命令格式如下：
`fatls <interface> [<dev[:part]>] [directory]`

interface 是要查询的接口，比如 mmc，dev 是要查询的设备号，part 是要查询的分区，directory是要查询的目录。
比如查询 EMMC 分区 1 中的所有的目录和文件，输入命令：
`fatls mmc 1:1`

![alt](./images/Snipaste_2024-12-06_19-05-39.png)

从上图可以看出，emmc 的分区 1 中存放着 8 个文件。

3. **fstype命令**

fstype 用于查看 MMC 设备某个分区的文件系统格式，命令格式如下：
`fstype <interface> <dev>:<part>`

正点原子 EMMC 核心板上的 EMMC 默认有 3 个分区，我们来查看一下这三个分区的文件系统格式，输入命令：

```sh
fstype mmc 1:0
fstype mmc 1:1
fstype mmc 1:2
```

![alt](./images/Snipaste_2024-12-06_19-08-21.png)

从上图可以看出，分区 0 格式未知，因为分区 0 存放的 uboot，并且分区 0 没有格式化，所以文件系统格式未知。
分区 1 的格式为 fat，分区 1 用于存放 linux 镜像和设备树。
分区 2 的格式为 ext4，用于存放 Linux 的根文件系统(rootfs)。

4. **fatload命令**

fatload 命令用于将指定的文件读取到 DRAM 中，命令格式如下：
`fatload <interface> [<dev[:part]> [<addr> [<filename> [bytes [pos]]]]]`

interface 为接口，比如 mmc，dev 是设备号，part 是分区，addr 是保存在 DRAM 中的起始地址，filename 是要读取的文件名字。bytes 表示读取多少字节的数据，如果 bytes 为 0 或者省略的话表示读取整个文件。pos 是要读的文件相对于文件首地址的偏移，如果为 0 或者省略的话表示从文件首地址开始读取。
我们将 EMMC 分区 1 中的 zImage 文件读取到 DRAM 中的0X80800000 地址处，命令如下：
`fatload mmc 1:1 80800000 zImage`

从SD卡读取
![alt](./images/Snipaste_2024-12-06_19-12-40.png)
从EMMC读取
![alt](./images/Snipaste_2024-12-06_19-14-01.png)

5. **fatwrite命令**

注意！uboot 默认没有使能 fatwrite 命令，需要修改板子配置头文件，比如 mx6ullevk.h、mx6ull_alientek_emmc.h 等等，板子不同，其配置头文件也不同。
找到自己开发板对应的配置头文件然后添加如下一行宏定义来使能 fatwrite 命令：
`#define CONFIG_FAT_WRITE /* 使能 fatwrite 命令 */`

fatwirte 命令用于将 DRAM 中的数据写入到 MMC 设备中，命令格式如下：
`fatwrite <interface> <dev[:part]> <addr> <filename> <bytes>`

interface 为接口，比如 mmc，dev 是设备号，part 是分区，addr 是要写入的数据在 DRAM中的起始地址，filename 是写入的数据文件名字，bytes 表示要写入多少字节的数据。
我们可以通过 fatwrite 命令在 uboot 中更新 linux 镜像文件和设备树。

我们以更新 linux 镜像文件 zImage为例，首先将正点原子 I.MX6U-ALPHA 开发板提供的 zImage 镜像文件拷贝到 Ubuntu 中的tftpboot 目录下，zImage 镜像文件放到了开发板光盘中。
路径为：开发板光盘->8、系统镜像->1、出厂系统镜像->2、kernel 镜像->linux-imx-4.1.15-2.1.0-gbfed875-v1.6->zImage。

拷贝完成以后使用命令 tftp 将 zImage 下载到 DRAM 的 0X80800000 地址处，命令如下：
`tftp 80800000 zImage`

下载过程如图：
![alt](./images/Snipaste_2024-12-06_19-17-27.png)

zImage 大小为 6785272(0X6788f8)个字节(注意，由于开发板系统在不断的更新中，因此zImage 大小不是固定的，一切以实际大小为准)，接下来使用命令 fatwrite 将其写入到 EMMC 的分区 1 中，文件名字为 zImage，命令如下：
`fatwrite mmc 1:1 80800000 zImage 6788f8`

结果如图：
![alt](./images/Snipaste_2024-12-06_19-18-26.png)

完成以后使用“fatls”命令查看一下 EMMC 分区 1 里面的文件，结果如图:
![alt](./images/Snipaste_2024-12-06_19-18-53.png)

#### 30.4.7 EXT格式文件系统操作命令

uboot 有 ext2 和 ext4 这两种格式的文件系统的操作命令，常用的就四个命令，分别为：ext2load、ext2ls、ext4load、ext4ls 和 ext4write。
这些命令的含义和使用与 fatload、fatls 和 fatwrite一样，只是 ext2 和 ext4 都是针对 ext 文件系统的。

比如 ext4ls 命令，EMMC 的分区 2 就是 ext4格式的，使用 ext4ls 就可以查询 EMMC 的分区 2 中的文件和目录，输入命令：
`ext4ls mmc 1:2`

![alt](./images/Snipaste_2024-12-06_19-21-12.png)

关于 ext 格式文件系统其他命令的操作参考 30.4.6 小节的即可，这里就不讲解了。

#### 30.4.8 NAND操作命令(本人使用EMMC故而概述)

uboot 是支持 NAND Flash 的，所以也有 NAND Flash 的操作命令，前提是使用的 NAND 版本的核心板，并且编译 NAND 核心板对应的 uboot，然后使用 imxdownload 软件将 u-boot.bin 烧写到 SD 卡中，最后通过 SD 卡启动。
一般情况下 NAND 版本的核心板已经烧写好了 uboot、linux kernel 和 rootfs 这些文件，所以可以将 BOOT 拨到 NAND，然后直接从 NAND Flash 启动即可。

启动信息如图：
![alt](./images/Snipaste_2024-12-06_19-22-46.png)

从图可以看出，当前开发板的 NAND 容量为 512MiB。输入“? nand”即可查看NAND 相关命令，如图所示：

![alt](./images/Snipaste_2024-12-06_19-23-29.png)

1. **nand info**

![alt](./images/Snipaste_2024-12-06_19-24-43.png)
打印NAND FLASH信息
给出了 NAND 的页大小、OOB 域大小，擦除大小等信息。可以对照着所使用的 NAND Flash 数据手册来查看一下这些信息是否正确。

2. **nand device 命令**

nand device 用于切换 NAND Flash，如果你的板子支持多片 NAND 的话就可以使用此命令来设置当前所使用的 NAND。
这个需要你的 CPU 有两个 NAND 控制器，并且两个 NAND 控制器各接一片 NAND Flash。
就跟 I.MX6U 有两个 SDIO 接口，这两个 SDIO 接口可以接两个 MMC设备一样。不过一般情况下 CPU 只有一个 NAND 接口，而且在使用中只接一片 NAND。

3. **nand erase命令**

nand erase 命令用于擦除 NAND Flash，NAND Flash 的特性决定了在向 NAND Flash 写数据之前一定要先对要写入的区域进行擦除。“nand erase”命令有三种形式：

```sh
nand erase[.spread] [clean] off size //从指定地址开始(off)开始，擦除指定大小(size)的区域。
nand erase.part [clean] partition //擦除指定的分区
nand erase.chip [clean] //全篇擦除
```

NAND 的擦除命令一般是配合写命令的，后面讲解 NAND 写命令的时候在演示如何使用“nand erase”。

4. **nand write**

此命令用于向 NAND 指定地址写入指定的数据，一般和“nand erase”命令配置使用来更新NAND 中的 uboot、linux kernel 或设备树等文件，命令格式如下：

`nand write addr off size`

addr 是要写入的数据首地址，off 是 NAND 中的目的地址，size 是要写入的数据大小。
由于 I.MX6ULL 要求 NAND 对应的 uboot 可执行文件还需要另外包含 BCB 和 DBBT，因此直接编译出来的 uboot.imx 不能直接烧写到 NAND 里面。
关于 BCB 和 DBBT 的详细介绍请参考《I.MX6ULL 参考手册》的 8.5.2.2 小节，笔者目前没有详细去研究 BCB 和 DBBT，因此我们不能在 NAND 版的 uboot 里面更新 uboot 自身。
除非大家去研究一下 I.MX6ULL 的 BCB 和DBBT，然后在 u-boot.imx 前面加上相应的信息，否则即使将 uboot 烧进去了也不能运行。
我们使用 mfgtool 烧写系统到 NAND 里面的时候，mfgtool 会使用一个叫做“kogs-ng”的工具完成BCB 和 DBBT 的添加。

但是我们可以在 uboot 里面使用“nand write”命令烧写 kernel 和 dtb。
先编译出来 NAND版本的 kernel 和 dtb 文件，在烧写之前要先对 NAND 进行分区，也就是规划好 uboot、linux kernel、设备树和根文件系统的存储区域，I.MX6U-ALPHA 开发板出厂系统 NAND 分区如下：

```sh
0x000000000000-0x0000003FFFFF : "boot"
0x000000400000-0x00000041FFFF : "env"
0x000000420000-0x00000051FFFF : "logo"
0x000000520000-0x00000061FFFF : "dtb"
0x000000620000-0x000000E1FFFF : "kernel"
0x000000E20000-0x000020000000 : "rootfs"
```

一共有六个分区，第一个分区存放 uboot，地址范围为 0x0~0x3FFFFF(共 4MB)；
第二个分区存放 env（环境变量），地址范围为 0x400000~0x420000(共 128KB)；
第三个分区存放 logo(启动图标)，地址范围为 0x420000~0x51FFFF(共 1MB)；
第四个分区存放 dtb(设备树)，地址范围为0x520000~0x61FFFF(共 1MB)；
第五个分区存放 kernel(也就是 linux kernel)，地址范围为0x620000~0xE1FFFF(共 8MB)；
剩下的所有存储空间全部作为最后一个分区，存放 rootfs(根文件系统)。

可以看出 kernel 是从地址 0x620000 开始存放的，将 NAND 版本 kernel 对应的 zImage 文件放到 Ubuntu 中的 tftpboot 目录中，然后使用 tftp 命令将其下载到开发板的 0X87800000 地址处，最终使用“nand write”将其烧写到 NAND 中，命令如下：

```sh
tftp 0x87800000 zImage //下载 zImage 到 DRAM 中
nand erase 0x620000 0x800000 //从地址 0x620000 开始擦除 8MB 的空间
nand write 0x87800000 0x620000 0x800000 //将接收到的 zImage 写到 NAND 中
```

这里我们擦除了 8MB 的空间，因为一般 zImage 就是 6,7MB 左右，8MB 肯定够了，如果不够的话就再多擦除一点就行了。

同理，最后烧写设备树(dtb)文件文件，命令如下：

```sh
tftp 0x87800000 imx6ull-14x14-emmc-7-1024x600-c.dtb //下载 dtb 到 DRAM 中
nand erase 0x520000 0x100000 //从地址 0x520000 开始擦除 1MB 的空间
nand write 0x87800000 0x520000 0x100000 //将接收到的 dtb 写到 NAND 中
```

dtb 文件一般只有几十 KB，所以擦除 1M 是绰绰有余的了。
注意！正点原子出厂系统在NAND 里面烧写了很多种设备树文件！
这里只是举例烧写一种的方法，我们在实际产品开发中肯定只有一种设备树。

根文件系统(rootfs)就不要在 uboot 中更新了，还是使用 NXP 提供的 Mfgtool 工具来烧写，因为根文件系统太大！很有可能超过开发板 DRAM 的大小，这样连下载都没法下载，更别说更新了。

5. **nand read**

此命令用于从 NAND 中的指定地址读取指定大小的数据到 DRAM 中，命令格式如下：
`nand read addr off size`
addr 是目的地址，off 是要读取的 NAND 中的数据源地址，size 是要读取的数据大小。
比如我们读取设备树(dtb)文件到 0x83000000 地址处，命令如下：

```sh
nand read 0x83000000 0x520000 0x19000
```

结果如图：
![alt](./images/Snipaste_2024-12-06_19-33-17.png)

设备树文件读取到 DRAM 中以后就可以使用 fdt 命令来对设备树进行操作了，首先设置 fdt的地址，fdt 地址就是 DRAM 中设备树的首地址，命令如下：

`fdt addr 83000000`

设置好以后可以使用“fdt header”来查看设备树的头信息，输入命令：

`fdt header`

![alt](./images/Snipaste_2024-12-06_19-34-13.png)

输入命令“fdt print”就可以查看设备树文件的内容，输入命令：
`fdt print`

结果如图：
![alt](./images/Snipaste_2024-12-06_19-34-50.png)

图中的文件就是我们写到 NAND 中的设备树文件，至于设备树文件的详细内容我们后面会有专门的章节来讲解，这里大家知道这个文件就行了。
NAND 常用的操作命令就是擦除、读和写，至于其他的命令大家可以自行研究一下，一定不要尝试全片擦除 NAND 的指令！！否则 NAND 就被全部擦除掉了，什么都没有了，又得重头烧整个系统。

#### 30.4.9 BOOT操作命令(*)

uboot 的本质工作是引导 Linux，所以 uboot 肯定有相关的 boot(引导)命令来启动 Linux。  
常用的跟 boot 有关的命令有：bootz、bootm 和 boot。  

1. **bootz命令**  

要启动 Linux，需要先将 Linux 镜像文件拷贝到 DRAM 中，如果使用到设备树的话也需要将设备树拷贝到 DRAM 中。  
可以从 EMMC 或者 NAND 等存储设备中将 Linux 镜像和设备树文件拷贝到 DRAM，也可以通过 nfs 或者 tftp 将 Linux 镜像文件和设备树文件下载到 DRAM 中。  
不管用那种方法，只要能将 Linux 镜像和设备树文件存到 DRAM 中就行，然后使用 bootz 命令来启动，bootz 命令用于启动 zImage 镜像文件，bootz 命令格式如下：  
`bootz [addr [initrd[:size]] [fdt]]`  
命令 bootz 有三个参数，addr 是 Linux 镜像文件在 DRAM 中的位置，initrd 是 initrd 文件在DRAM 中的地址，如果不使用 initrd 的话使用‘-’代替即可，fdt 就是设备树文件在 DRAM 中的地址。  
现在我们使用网络和 EMMC 两种方法来启动 Linux 系统，首先将 I.MX6U-ALPHA 开发板的 Linux 镜像和设备树发送到 Ubuntu 主机中的 tftpboot 文件夹下。

Linux 镜像文件前面已经放到了 tftpboot 文件夹中，现在把设备树文件放到 tftpboot 文件夹里面。
由于不同的屏幕其设备树不同，因此我们出厂系统提供了很多设备树。  
路径为：开发板光盘->8、系统镜像->1、出厂系统镜像->2、kernel 镜像->linux-imx-4.1.15-2.1.0-gbfed875-v1.6.
所有设备树文件如图:
![alt](./images/Snipaste_2024-12-06_19-50-15.png)

可以看出，我们提供了 14 种设备树，笔者正在使用的是 EMMC 核心板，7 寸1024×600 分辨率的屏幕，所以需要使用 imx6ull-14x14-emmc-7-1024x600-c.dtb 这个设备树。
将imx6ull-14x14-emmc-7-1024x600-c.dtb 发送到 Ubuntu 主机中的 tftpboot 文件夹里面，完成以后的 tftpboot 文件夹如图:

![alt](./images/Snipaste_2024-12-06_19-51-55.png)

给予 imx6ull-14x14-emmc-7-1024x600-c.dtb 可执行权限，命令如下：

`chmod 777 imx6ull-14x14-emmc-7-1024x600-c.dtb`

Linux 镜像文件和设备树都准备好了，我们先学习如何通过网络启动 Linux，使用 tftp 命令将zImage下载到DRAM的0X80800000地址处，然后将设备树imx6ull-14x14-emmc-7-1024x600-c.dtb 下载到 DRAM 中的 0X83000000 地址处，最后之后命令 bootz 启动，命令如下：
```sh
tftp 80800000 zImage
tftp 83000000 imx6ull-14x14-emmc-7-1024x600-c.dtb
bootz 80800000 - 83000000
```

运行结果如图：
![alt](./images/Snipaste_2024-12-06_19-55-03.png)

上图就是我们通过 tftp 和 bootz 命令来从网络启动 Linux 系统，如果我们要从 EMMC 中启动 Linux 系统的话只需要使用命令 fatload 将 zImage 和 imx6ull-14x14-emmc-7-1024x600-c.dtb 从EMMC 的分区 1 中拷贝到 DRAM 中，然后使用命令 bootz 启动即可。

先使用命令 fatls 查看要下 EMMC 的分区 1 中有没有 Linux 镜像文件和设备树文件，如果没有的话参考 30.4.6 小节中讲解的 fatwrite 命令将 tftpboot 中的 zImage 和 imx6ull-14x14-emmc-7-1024x600-c.dtb 文件烧写到 EMMC 的分区 1 中。
然后使用命令 fatload 将 zImage 和 imx6ull-14x14-emmc-7-1024x600-c.dtb文件拷贝到 DRAM 中，地址分别为 0X80800000 和 0X83000000，最后使用 bootz 启动，命令如下：

```sh
fatload mmc 1:1 80800000 zImage
fatload mmc 1:1 83000000 imx6ull-14x14-emmc-7-1024x600-c.dtb
bootz 80800000 - 83000000
```
查看文件是否存在
![alt](./images/Snipaste_2024-12-06_20-00-44.png)
load入内存
![alt](./images/Snipaste_2024-12-06_20-01-11.png)
![alt](./images/Snipaste_2024-12-06_20-02-07.png)
bootz启动：
![alt](./images/Snipaste_2024-12-06_20-03-29.png)

参考结果：
![alt](./images/Snipaste_2024-12-06_20-04-01.png)

2. **bootm 命令**

bootm 和 bootz 功能类似，但是 bootm 用于启动 uImage 镜像文件。
如果不使用设备树的话启动 Linux 内核的命令如下：

`bootm addr`

addr 是 uImage 镜像在 DRAM 中的首地址。

如果要使用设备树，那么 bootm 命令和 bootz 一样，命令格式如下：

`bootm [addr [initrd[:size]] [fdt]]`

其中 addr 是 uImage 在 DRAM 中的首地址，initrd 是 initrd 的地址，fdt 是设备树(.dtb)文件在 DRAM 中的首地址，如果 initrd 为空的话，同样是用“-”来替代。

补充uImage 与 zImage:

    uImage 和 zImage 都是 Linux 内核映像文件格式，主要用于嵌入式系统和一些其他平台，它们的区别在于它们如何进行压缩和打包。
    下面是这两者的区别：
    1. uImage
        定义：uImage 是一个经过压缩并加上头部信息的内核映像文件格式。
        通常，它是通过 mkimage 工具从 zImage 文件生成的。
        结构：uImage 包含一个头部，这个头部描述了映像的类型、大小、加载地址、入口地址等信息。
        头部还包括校验和等元数据，用于引导加载程序（如 U-Boot）正确加载内核。
        压缩格式：uImage 通常使用 gzip、bzip2 或 lzma 等压缩格式来压缩内核映像。
        用途：uImage 格式的内核通常用于 U-Boot 引导加载器，U-Boot 会解析 uImage 文件头并将其加载到内存中，然后启动内核。

    2. zImage
        定义：zImage 是一个压缩过的内核映像文件，它是 Linux 内核的压缩版本，可以直接由引导加载程序（如 U-Boot 或其他）加载。
        结构：zImage 是直接压缩的内核映像文件，通常是通过 gzip 压缩生成的，但它没有额外的文件头信息（没有描述文件元数据的头部）。
        其大小相对较小。
        压缩格式：zImage 通常采用 gzip 压缩方式。
        用途：zImage 可以被直接加载到内存并启动，适用于很多嵌入式系统或一些简化的引导加载程序。
    
    总结
    uImage 是带有头部信息并且通常适合与 U-Boot 等引导加载器一起使用的内核映像格式。
    zImage 是直接压缩的内核映像格式，通常适用于更简单的引导加载程序，且不包含额外的头部信息。
    简而言之，uImage 可以看作是 zImage 的一种增强版，具有更多元数据来支持引导加载，而 zImage 则是更基础的、仅包含压缩内核的格式。

3. **boot命令**

boot 命令也是用来启动 Linux 系统的，只是 boot 会读取环境变量 bootcmd 来启动 Linux 系统，bootcmd 是一个很重要的环境变量！
其名字分为“boot”和“cmd”，也就是“引导”和“命令”，说明这个环境变量保存着引导命令，其实就是启动的命令集合，具体的引导命令内容是可以修改的。
![alt](./images/Snipaste_2024-12-06_20-41-59.png)

比如我们要想使用 tftp 命令从网络启动 Linux 那么就可以设置 bootcmd 为“tftp
80800000 zImage; tftp 83000000 imx6ull-14x14-emmc-7-1024x600-c.dtb; bootz 80800000 - 83000000”，然后使用 saveenv 将 bootcmd 保存起来。

然后直接输入 boot 命令即可从网络启动Linux 系统，命令如下：
```sh
setenv bootcmd 'tftp 80800000 zImage; tftp 83000000 imx6ull-14x14-emmc-7-1024x600-c.dtb; bootz 80800000 - 83000000'
saveenv
boot
```
运行结果如图:
![alt](./images/Snipaste_2024-12-06_20-44-14.png)

前面说过 uboot 倒计时结束以后就会启动 Linux 系统，其实就是执行的 bootcmd 中的启动命令。
只要不修改 bootcmd 中的内容，以后每次开机 uboot 倒计时结束以后都会使用 tftp 命令从网络下载 zImage 和 imx6ull-14x14-emmc-7-1024x600-c.dtb，然后启动 Linux。
如果想从 EMMC 启动那就设置 bootcmd 为“fatload mmc 1:1 80800000 zImage; fatload mmc 1:1 83000000 imx6ull-14x14-emmc-7-1024x600-c.dtb; bootz 80800000 - 83000000”，然后使用 boot命令启动即可，命令如下：
```sh
setenv bootcmd 'fatload mmc 1:1 80800000 zImage; fatload mmc 1:1 83000000 imx6ull-14x14-emmc-7-1024x600-c.dtb; bootz 80800000 - 83000000'
savenev
boot
```
运行结果如图:
![alt](./images/Snipaste_2024-12-06_20-46-18.png)

如果不修改 bootcmd 的话，每次开机 uboot 倒计时结束以后都会自动从 EMMC 里面读取zImage 和 imx6ull-14x14-emmc-7-1024x600-c.dtb，然后启动 Linux。

在启动 Linux 内核的时候可能会遇到如下错误：
`“Kernel panic – not Syncing: VFS: Unable to mount root fs on unknown-block(0,0)”`

这个错误的原因是 linux 内核没有找到根文件系统，这个很正常，因为没有设置 uboot 的bootargs 环境变量，关于 bootargs 环境变量后面会讲解！
此处我们重点是验证 boot 命令，linux内核已经成功启动了，说明 boot 命令工作正常。

#### 30.4.10 其他常用命令

uboot 中还有其他一些常用的命令，比如 reset、go、run 和 mtest 等。

1. **reset命令**

reset 命令顾名思义就是复位的，输入“reset”即可复位重启，如图所示：
![alt](./images/Snipaste_2024-12-06_20-48-27.png)

2. **go命令**

go 命令用于跳到指定的地址处执行应用，命令格式如下：
`go addr [arg ...]`

addr 是应用在 DRAM 中的首地址，我们可以编译一下裸机例程的实验 13_printf，然后将编译出来的 printf.bin 拷贝到 Ubuntu 中的 tftpboot 文件夹里面，注意，这里要拷贝 printf.bin 文件，不需要在前面添加 IVT 信息，因为 uboot 已经初始化好了 DDR 了。
使用 tftp 命令将 printf.bin下载到开发板 DRAM 的 0X87800000 地址处，因为裸机例程的链接首地址就是 0X87800000，最后使用 go 命令启动 printf.bin 这个应用，命令如下：
```sh
tftp 87800000 printf.bin
go 87800000
```
结果如下图：
![alt](./images/Snipaste_2024-12-06_21-54-40.png)

通过 go 命令我们就可以在 uboot 中运行裸机例程。

3. **run命令**

run 命令用于运行环境变量中定义的命令，比如可以通过“run bootcmd”来运行 bootcmd 中的启动命令，但是 run 命令最大的作用在于运行我们自定义的环境变量。
在后面调试 Linux 系统的时候常常要在网络启动和 EMMC/NAND 启动之间来回切换，而 bootcmd 只能保存一种启动方式，如果要换另外一种启动方式的话就得重写 bootcmd，会很麻烦。
这里我们就可以通过自定义环境变量来实现不同的启动方式，比如定义环境变量 mybootemmc 表示从 emmc 启动，定义 mybootnet 表示从网络启动，定义 mybootnand 表示从 NAND 启动。
如果要切换启动方式的话只需要运行“run mybootxxx(xxx 为 emmc、net 或 nand)”即可。

说干就干，创建环境变量 mybootemmc、mybootnet 和 mybootnand，命令如下：
```sh
setenv mybootemmc 'fatload mmc 1:1 80800000 zImage; fatload mmc 1:1 83000000 imx6ull14x14-emmc-7-1024x600-c.dtb;bootz 80800000 - 83000000'

setenv mybootnand 'nand read 80800000 4000000 800000;nand read 83000000 6000000 100000;bootz 80800000 - 83000000'

setenv mybootnet 'tftp 80800000 zImage; tftp 83000000imx6ull-14x14-emmc-7-1024x600-c.dtb;bootz 80800000 - 83000000'

saveenv
```
创建环境变量成功以后就可以使用 run 命令来运行 mybootemmc、mybootnet 或 mybootnand来实现不同的启动：
```sh
run mybootemmc

run mytoobnand

run mybootnet
```

4. **mtest命令**

mtest 命令是一个简单的内存读写测试命令，可以用来测试自己开发板上的 DDR，命令格式如下：
`mtest [start [end [pattern [iterations]]]]`

start是要测试的DRAM 开始地址，end 是结束地址，比如我们测试 0X80000000~0X80001000这段内存
输入 `mtest 80000000 80001000`，结果如图:
![alt](./images/Snipaste_2024-12-06_22-00-51.png)

如果要结束测试就按下键盘上的“Ctrl+C”键。

至此，uboot 常用的命令就讲解完了，如果要使用 uboot 的其他命令，可以查看 uboot 中的帮助信息，或者上网查询一下相应的资料。

## 第三十一章 U-Boot 顶层Makefile详解

上一章我们详细的讲解了 uboot 的使用方法，其实就是各种命令的使用，学会 uboot 使用以后就可以尝试移植 uboot 到自己的开发板上了，但是在移植之前需要我们得先分析一遍 uboot的启动流程源码，得捋一下 uboot 的启动流程，否则移植的时候都不知道该修改那些文件。
本章我们就来分析一下正点原子提供的 uboot 源码，重点是分析 uboot 启动流程，而不是整个 uboot源码，uboot 整个源码非常大，我们只看跟我们关心的部分即可。

