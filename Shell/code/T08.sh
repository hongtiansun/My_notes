#!/bin/bash
# author:菜鸟教程
# url:www.runoob.com

a=10 #默认字符串
b=20 

val=`expr $a + $b` #字符串同样可以识别
echo "a + b : $val"

val=`expr $a - $b`
echo "a - b : $val"

val=`expr $a \* $b` #乘号需要进行转义
echo "a * b : $val"

val=`expr $b / $a`
echo "b / a : $val"

val=`expr $b % $a`
echo "b % a : $val"

if [ $a == $b ] #方括号括起来且两侧需要空格
then
   echo "a 等于 b"
fi
if [ $a != $b ]
then
   echo "a 不等于 b"
fi
