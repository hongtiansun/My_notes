## 以下是学习Linux驱动开发时遇到的Makefile的补充知识

### 1.命令行 make V=1 目标
可以通过命令行定义变量V = 1
目标是要生成的目标，不指定生成ALL:最终目标

### 2.MAKEFLAGS变量

当使用“make -s”编译的时候，“-s”会作为 MAKEFLAGS 变量的一部分传递给 Makefile。
MAKEFLAGS 是 GNU Make 中的一个特殊变量，用于指定 make 命令的默认选项。

- 全局设置 Make 的选项
MAKEFLAGS 中的值会被自动传递给所有子 make 进程，确保它们继承相同的选项。

- 指定默认选项
如果你经常使用某些 make 参数，可以通过 MAKEFLAGS 设置它们，这样就不需要每次运行 make 时手动输入。

1. 命令行指定
`make -j4 MAKEFLAGS="--silent"`
这会设置 MAKEFLAGS 为 --silent，并运行 Make 时允许 4 个并行任务。
2. 在 Makefile 中设置
`MAKEFLAGS += -j4`
这会让 Make 使用 4 个并行任务(字make 本make不受影响)
3. 环境变量调用
`export MAKEFLAGS="-j4 -k"`
所有调用的 make 进程都会使用 -j4 -k

注意：

    1. 环境变量自动导入 Makefile
    当你在 Shell 中通过 export MAKEFLAGS=... 设置 MAKEFLAGS 时：
    make 会自动将这个环境变量导入到 Makefile 的上下文中，作为一个 Makefile 变量。

    2. 在 Makefile 中，你可以像使用普通变量一样使用 $(MAKEFLAGS)。
    Makefile 变量自动导出到子进程
    如果在 Makefile 中定义或修改了 MAKEFLAGS，make 会将其自动传递到子进程（包括调用的子 make）中。
    子进程将接收到作为环境变量的 MAKEFLAGS，从而继承所有选项和参数。


查看MAKEFLAGS:
命令行中
```sh
make -p | grep MAKEFLAGS`
```
Makefile中
```makefile
all:
    @echo $(MAKEFLAGS)
```
MAKEFLAGS优先级：
```
命令行优先：如果在命令行中显式指定选项，它会覆盖 MAKEFLAGS 的设置。

MAKEFLAGS 全局生效：如果命令行没有指定选项，MAKEFLAGS 的值会作为默认设置。
```
### 3.递归调用
`$(MAKE) -C subdir `

`$(MAKE)`就是调用“make”命令，-C 指定子目录。
有时候我们需要向子 make 传递变量，这个时候使用“export”来导出要传递给子 make 的变量即可，如果不希望哪个变量传递给子make 的话就使用“unexport”来声明不导出.
“SHELL”和“MAKEFLAGS”，这两个变量除非使用“unexport”声明，否则的话在整个make的执行过程中，它们的值始终自动的传递给子make。

### 4.添加变量

+= 来给变量追加了一些值
```makefile
# 定义初始变量值
SRC_FILES = main.c

# 追加内容
SRC_FILES += utils.c

# 再次追加
SRC_FILES += math.c

# 输出变量
all:
    @echo $(SRC_FILES)
```

### 5.条件语句

在 Makefile 中，ifeq 是一种条件语句，用于根据条件来执行不同的指令或设置不同的变量值。
它可以让 Makefile 根据环境、变量值等动态调整其行为。
```makefile
ifeq (条件1, 条件2)
    # 条件为真时执行的内容
else
    # 条件为假时执行的内容（可选）
endif
```
示例 1：根据变量值设置
```makefile
DEBUG = 1

ifeq ($(DEBUG), 1)
    CFLAGS = -g -O0
else
    CFLAGS = -O2
endif

all:
    @echo $(CFLAGS)
```
如果运行 make，输出结果为：

```diff
-g -O0
```

示例 2：检查环境变量
```makefile
ifeq ($(shell uname), Linux)
    OS = Linux
else
    OS = Other
endif

all:
    @echo Target OS: $(OS)
```
根据当前系统类型，可能输出：
```yaml
Target OS: Linux
```

示例 3：目标存在与否判断
```makefile
ifeq ($(wildcard myfile.txt),)
    FILE_EXISTS = no
else
    FILE_EXISTS = yes
endif

all:
    @echo File exists: $(FILE_EXISTS)
```
如果当前目录下没有 myfile.txt 文件，输出为：
```arduino
File exists: no
```

同理ifneq表示如果不相等执行！

### 6.补充函数

1. `$(origin <variable>)`
variable 是变量名，origin 函数的返回值就是变量来源
如果变量V是在命令行定义的那么它的来源就是"commandline"

2. `$(filter <pattern...>,<text>)`
filter 函数表示以 pattern 模式过滤 text 字符串中的单词，仅保留符合模式 pattern 的单词，可以有多个模式。
函数返回值就是符合 pattern 的字符串。

3. `$(firstword <text>)`
firstword 函数用于取出 text 字符串中的第一个单词，函数的返回值就是获取到的单词。

4. `$(shell cmd)`
可以执行cmd命令

5. `$(words <text>)`
words 函数来统计单词个数

6. `$(patsubst <pattern>,<replacement>,<text>)`
此函数用于在 text 中查找符合 pattern 的部分，如果匹配的话就用 replacement 替换掉。
pattern 是可以包含通配符“%”，如果 replacement 中也包含通配符“%”，那么 replacement 中的这个“%”将是 pattern 中的那个“%”所代表的字符串。
函数的返回值为替换后的字符串。

### 7.@符号

make 在执行的时候默认会在终端输出命令，但是在命令前面加上“@”就不会在终端输出命令了。

### 8.VPATH变量

在 Makefile 中，VPATH 是一个特殊变量，用于指定搜索目标文件和依赖文件的路径。
当 Makefile 中指定的文件（如目标文件、源文件或依赖文件）不在当前目录时，VPATH 提供了一个查找的目录列表。
`VPATH = path1:path2:path3`
path1, path2, path3：用冒号 : 分隔的目录路径列表。
Make 会按照 VPATH 中指定的顺序搜索这些目录。

### 9.include

include是Makefile中的一个指令，用于在当前Makefile中包含另一个Makefile或脚本。
通过包含scripts/Kbuild.include文件，可以将该文件中定义的大量通用函数和变量引入到当前Makefile中，这些定义和规则随后可以在当前Makefile中被频繁调用，以实现特定的功能‌.

### 10.必定更新
```makefile
FORCE:

```

可以看出 FORCE 是没有规则和依赖的，所以每次都会重新生成 FORCE。
当 FORCE 作为其他目标的依赖时，由于 FORCE 总是被更新过的，因此依赖所在的规则总是会执行的。