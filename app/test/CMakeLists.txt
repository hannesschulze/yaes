add_executable(nes_test)

target_include_directories(nes_test PRIVATE .)
target_link_libraries(
	nes_test
	PRIVATE
		nes::options
		nes::nes
		nes::ref)

target_sources(
	nes_test
	PRIVATE
		main.cc)