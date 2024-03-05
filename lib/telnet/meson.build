
libtelnet_sources = files('''
    telnet.h  
    telnet.c
'''.split())


libtelnet_inc = include_directories('.')

libtelnet = library('ogstelnet',
    sources : libtelnet_sources,
    version : libogslib_version,
    dependencies : [libcore_dep],    
    install : true)

libtelnet_dep = declare_dependency(
    link_with : libtelnet,
    dependencies : [libcore_dep],
    include_directories : [libtelnet_inc, libinc])
