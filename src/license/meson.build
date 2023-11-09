license_sources = files('''
    MakeLicense.c
'''.split())

executable(project_name + '-MakeLicense',
    sources : license_sources,
    include_directories : srcinc,
    dependencies : liblicense_dep,
    install_rpath : libdir,
    install : true)

license_sources = files('''
    CheckLicense.c
'''.split())

executable(project_name + '-CheckLicense',
    sources : license_sources,
    include_directories : srcinc,
    dependencies : liblicense_dep,
    install_rpath : libdir,
    install : true)
    
license_sources = files('''
    MakeMachineID.c
'''.split())

executable(project_name + '-MakeMachineID',
    sources : license_sources,
    include_directories : srcinc,
    dependencies : liblicense_dep,
    install_rpath : libdir,
    install : true)