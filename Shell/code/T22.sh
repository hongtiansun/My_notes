#!/bin/bash
a=50
b=20
if [ $a == $b ]
then
   echo "a 等于 b"
elif [ $a -gt $b ]
then
   echo "a 大于 b"
elif (($a < $b))
then
   echo "a 小于 b"
else
   echo "没有符合的条件"
fi
