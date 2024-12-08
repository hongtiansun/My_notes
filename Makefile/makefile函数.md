Makefile中的函数提供了一种灵活的方式来处理文本和逻辑操作。它们类似于编程语言中的函数，可以接收参数并返回结果。

---

### **函数分类**

1. **文本处理函数**  
   用于字符串替换、分割、过滤等操作。
   
2. **条件函数**  
   用于逻辑判断。
   
3. **循环函数**  
   用于列表迭代和生成。
   
4. **文件操作函数**  
   用于文件路径和时间戳处理。

5. **用户自定义函数**  
   使用 `$(call)` 创建和调用。

---

### **常用内置函数**

#### **1. 文本处理函数**

| 函数                      | 描述                                               | 示例                                                    | 结果           |
|---------------------------|--------------------------------------------------|-------------------------------------------------------|----------------|
| `$(subst from,to,text)`   | 将 `text` 中的 `from` 替换为 `to`                    | `$(subst a,b,a_cat)`                                  | `b_cat`        |
| `$(patsubst pattern,replacement,text)` | 类似 `subst`，支持通配符匹配                        | `$(patsubst %.c,%.o,a.c b.c)`                        | `a.o b.o`      |
| `$(strip text)`           | 删除 `text` 中的所有前后空格和多余空白                 | `$(strip  a   b )`                                   | `a b`          |
| `$(findstring find,text)` | 检查 `find` 是否是 `text` 的子串，返回子串或空字符串      | `$(findstring bar,foobar)`                           | `bar`          |
| `$(filter pattern...,text)` | 返回 `text` 中匹配 `pattern` 的部分                  | `$(filter %.c %.h,a.c b.h c.txt)`                    | `a.c b.h`      |
| `$(filter-out pattern...,text)` | 返回 `text` 中不匹配 `pattern` 的部分               | `$(filter-out %.o,a.o b.c)`                          | `b.c`          |
| `$(sort list)`            | 对列表去重并按字典序排序                              | `$(sort c b a b c)`                                  | `a b c`        |

---

#### **2. 条件函数**

| 函数                   | 描述                                               | 示例                                | 结果                  |
|------------------------|--------------------------------------------------|-------------------------------------|-----------------------|
| `$(if condition,then[,else])` | 如果 `condition` 非空，则返回 `then`，否则返回 `else` | `$(if a,yes,no)`                   | `yes`                |
|                        |                                                  | `$(if ,yes,no)`                    | `no`                 |

---

#### **3. 循环函数**

| 函数                               | 描述                                                     | 示例                                                      | 结果                  |
|------------------------------------|--------------------------------------------------------|---------------------------------------------------------|-----------------------|
| `$(foreach var,list,text)`         | 对 `list` 中的每个元素执行 `text`，将结果拼接成字符串         | `$(foreach f,a b,c_$(f))`                               | `c_a c_b`            |
| `$(origin variable)`               | 返回 `variable` 的定义来源（如 `file`、`command` 等）     | `$(origin CC)`                                          | `default`            |

---

#### **4. 文件操作函数**

| 函数                     | 描述                                               | 示例                              | 结果                  |
|--------------------------|--------------------------------------------------|-----------------------------------|-----------------------|
| `$(dir names...)`        | 返回路径部分，保留末尾的 `/`                         | `$(dir src/foo.c src/bar.o)`     | `src/ src/`          |
| `$(notdir names...)`     | 返回文件名部分（去掉路径）                            | `$(notdir src/foo.c)`            | `foo.c`              |
| `$(suffix names...)`     | 返回文件后缀（包括 `.`）                             | `$(suffix src/foo.c)`            | `.c`                 |
| `$(basename names...)`   | 返回文件名的主体（去掉路径和后缀）                     | `$(basename src/foo.c)`          | `src/foo`            |
| `$(wildcard pattern)`    | 返回匹配 `pattern` 的文件列表                          | `$(wildcard *.c)`                | `main.c utils.c`     |

---

### **自定义函数**

#### **1. 定义和调用**

```makefile
# 定义一个函数
reverse = $(2) $(1)

# 调用函数
result = $(call reverse,first,second)

all:
    @echo $(result)
```

**输出：**
```
second first
```

#### **2. 使用 `define` 定义多行函数**

```makefile
define capitalize
$(strip $(foreach word,$(1),$(shell echo $(word) | tr a-z A-Z)))
endef

all:
    @echo $(call capitalize,hello world)
```

**输出：**
```
HELLO WORLD
```

---

### **组合示例**

#### 自动生成 `.o` 文件列表
```makefile
SRC = main.c utils.c
OBJ = $(patsubst %.c,%.o,$(SRC))

all:
    @echo $(OBJ)
```

**输出：**
```
main.o utils.o
```

#### 条件编译
```makefile
DEBUG = 1

CFLAGS = -Wall
CFLAGS += $(if $(DEBUG),-g,-O2)

all:
    @echo $(CFLAGS)
```

**输出（DEBUG=1 时）：**
```
-Wall -g
```

**输出（DEBUG=0 时）：**
```
-Wall -O2
```

#### 动态生成规则
```makefile
SRC = main.c utils.c
OBJ = $(patsubst %.c,%.o,$(SRC))

define rule_template
$(1): $(2)
    gcc -o $(1) $(2)
endef

$(foreach obj,$(OBJ),$(eval $(call rule_template,$(obj),$(basename $(obj)).c)))
```

---

### **总结**
Makefile中的函数提供了强大的文本处理和逻辑能力，通过合理使用这些函数，可以极大地简化构建流程，提升Makefile的灵活性和可维护性。