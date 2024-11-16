#!/bin/bash
# author:菜鸟教程
# url:www.runoob.com

echo "Shell 传递参数实例！";
echo "第一个参数为：$1";

echo "当前进程ID $$"
echo "后台最后一个进程ID $!"
echo "参数个数为：$#";
echo "当前选项 $-"
echo "最后一个命令的退出状态 $?"

echo "传递的参数作为一个字符串显示：$*";
echo "传递的参数多个字符串显示：$@"
