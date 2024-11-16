#!/bin/bash

echo '输入 1 到 4 之间的数字:'
echo '你输入的数字为:'
while read aNum
    do
        case $aNum in
            1)  echo '你选择了 1'
                echo '你输入的数字为:'
                ;;
            2)  echo '你选择了 2'
                echo '你输入的数字为:'
                ;;
            3)  echo '你选择了 3'
                echo '你输入的数字为:'
                ;;
            4)  echo '你选择了 4'
                echo '你输入的数字为:'
                ;;
            *)  echo '你没有输入 1 到 4 之间的数字'
                echo '你输入的数字为:'
                ;;
        esac
    done
