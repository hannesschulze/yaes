target_sources(
	nes
	PRIVATE
		types.hh
		debug.hh
		rgb.hh
		display.hh
		status.hh
		fps-counter.hh
		fps-counter.cc
		utils.hh)

add_subdirectory(containers)