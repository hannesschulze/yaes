target_sources(
	nes
	PRIVATE
		application.hh
		application.cc)

add_subdirectory(input)
add_subdirectory(graphics)