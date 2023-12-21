#!/bin/bash

directory="/home/test/install/etc/5gc"
update_count=${1:-1}  # 命令行参数指定更新的次数，默认为1次

for ((i=0; i<update_count; i++)); do
    # 遍历目录下的所有YAML文件
    for file in "$directory"/*.yaml; do
        # 检查文件是否存在
        if [ -f "$file" ]; then
            # 获取当前时间
            current_time=$(date +"%Y%m%d%H%M.%S")
            # 修改文件的访问和修改时间为当前时间
            touch -a -m -t "$current_time" "$file"
	    # 如果是udm.yaml文件，则进行容量的替换操作,用于测试跟NRF的同步操作
            if [[ "$file" == *"/udm.yaml" ]]; then
                if grep -q "capacity: 100" "$file"; then
                    sed -i 's/capacity: 100/capacity: 99/g' "$file"
                elif grep -q "capacity: 99" "$file"; then
                    sed -i 's/capacity: 99/capacity: 100/g' "$file"
                fi
            fi
        fi
    done

    # 判断是否需要 sleep
    if [ "$i" -lt "$((update_count-1))" ]; then
	    echo "updateed $((i+1)) times, sleep 5s for another update."
	sleep 3
    fi
done

