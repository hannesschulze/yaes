target_sources(
	nes
	PRIVATE
		color.hh
		image-view.hh
		mask-tile.hh
		image-tile.hh
		text-attributes.hh
		renderer.hh
		renderer.cc)

add_subdirectory(tiles)