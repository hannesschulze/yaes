add_subdirectory(util)

target_sources(
    nes
    PRIVATE
        nes.hh
        nes.cc
        display.hh
        display.cc)