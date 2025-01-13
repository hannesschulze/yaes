cmake_minimum_required(VERSION 3.30)
project(
    nes
    LANGUAGES CXX OBJCXX OBJC
    VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(nes)
add_subdirectory(src)

find_library(FRAMEWORK_COCOA Cocoa REQUIRED)
find_library(FRAMEWORK_SPRITEKIT SpriteKit REQUIRED)
target_link_libraries(nes PRIVATE ${FRAMEWORK_COCOA} ${FRAMEWORK_SPRITEKIT})