add_executable(nes_mac)

find_library(FRAMEWORK_COCOA Cocoa REQUIRED)
find_library(FRAMEWORK_SPRITEKIT SpriteKit REQUIRED)

target_include_directories(nes_mac PRIVATE .)
target_link_libraries(
	nes_mac
	PRIVATE
		nes::options
		nes::nes
		${FRAMEWORK_COCOA}
		${FRAMEWORK_SPRITEKIT})

target_sources(
	nes_mac
	PRIVATE
		main.mm
		scene.hh
		scene.mm)