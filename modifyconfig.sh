#!/bin/bash

directory="/home/test/install/etc/5gc"

# 遍历目录下的所有YAML文件
for file in "$directory"/*.yaml; do
    # 检查文件是否存在
    if [ -f "$file" ]; then
        # 获取当前时间
        current_time=$(date +"%Y%m%d%H%M.%S") 
        # 修改文件的访问和修改时间为当前时间
        touch -a -m -t "$current_time" "$file"
        
        #echo "Modified timestamp of $file"
    fi
done

