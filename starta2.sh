pkill -9 amf
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
./build/src/amf/5gc-amfd -e $level &
./build/src/amf/5gc-amfd -e $level -c ./install/etc/5gc/amf2.yaml -l ./install/var/log/5gc/amf2.log &
