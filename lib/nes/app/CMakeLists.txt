target_sources(
	nes
	PRIVATE
		application.hh
		application.cc
		action.hh
		file-browser.hh)

add_subdirectory(input)
add_subdirectory(graphics)
add_subdirectory(ui)