#!/bin/bash

# 遍历当前目录及子目录下所有名为2.yaml的文件
find ./install/etc/ -type f -name "*2.yaml" -print0 | while IFS= read -r -d '' file; do
    echo "Processing $file"

    # 使用sed进行就地编辑（直接修改文件）
    # 在capacity: 100下添加 group: 1 和 node: 1
    sed -i '/capacity: 100/a \
      group: 1\
      node: 2' "$file"
done

echo "All 2.yaml files have been updated."
