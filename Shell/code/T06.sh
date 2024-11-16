#!/bin/bash
declare -A site
site["google"]="www.google.com"
site["runoob"]="www.runoob.com"
site["taobao"]="www.taobao.com"

echo "数组的元素为: ${site[*]}"
echo "数组的元素为: ${site[@]}"

echo "数组的键值为: ${!site[*]}}"
echo "数组的键值为: ${!site[@]}}"
