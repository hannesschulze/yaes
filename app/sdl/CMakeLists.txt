add_executable(nes_app_sdl)

target_include_directories(nes_app_sdl PRIVATE .)
target_link_libraries(
	nes_app_sdl
	PRIVATE
		nes::options
		nes::nes
		SDL3::SDL3)

target_sources(
	nes_app_sdl
	PRIVATE
		main.cc
		state.hh
		state.cc)

add_subdirectory(impl)
