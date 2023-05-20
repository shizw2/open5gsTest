pkill -9 open5gs
rm -rf ./install/var/log/open5gs/*
level="warn"

if [ ! -n "$1" ] ;then
	level="warn"
else
	level=$1
fi
spsno=1

if [ ! -n "$2" ] ;then
        spsno=1
else
	spsno=$2
fi
key=0
./build/src/nrf/open5gs-nrfd -e $level &
./build/src/amf/open5gs-amfd -e $level &
./build/src/ausf/open5gs-ausfd -e $level &
./build/src/ausf/open5gs-ausfd -e $level -c ./install/etc/open5gs/ausf2.yaml &
./build/src/udm/open5gs-udmd -e $level &
./build/src/udm/open5gs-udmd -e $level -c ./install/etc/open5gs/udm2.yaml &
./build/src/pcf/open5gs-pcfd -e $level &
./build/src/pcf/open5gs-pcfd -e $level -c ./install/etc/open5gs/pcf2.yaml &
./build/src/nssf/open5gs-nssfd -e $level &
./build/src/bsf/open5gs-bsfd -e $level &
./build/src/udr/open5gs-udrd -e $level &
./build/src/udr/open5gs-udrd -e $level -c ./install/etc/open5gs/udr2.yaml &
./build/src/upf/open5gs-upfd -e $level &
./build/src/upf/open5gs-upfd -e $level -c ./install/etc/open5gs/upf2.yaml &
for ((i=1; i<=$spsno; i++))
do
    key=$[${i}+9]	
    ./build/src/amf/open5gs-amf-spsd -i ${i} -k ${key} -e $level -l ./install/var/log/open5gs/amf_sps${i}.log &
done
./build/src/smf/open5gs-smfd -e $level -c ./install/etc/open5gs/smf2.yaml &
./build/src/smf/open5gs-smfd -e $level &
