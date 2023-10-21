#!/bin/bash
# 设置 sps_num 的默认值为 1
sps_num=${1:-1}
process_list=()

cd /home/5gc
./misc/netconf.sh 
./misc/udp_ini_conf.sh 
# 根据 sps_num 动态生成 process_list 数组
for ((i=1; i<=sps_num; i++))
do
	process_list+=("5gc-amf-spsd -i $i -l ./install/var/log/5gc/amf_sps$i.log")
done

process_list+=("5gc-amfd" "5gc-ausfd" "5gc-bsfd" "5gc-nrfd" "5gc-nssfd" "5gc-pcfd" "5gc-smfd" "5gc-udmd" "5gc-udrd" "5gc-upfd")

process_dir=./install/bin
log_file=/var/run/process_watchdog.log

echo "`date`: $0 start!" > $log_file
export export LD_LIBRARY_PATH=./lib/x86_64-linux-gnu/:./lib64:$LD_LIBRARY_PATH
ulimit -c unlimited

while true;
do

    for process in "${process_list[@]}"
    do
        process_no=`ps -ef | grep -i -E "/$process" | grep -v grep |grep -v watchdog | wc -l`
        if [ ${process_no} -eq 0 ]; then
            echo "`date`: $process die, restart!" >> $log_file
            pkill $process > /dev/null

            $process_dir/$process &

        fi
    done

        sleep 1
done

echo "`date`: $0 end!" > $log_file
