pkill -9 amf
pkill -9 nrf
level="warn"

if [ -n "$1" ] ;then
    level=$1
fi


./build/src/nrf/5gc-nrfd -e $level &
./build/src/amf/5gc-amfd -e $level &
./build/src/amf/5gc-amfd -e $level -c ./install/etc/5gc/amf2.yaml -l ./install/var/log/5gc/amf2.log &
./build/src/amf/5gc-amfd -e $level -c ./install/etc/5gc/amf3.yaml -l ./install/var/log/5gc/amf3.log &
#./build/src/amf/5gc-amfd -e $level -c ./install/etc/5gc/amf4.yaml -l ./install/var/log/5gc/amf4.log &
