add_subdirectory(util)

target_sources(
    nes
    PRIVATE
        nes.hh
        nes.cc
        cartridge.hh
        cartridge.cc
        display.hh
        display.cc)