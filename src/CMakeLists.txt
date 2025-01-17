add_subdirectory(nes)
#add_subdirectory(ref)
#add_subdirectory(test)

target_sources(
    nes
    PRIVATE
        main.mm
        status.hh
        scene.hh
        scene.mm)

target_include_directories(nes PRIVATE .)