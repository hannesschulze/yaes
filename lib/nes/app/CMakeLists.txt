target_sources(
	nes
	PRIVATE
		application.hh
		application.cc
		action.hh
		file-browser.hh
		preferences.hh)

add_subdirectory(input)
add_subdirectory(graphics)
add_subdirectory(ui)
add_subdirectory(crypto)