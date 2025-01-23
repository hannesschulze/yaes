target_sources(
	nes
	PRIVATE
		nes.hh
		nes.cc
		cartridge.hh
		cartridge.cc
		controller.hh
		controller.cc
		cpu.hh
		cpu.cc
		ppu.hh
		ppu.cc
		mapper.hh
		mapper.cc)

add_subdirectory(util)
add_subdirectory(ref)