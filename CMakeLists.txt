cmake_minimum_required(VERSION 3.30)
project(
    nes
    LANGUAGES CXX OBJCXX OBJC
    VERSION 1.0)

option(NES_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer." OFF)
option(NES_ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer." OFF)
option(NES_ENABLE_DEBUG_OUTPUT "Enable debug output." OFF)
option(NES_ENABLE_PPU "Enable Picture Processing Unit." ON)

add_library(nes_options INTERFACE)
add_library(nes::options ALIAS nes_options)
target_compile_features(nes_options INTERFACE cxx_std_17)
target_compile_options(nes_options
    INTERFACE
        -Wall
        -Wextra
        -Wnon-virtual-dtor
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wformat=2
        -Wswitch-enum
        -Wno-gnu-anonymous-struct)
if(APPLE)
    # Use automatic reference counting for Objective-C code.
    target_compile_options(nes_options INTERFACE "-fobjc-arc")
endif()
if(NES_ENABLE_SANITIZER_ADDRESS AND NES_ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
    message(FATAL_ERROR "Address sanitizer and undefined behavior sanitizer cannot be enabled at the same time.")
elseif(NES_ENABLE_SANITIZER_ADDRESS)
    target_compile_options(nes_options INTERFACE -fsanitize=address)
elseif(NES_ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
    target_compile_options(nes_options INTERFACE -fsanitize=undefined)
endif()

if(NES_ENABLE_DEBUG_OUTPUT)
    include(FetchContent)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.0)
    FetchContent_MakeAvailable(spdlog)
endif()

add_subdirectory(lib)
add_subdirectory(app)