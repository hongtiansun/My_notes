#!/bin/bash

if [ -f "./text" ];then
  echo "文件夹存在"
  else
  echo "文件夹不存在,创建文件"
  touch ./text
fi
