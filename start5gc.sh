export export LD_LIBRARY_PATH=./lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH
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

./bin/5gc-nrfd -e $level &
./bin/5gc-amfd -e $level &
./bin/5gc-ausfd -e $level &
./bin/5gc-udmd -e $level &
./bin/5gc-pcfd -e $level &
./bin/5gc-nssfd -e $level &
./bin/5gc-bsfd -e $level &
./bin/5gc-udrd -e $level &
./bin/5gc-upfd -e $level &

for ((i=1; i<=$spsno; i++))
do
    ./bin/5gc-amf-spsd -i ${i} -e $level -l ./var/log/5gc/amf_sps${i}.log &
done
./bin/smf/5gc-smfd -e $level &
