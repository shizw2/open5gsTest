#!/bin/bash

process_dir=/home/test/install/bin
process_list="5gc-udmd"
log_file=/var/run/process_watchdog.log

echo "`date`: $0 start!" > $log_file
export LD_LIBRARY_PATH=/home/test/install/lib:$LD_LIBRARY_PATH
ulimit -c unlimited

while true;
do

    for process in $process_list
    do
        process_no=`ps -ef | grep -i -E "/$process" | grep -v grep |grep -v log |grep -v watchdog | wc -l`
        if [ ${process_no} -eq 0 ]; then
            echo "`date`: $process die, restart!" >> $log_file
            pkill $process > /dev/null

            $process_dir/$process &

        fi
    done

        sleep 1
done

echo "`date`: $0 end!" > $log_file
