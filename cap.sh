
#output 文件。
output_filenow="tcpdump_output.pcap"
output_file="tcpdump_output"

# 待关闭的 TCPdump 进程 PID
tcpdump_cur_pid=
tcpdump_pre_pid=
 
# 指定每个文件的最大大小，以 M 为单位。
max_size=30
 
# 实际大小计量单位: 字节
max_bytes=$((${max_size} * 1024 * 1024))
 
# 指定要保留的文件数量上限。
max_files=10
 
mv "${output_filenow}" "${output_file}_$(date +%Y%m%d-%H%M%S).pcap"
 
tcpdump  -i any -w  "${output_filenow}" &
tcpdump_cur_pid=$!
tcpdump_pre_pid=${tcpdump_cur_pid}
 
while true; do
  # 获取输出文件的当前大小（单位：字节）。
  file_size=$(stat -c%s ${output_filenow})
 
  # 如果文件大小超过了指定大小，则创建新的输出文件，
  # 并将TCPdump命令的输出重定向到新文件。
  if [[ "${file_size}" -ge "${max_bytes}" ]]; then
	mv "${output_filenow}" "${output_file}_$(date +%Y%m%d-%H%M%S).pcap"
 
	# 启动 TCPdump 命令，将输出重定向至新的指定文件。
    # 输出包括时间戳和所有数据包。
    tcpdump  -i any -w "${output_filenow}" &
    tcpdump_cur_pid=$!
 
    # echo "Started capture with PID=${tcpdump_cur_pid}"
	
	# 在启动Tcpdump之后，关闭之前的进程
    if [[ -n "${tcpdump_pre_pid}" ]]; then
		
        # echo "Stopping capture with PID=${tcpdump_pre_pid}"
        kill ${tcpdump_pre_pid}
		tcpdump_pre_pid=${tcpdump_cur_pid}
    fi
 
  fi
 
  # 删除旧的输出文件以保持指定数量上限。
  file_count=$(ls -A ${PWD} | grep ^tcpdump_output_ | wc -l)
  if [[ "${file_count}" -gt "${max_files}" ]]; then
     # echo "File count (${file_count}) is greater than limit (${max_files}). Removing oldest file."
     oldest_file=$(ls -tr1 ${PWD} | grep ^tcpdump_output_ | head -1)
     rm -f ${oldest_file}
  fi
      
  # 等待一秒钟再次检查文件大小和数量。
  sleep 1
 
done
