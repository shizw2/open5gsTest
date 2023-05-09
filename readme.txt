1、配置文件
amf.yaml中，新增icps配置项，包含spsnum(支持的最大sps模块数,默认值16)，port(icps跟sps之间的udp通信端口，默认值9777)。

例如：

amf:
    icps:
        spsnum: 3
        port: 9777
		
2、为icps和各个sps模块配置内部通信ip。
其中，icps的IP固定为128.128.128.127，sps的IP为128.128.128.模块号
例如
ifconfig ens33:0 128.128.128.127
ifconfig ens33:1 128.128.128.1
ifconfig ens33:2 128.128.128.2
ifconfig ens33:3 128.128.128.3
ifconfig ens33:4 128.128.128.4
ifconfig ens33:5 128.128.128.5

可以通过执行./misc/udp_ini_conf.sh 设置IP。	

3、启动脚本	

目前测试阶段，没有将各个进程注册为系统服务，可以通过执行start.sh直接启动各个网元。
例如：
./start.sh warn 1 #启动1个amf-sps进程，程序日志级别为warn
./start.sh info 2 #启动2个amf-sps进程，程序日志级别为info

这里，除了amf外，其他网元的启动方式跟之前没有变化。
amf拆分为icps模块（open5gs-amfd）和sps模块(open5gs-amf-spsd)，所以启动方式稍有不同。
他们共用一个配置文件。
open5gs-amf-spsd 启动时，需要通过参数i携带本sps的模块号。

例如：
./build/src/amf/open5gs-amf-spsd -i 1 -e $level -l ./install/var/log/open5gs/amf_sps1.log &
./build/src/amf/open5gs-amf-spsd -i 2 -e $level -l ./install/var/log/open5gs/amf_sps2.log &



