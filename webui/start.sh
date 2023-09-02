#!/bin/bash

# 杀死现有进程
pkill node

# 设置环境变量并启动服务
DB_URI=mongodb://127.0.0.1/5gc HOSTNAME=0.0.0.0 npm run dev &
