
libtelnet_sources = files('''
    telnet.h  
    telnet.c
'''.split())


libtelnet_inc = include_directories('.')

libtelnet = library('ogstelnet',
    sources : libtelnet_sources,
    version : libogslib_version,    
    install : true)

libtelnet_dep = declare_dependency(
    link_with : libtelnet,
    include_directories : [libtelnet_inc, libinc])
