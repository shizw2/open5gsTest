interface=${1:-ens160}
num=${2:-2}

for ((i=0; i<num; i++))
do
  if [ $i -eq 0 ]; then
    ip_address="128.128.128.127"
  else
    ip_address="128.128.128.$i"
  fi
  ifconfig $interface:$i $ip_address
done
