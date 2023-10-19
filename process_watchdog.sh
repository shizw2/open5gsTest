#!/bin/bash

process_dir=./install/bin
process_list="5gc-amfd 5gc-amf-spsd 5gc-ausfd 5gc-bsfd 5gc-nrfd 5gc-nssfd 5gc-pcfd 5gc-smfd 5gc-udmd 5gc-udrd 5gc-upfd"
log_file=/var/run/process_watchdog.log

echo "`date`: $0 start!" > $log_file
export export LD_LIBRARY_PATH=./lib/x86_64-linux-gnu/:./lib64:$LD_LIBRARY_PATH
ulimit -c unlimited

while true;
do

    for process in $process_list
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
