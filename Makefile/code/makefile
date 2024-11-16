src=$(wildcard ./src/*.c) #取出所有的.c文件
obj=$(patsubst ./src/%.c,./out/%.out,$(src)) #src中匹配目标文件

ALL:$(obj) #指定目标
myargs=-Wall -g

$(obj):./out/%.out:./src/%.c
	gcc $^ -o $@ $(myargs)
clean:
	-rm -rf $(obj) #删除obj中所有文件
ls:
	ls -al ./out

.PHONY:clean ALL ls
