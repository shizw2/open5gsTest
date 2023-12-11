一、发布版本使用说明
1、配置tun口
1.1  将99-5gc.netdev、99-5gc.network 拷贝至/etc/systemd/network
1.2  执行 systemctl restart network(centos)/systemctl restart systemd-networkd(ubuntu) 重启网络即可创建ogstun设备

2、配置内部网元交互的子接口IP
2.1   在/etc/rc.local文件中，添加如下代码：
   /home/5gc/misc/udp_ini_conf.sh 实际网卡名称
   
  如 /home/5gc/misc/udp_ini_conf.sh ens160
  
2.2	 执行 systemctl restart rc-local 完成子接口IP配置

注：Ubuntu默认未开启rc-local服务，可通过systemctl enable rc-local 开启rc-local。

3、将各个NF作为系统服务启动  
3.1 将各个NF的服务service文件拷贝至/etc/systemd/system目录
3.2 执行./service.sh enable #生效所有NF的服务
3.3 执行./service.sh start  #启动所有服务
3.4 执行./service.sh restart#重启所有服务
3.5 执行./service.sh stop   #停止所有服务
注: service.sh脚本只是批量enable/start/stop/restart各个NF的服务。
用户可直接通过systemctl start 来启动各个NF的服务，如：
systemctl start 5gc-amfd
systemctl start 5gc-smfd
systemctl start 5gc-amf-spsd@1 #启动第1个5gc-amf-spsd
systemctl start 5gc-amf-spsd@2 #启动第2个5gc-amf-spsd

./service.sh脚本默认启动一个amf-spsd，如要启动多个amf-spsd， 如以启动3个 amf-spsd为例，则：
执行./service.sh enable 3  #生效所有NF的服务，第二个参数3表示生成3个amf-spsd的服务
执行./service.sh start 3 #启动服务

4、重启机器，确保上述配置可以开机自启动
注：1~3步只需要首次搭建环境时执行，如相关服务脚本无变化，则后续不需要重新配置

5、如果目标机器提示找不到xxx库，则可以将编译机上的相关库拷贝到/home/5gc/install/lib64目录下，
然后在/etc/profile中增加如下代码后source /etc/profile 使其生效即可：
export LD_LIBRARY_PATH=/home/5gc/install/lib64:$LD_LIBRARY_PATH 

二、开发版本使用说明
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



