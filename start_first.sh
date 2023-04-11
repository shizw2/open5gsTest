pkill -9 open5gs
level="info"

if [ ! -n "$1" ] ;then
	level="info"
else
	level=$1
fi

./build/src/nrf/open5gs-nrfd -e $level &
./build/src/amf/open5gs-amfd -e $level &
./build/src/amf/open5gs-amf-spsd -i 1 -e $level -l ./install/var/log/open5gs/amf_sps1.log 
#./build/src/amf/open5gs-amf-spsd -i 2 -e $level -l /home/git/open5gsTest/install/var/log/open5gs/amf_sps2.log &
