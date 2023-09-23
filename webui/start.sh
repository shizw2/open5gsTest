#!/bin/bash

# 杀死现有进程
pkill node

# 设置环境变量并启动服务
if [ "$1" = "dev" ]; then
    #dev版本每次启动都会自动构建
    #注意,dev版本用于开发调试,不能后台运行(即使加了&)
    DB_URI=mongodb://127.0.0.1/5gc HOSTNAME=0.0.0.0 npm run dev &
else
    # rel版本需要先构建，再启动。 
    # 检查传入的参数是否为 "nobuild",如果是,并且已经构建过，则不再构建(可提高启动速度)      
    if [ "$1" == "nobuild" ] &&  [ -f ".next/BUILD_ID" ]; then
        :
    else
		npm run build
    fi
    DB_URI=mongodb://127.0.0.1/5gc HOSTNAME=0.0.0.0 npm run start &
fi
