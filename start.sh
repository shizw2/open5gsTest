pkill -9 5gc
level="warn"
spsno=1

if [ -n "$1" ] ;then
    if [ $1 -le 16 ] ;then
        spsno=$1
    else
        level=$1
    fi
fi

if [  -n "$2" ] ;then
    spsno=$2
fi

./build/src/nrf/5gc-nrfd -e $level &
./build/src/amf/5gc-amfd -e $level &
./build/src/ausf/5gc-ausfd -e $level &
./build/src/udm/5gc-udmd -e $level &
./build/src/udm/5gc-udmd -e $level -c ./install/etc/5gc/udm2.yaml & 
./build/src/pcf/5gc-pcfd -e $level &
./build/src/nssf/5gc-nssfd -e $level &
./build/src/bsf/5gc-bsfd -e $level &
./build/src/udr/5gc-udrd -e $level &
./build/src/upf/5gc-upfd -e $level &
#./build/src/upf/5gc-upfd -e $level -c ./install/etc/5gc/upf2.yaml &
for ((i=1; i<=$spsno; i++))
do
    ./build/src/amf/5gc-amf-spsd -i ${i} -e $level -l ./install/var/log/5gc/amf_sps${i}.log &
done
./build/src/smf/5gc-smfd -e $level &
#./build/src/smf/5gc-smfd -e $level -c ./install/etc/5gc/smf2.yaml &
