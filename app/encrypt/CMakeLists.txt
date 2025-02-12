add_executable(nes_app_test)

target_include_directories(nes_app_test PRIVATE .)
target_link_libraries(
	nes_app_test
	PRIVATE
		nes::options
		nes::nes)

target_sources(
	nes_app_test
	PRIVATE
		main.cc)