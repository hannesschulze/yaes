add_subdirectory(nes)

target_sources(
    nes
    PRIVATE
        main.mm
        scene.hh
        scene.mm)

target_include_directories(nes PRIVATE .)