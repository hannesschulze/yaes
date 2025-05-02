add_executable(nes_app_controller)

target_include_directories(nes_app_controller PRIVATE .)
target_link_libraries(
	nes_app_controller
	PRIVATE
		nes::options
		nes::nes)

target_sources(
	nes_app_controller
	PRIVATE
		main.cc
		serial-controller.cc)
