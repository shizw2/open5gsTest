pkill -9 open5gs
./build/src/nrf/open5gs-nrfd -e info &
./build/src/amf/open5gs-amfd -e info &
./build/src/amf/open5gs-amf-spsd -i 1 -e info &
./build/src/amf/open5gs-amf-spsd -i 2 -e info &
