#!/bin/bash

# 函数：检查服务是否存在，并使用systemctl启动
start_service_if_exists() {
    SERVICE_NAME="5gc-webui.service"
    # 检查服务文件是否存在
    if [ -e "/etc/systemd/system/$SERVICE_NAME" ]; then
        echo "服务 $SERVICE_NAME 存在，使用 systemctl 启动服务。"
        # 启动服务
        systemctl start "$SERVICE_NAME"
        # 等待服务启动
        systemctl status "$SERVICE_NAME" --wait
        exit 0
    else
        echo "服务 $SERVICE_NAME 不存在，按常规方式启动应用。"
        # 常规启动代码
        DB_URI=mongodb://127.0.0.1/5gc HOSTNAME=0.0.0.0 npm run start &
        exit 0
    fi
}

# 杀死现有进程
pkill -f 'node server'

# 如果脚本在别的目录下执行, 必须要先进入webui目录，否则启动不成功
cd /home/5gc/webui

# 设置环境变量并启动服务
if [ "$1" = "dev" ]; then
    # dev版本每次启动都会自动构建
    DB_URI=mongodb://127.0.0.1/5gc HOSTNAME=0.0.0.0 npm run dev &
else
    # rel版本需要先构建，再启动
    if [ "$1" == "nobuild" ] && [ -f ".next/BUILD_ID" ]; then
        :
    else
        npm run build
    fi
    # 检查并尝试启动服务
    start_service_if_exists
fi