# Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>

# This file is part of 5G Core Network.

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

open5gs_sysconfdir = join_paths(sysconfdir, project_name)
systemd_sysconfdir = join_paths(open5gs_sysconfdir, 'systemd')
meson.add_install_script(python3_exe, '-c',
        mkdir_p.format(systemd_sysconfdir))

#这里不太好用变量替换，先写死
systemd_conf_in = '''
    5gc-mmed.service
    5gc-sgwcd.service
    5gc-smfd.service
    5gc-amfd.service
    5gc-sgwud.service
    5gc-upfd.service
    5gc-hssd.service
    5gc-pcrfd.service
    5gc-nrfd.service
    5gc-scpd.service
    5gc-ausfd.service
    5gc-udmd.service
    5gc-pcfd.service
    5gc-nssfd.service
    5gc-bsfd.service
    5gc-udrd.service
    5gc-amf-spsd@.service
    5gc-eird.service
'''.split()

foreach file : systemd_conf_in
    gen = configure_file(
            input : file + '.in',
            output : file,
            configuration : conf_data)
    meson.add_install_script(python3_exe, '-c',
            install_conf.format(gen, systemd_sysconfdir))
endforeach

systemd_conf = '''
    99-5gc.netdev
    99-5gc.network   
    udp_ini_conf.sh
'''.split()

foreach file : systemd_conf
    gen = configure_file(
            input : file,
            output : file,
            configuration : conf_data)
    meson.add_install_script(python3_exe, '-c',
            install_conf.format(gen, systemd_sysconfdir))        
endforeach
