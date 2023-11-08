
liblicense_sources = files('''
    psMD5.h
    psMD5.c
    license.c
'''.split())


liblicense_inc = include_directories('.')

liblicense = library('ogslicense',
    sources : liblicense_sources,
    version : libogslib_version,    
    install : true)

liblicense_dep = declare_dependency(
    link_with : liblicense,
    include_directories : [liblicense_inc, libinc])
