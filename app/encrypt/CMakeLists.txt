add_executable(nes_app_encrypt)

target_include_directories(nes_app_encrypt PRIVATE .)
target_link_libraries(
	nes_app_encrypt
	PRIVATE
		nes::options
		nes::nes)

target_sources(
	nes_app_encrypt
	PRIVATE
		main.cc)
