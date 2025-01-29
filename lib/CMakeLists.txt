add_library(nes STATIC)
add_library(nes::nes ALIAS nes)

target_include_directories(nes PUBLIC .)
target_link_libraries(nes PRIVATE nes::options)

if(NES_ENABLE_PPU)
	target_compile_definitions(nes PUBLIC NES_ENABLE_PPU)
endif()
if(NES_ENABLE_DEBUG_OUTPUT)
	target_compile_definitions(nes PUBLIC NES_ENABLE_DEBUG_OUTPUT)
	target_link_libraries(nes PUBLIC spdlog::spdlog)
endif()

add_subdirectory(nes)