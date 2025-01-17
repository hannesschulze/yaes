#include "nes/mapper.hh"
#include "nes/cartridge.hh"
#include <iostream>

namespace nes
{
	mapper::mapper(cartridge& cartridge, read_func const read, write_func const write)
		: cartridge_{ cartridge }
		, read_{ read }
		, write_{ write }
	{
	}

	auto mapper::select(cartridge& cartridge) -> mapper
	{
		switch (cartridge.get_mapper_number())
		{
			case 0x00:
				return mapper{ cartridge, &mapper::nrom_read, &mapper::nrom_write };
			default:
				std::cerr << "Mapper not implemented: " << std::hex << cartridge.get_mapper_number() << std::endl;
				std::abort();
		}
	}

	// NROM

	auto mapper::nrom_read(address const addr) -> std::uint8_t
	{
		if (addr <= address{ 0x5FFF })
		{
			return 0x0;
		}
		if (addr <= address{ 0x7FFF })
		{
			return cartridge_.get_ram()[addr.get_absolute() - 0x6000];
		}
		if (addr <= address{ 0xBFFF })
		{
			auto const rel = addr.get_absolute() - 0x8000;
			if (rel >= cartridge_.get_prg_rom_length()) { return 0x0; }

			return cartridge_.get_prg_rom()[rel];
		}
		if (addr <= address{ 0xFFFF })
		{
			auto rel = addr.get_absolute() - 0xC000;
			if (cartridge_.get_prg_rom_length() > 0x4000) { rel += 0x4000; }
			if (rel >= cartridge_.get_prg_rom_length()) { return 0x0; }

			return cartridge_.get_prg_rom()[rel];
		}

		return 0x0;
	}

	auto mapper::nrom_write(address const addr, std::uint8_t const value) -> void
	{
		if (addr <= address{ 0x5FFF })
		{
			return;
		}
		if (addr <= address{ 0x7FFF })
		{
			cartridge_.get_ram_mut()[addr.get_absolute() - 0x6000] = value;
		}
	}

} // namespace nes