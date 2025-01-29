target_sources(
	nes
	PRIVATE
		cartridge.hh
		cartridge.cc
		controller.hh
		controller.cc
		cpu.hh
		cpu.cc
		ppu.hh
		ppu.cc
		mapper.hh
		mapper.cc
		nes.hh
		nes.cc)

add_subdirectory(types)