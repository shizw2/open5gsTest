if [  -n "$2" ] ;then
    node=$2
fi

if [ -z "$node" ]; then
    pgrep -af '\.yaml' | grep -vE '([0-9])\.yaml' | awk '{print $1}' | xargs kill -9
else
    pgrep -f "${node}.yaml" | xargs kill -9
fi
level="warn"
spsno=1


if [ -n "$1" ] ;then
    if [ $1 -le 16 ] ;then
        spsno=$1
    else
        level=$1
    fi
fi



if [  -n "$3" ] ;then
    spsno=$3
fi



./build/src/nrf/5gc-nrfd -e $level -c ./install/etc/5gc/nrf${node:+${node}}.yaml   &
sleep 1
./build/src/amf/5gc-amfd -e $level -c ./install/etc/5gc/amf${node:+${node}}.yaml   &
./build/src/ausf/5gc-ausfd -e $level -c ./install/etc/5gc/ausf${node:+${node}}.yaml   &
./build/src/udm/5gc-udmd -e $level -c ./install/etc/5gc/udm${node:+${node}}.yaml   &
./build/src/pcf/5gc-pcfd -e $level -c ./install/etc/5gc/pcf${node:+${node}}.yaml   &
./build/src/nssf/5gc-nssfd -e $level -c ./install/etc/5gc/nssf${node:+${node}}.yaml   &
./build/src/bsf/5gc-bsfd -e $level -c ./install/etc/5gc/bsf${node:+${node}}.yaml   &
./build/src/udr/5gc-udrd -e $level -c ./install/etc/5gc/udr${node:+${node}}.yaml   &
./build/src/upf/5gc-upfd -e $level -c ./install/etc/5gc/upf${node:+${node}}.yaml  &
./build/src/smf/5gc-smfd -e $level -c ./install/etc/5gc/smf${node:+${node}}.yaml   &
for ((i=1; i<=$spsno; i++))
do
    ./build/src/amf/5gc-amf-spsd -i ${i} -e $level -c ./install/etc/5gc/amf${node:+${node}}.yaml -l ./install/var/log/5gc/amf_sps${node:+${node}}${i}.log &
done


