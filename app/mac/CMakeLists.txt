enable_language(OBJC OBJCXX)

add_executable(nes_app_mac)

find_library(FRAMEWORK_Cocoa Cocoa REQUIRED)
find_library(FRAMEWORK_SpriteKit SpriteKit REQUIRED)
find_library(FRAMEWORK_GameController GameController REQUIRED)

target_include_directories(nes_app_mac PRIVATE .)
target_link_libraries(
	nes_app_mac
	PRIVATE
		nes::options
		nes::nes
		${FRAMEWORK_Cocoa}
		${FRAMEWORK_SpriteKit}
		${FRAMEWORK_GameController})

target_sources(
	nes_app_mac
	PRIVATE
		main.mm
		scene.hh
		scene.mm
		view-controller.hh
		view-controller.mm)

add_subdirectory(impl)
