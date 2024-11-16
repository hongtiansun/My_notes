#!/bin/bash

a=5
b=6

result=$[a+b] # 注意等号两边不能有空格
result2=$((a+b))

echo "result 为： $result"
echo "result2 为: $result2"
