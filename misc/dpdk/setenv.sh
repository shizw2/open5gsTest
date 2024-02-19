cd /home/5gc/misc/dpdk
# 获取当前系统的内核版本  
kernel_version=$(uname -r)  
# 提取主版本号  
major_version=${kernel_version%%.*}  
minor_version=${kernel_version#*.}  
minor_version=${minor_version%%.*}  
  
# 定义函数来加载指定目录下的igb_uio.ko模块  
load_module() {  
    local dir=$1  
    echo "Loading igb_uio.ko from $dir..."  
    insmod "$dir/igb_uio.ko"  
    if [ $? -eq 0 ]; then  
        echo "igb_uio.ko loaded successfully."  
    else  
        echo "Failed to load igb_uio.ko from $dir."  
        exit 1  
    fi  
}  
  
if [ "$1" == "1" ]; then
  # 获取当前的 DPDK 驱动程序信息
  driver_info=$(./dpdk-devbind.py -s)

  # 通过正则表达式匹配出 dpdk_uio 驱动绑定的设备
  regex="([0-9a-fA-F]{4}:[0-9a-fA-F]{2}:[0-9a-fA-F]{2}\.[0-9a-fA-F]) '[^']*' drv=igb_uio unused=(\w+)"
  while [[ $driver_info =~ $regex ]]; do
    # 提取设备标识
    device=${BASH_REMATCH[1]}
    unused=${BASH_REMATCH[2]}
  
    echo "恢复设备 $device 为内核绑定模式 $unused"
  
    # 使用 dpdk-devbind.py 将设备绑定回内核驱动程序
    ./dpdk-devbind.py --bind="$unused" "$device"

    # 移除已处理的驱动信息，以便下一次循环
    driver_info=${driver_info/${BASH_REMATCH[0]}/}
  done

  echo "恢复完成"

else
  # 关闭 ens192 和 ens224 网络设备
  ifconfig ens192 down
  ifconfig ens224 down

  # 加载 uio 和 igb 内核模块
  modprobe uio
  modprobe igb
  # 判断内核版本并加载对应模块  
  if [[ $major_version -lt 5 || ( $major_version -eq 5 && $minor_version -le 9 ) ]]; then
    load_module "kernel/ubuntu20_kernel5.4"
  elif [[ $major_version -lt 6 || ( $major_version -eq 6 && $minor_version -lt 5 ) ]]; then
    load_module "kernel/ubuntu22_kernel5.9"
  else
    load_module "kernel/ubuntu22_kernel6.5"
  fi
  # 使用 dpdk-devbind.py 将网络设备绑定到 igb_uio
  ./dpdk-devbind.py --bind=igb_uio ens192
  ./dpdk-devbind.py --bind=igb_uio ens224

  # 设置 HugePages 页面数量为 1024
  echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

  echo "DPDK环境设置完成"
fi

