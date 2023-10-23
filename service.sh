#!/bin/bash

action=$1
# 设置 sps_num 的默认值为 1
sps_num=${2:-1}

#process_list=("5gc-amfd" "5gc-amf-spsd" "5gc-ausfd" "5gc-bsfd" "5gc-nrfd" "5gc-nssfd" "5gc-pcfd" "5gc-smfd" "5gc-udmd" "5gc-udrd" "5gc-upfd")
process_list=("5gc-amf-spsd" "5gc-nrfd")

start_processes() {
    echo "Starting processes..."
    for process in "${process_list[@]}"
    do
        if [ "$process" == "5gc-amf-spsd" ]; then
            echo "Starting $sps_num instances of $process..."
            for ((i=1; i<=$sps_num; i++))
            do
                echo "Starting $process@$i..."
                sudo systemctl start $process@$i
            done
        else
            echo "Starting $process..."
            sudo systemctl start $process
        fi
    done
}

stop_processes() {
    echo "Stopping processes..."
    for process in "${process_list[@]}"
    do
        if [ "$process" == "5gc-amf-spsd" ]; then
            echo "Stopping $sps_num instances of $process..."
            for ((i=1; i<=$sps_num; i++))
            do
                echo "Stopping $process@$i..."
                sudo systemctl stop $process@$i
            done
        else
            echo "Stopping $process..."
            sudo systemctl stop $process
        fi
    done
}

restart_processes() {
    echo "Restarting processes..."
    for process in "${process_list[@]}"
    do
        if [ "$process" == "5gc-amf-spsd" ]; then
            echo "Restarting $sps_num instances of $process..."
            for ((i=1; i<=$sps_num; i++))
            do
                echo "Restarting $process@$i..."
                sudo systemctl restart $process@$i
            done
        else
            echo "Restarting $process..."
            sudo systemctl restart $process
        fi
    done
}

enable_processes() {
    echo "Enabling processes..."
    for process in "${process_list[@]}"
    do
        if [ "$process" == "5gc-amf-spsd" ]; then
            echo "Enabling $sps_num instances of $process..."
            for ((i=1; i<=$sps_num; i++))
            do
                echo "Enabling $process@$i..."
                sudo systemctl enable $process@$i
            done
        else
            echo "Enabling $process..."
            sudo systemctl enable $process
        fi
    done
}

if [ $# -lt 1 ]; then
    echo "Usage: $0 '<start|stop|restart|enable>'"
    exit 1
fi

case $action in
    start)
        start_processes
        ;;
    stop)
        stop_processes
        ;;
    restart)
        restart_processes
        ;;
    enable)
        enable_processes
        ;;
    *)
        echo "Invalid action. Use 'start', 'stop', 'restart', or 'enable'."
        exit 1
        ;;
esac
