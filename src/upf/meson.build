# Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>

# This file is part of Open5GS.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

upf_conf = configuration_data()

upf_headers = ('''
    ifaddrs.h
    net/ethernet.h
    net/if.h
    net/if_dl.h
    netinet/in.h
    netinet/ip.h
    netinet/ip6.h
    netinet/ip_icmp.h
    netinet/icmp6.h
    sys/ioctl.h
    sys/socket.h
'''.split())

foreach h : upf_headers
    if cc.has_header(h)
        define = 'HAVE_' + h.underscorify().to_upper()
        upf_conf.set(define, 1)
    endif
endforeach

if have_func_kqueue
    upf_conf.set('HAVE_KQUEUE', 1)
endif

configure_file(output : 'upf-config.h', configuration : upf_conf)

libupf_sources = files('''
    rule-match.h
    event.h
    timer.h
    metrics.h
    context.h
    upf-sm.h
    gtp-path.h
    pfcp-path.h
    n4-build.h
    n4-handler.h

    rule-match.c
    init.c
    metrics.c
    event.c
    timer.c
    context.c
    upf-sm.c
    pfcp-sm.c
    gtp-path.c
    pfcp-path.c
    n4-build.c
    n4-handler.c
    telnetcmd.c
'''.split())

libtins_dep = dependency('libtins',
    version: ['>=3.4'],
    fallback: ['libtins', 'libtins_dep'])

libarp_nd = static_library('arp_nd',
    sources : files('''
      arp-nd.cpp
      arp-nd.h
    '''.split()),
    dependencies : libtins_dep,
    install : false)
libarp_nd_dep = declare_dependency(
    link_with : libarp_nd,
    dependencies : libtins_dep)

#libnuma_dep = dependency('numa')
libdl_dep = meson.get_compiler('c').find_library('dl', dirs : ['/usr/lib64'])

cmd = run_command('sh', '-c', 'echo $RTE_SDK')
RTE_SDK = cmd.stdout().strip()
cmd = run_command('sh', '-c', 'echo $RTE_TARGET')
RTE_TARGET = cmd.stdout().strip()

libdpdk_dep = dependency('libdpdk')

libupf = static_library('upf',
    sources : libupf_sources,
    dependencies : [
        libmetrics_dep,
        libpfcp_dep,
        libtun_dep,
        liblicense_dep,
        libarp_nd_dep,
        libtelnet_dep,
    ],
    install : false)

libupf_dep = declare_dependency(
    link_with : libupf,
    dependencies : [
        libmetrics_dep,
        libpfcp_dep,
        libtun_dep,
        liblicense_dep,
        libarp_nd_dep,
        libtelnet_dep,
    ])

upf_sources = files('''
    app.c
    ../main.c
'''.split())

executable(project_name + '-upfd',
    sources : upf_sources,
    c_args : '-DDEFAULT_CONFIG_FILENAME="@0@/upf.yaml"'.format(open5gs_sysconfdir),
    include_directories : srcinc,
    dependencies : libupf_dep,
    install_rpath : libdir,
    install : true)

# ------------------ UPF DPDK VERSION ------------------
libupf_dpdk_sources = libupf_sources + files('''
    dpdk-arp-nd.h
    dispatcher.c
    forward.c
    dpdk-arp-nd.c
    upf-dpdk.h
    ctrl-path.h
    dpdk-path.h
    hash.h
    tw_timer.h
    tw_timer.c
    ctrl-path.c
    dpdk-path.c
    upf-dpdk.c
'''.split())

if get_option('release') == 'on'
    c_args = ['-DVER_RELEASE', '-DUSE_DPDK',]
else
    c_args = ['-DUSE_DPDK',]
endif

libupf_dpdk = static_library('upf-dpdk',
    sources : libupf_dpdk_sources,
    c_args: c_args,
    dependencies : [
        libdpdk_dep,
        libapp_dep,
        libgtp_dep,
        libpfcp_dpdk_dep,
        libtun_dep,
        libarp_nd_dep,
        libmetrics_dep,
        libtelnet_dep,
    ],
    install : false)

libupf_dpdk_dep = declare_dependency(
    link_with : libupf_dpdk,
    #c_args: c_args,
    dependencies : [
        libdpdk_dep,
        libapp_dep,
        libgtp_dep,
        libpfcp_dpdk_dep,
        libtun_dep,
        libarp_nd_dep,  
        #libnuma_dep,
        libdl_dep,
        libmetrics_dep,
        libtelnet_dep,
    ])

executable('5gc-upf-dpdkd',
    sources : upf_sources,
    c_args : '-DDEFAULT_CONFIG_FILENAME="@0@/upf.yaml"'.format(open5gs_sysconfdir),
    include_directories : srcinc,
    dependencies : libupf_dpdk_dep,
    install_rpath : libdir,
    install : true)
