#include "nes/ref/mapper.hh"
#include "nes/ref/cartridge.hh"
#include <iostream>

namespace nes::ref
{
	mapper::mapper(
		cartridge& cartridge,
		read_cpu_func const read_cpu, write_cpu_func const write_cpu,
		read_ppu_func const read_ppu, write_ppu_func const write_ppu)
		: cartridge_{ cartridge }
		, read_cpu_{ read_cpu }
		, write_cpu_{ write_cpu }
		, read_ppu_{ read_ppu }
		, write_ppu_{ write_ppu }
	{
	}

	auto mapper::select(cartridge& cartridge) -> mapper
	{
		switch (cartridge.get_mapper_number())
		{
			case 0x00:
				return mapper{
					cartridge,
					&mapper::nrom_read_cpu, &mapper::nrom_write_cpu,
					&mapper::nrom_read_ppu, &mapper::nrom_write_ppu };
			default:
				std::cerr << "Mapper not implemented: " << std::hex << cartridge.get_mapper_number() << std::endl;
				std::abort();
		}
	}

	// -----------------------------------------------------------------------------------------------------------------
	// NROM
	// -----------------------------------------------------------------------------------------------------------------

	auto mapper::nrom_read_cpu(address const addr) -> std::uint8_t
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

	auto mapper::nrom_write_cpu(address const addr, std::uint8_t const value) -> void
	{
		if (addr <= address{ 0x5FFF })
		{
			return;
		}
		if (addr <= address{ 0x7FFF })
		{
			cartridge_.get_ram_mut()[addr.get_absolute() - 0x6000] = value;
			return;
		}
	}

	auto mapper::nrom_read_ppu(address const addr, std::uint8_t const* vram) -> std::uint8_t
	{
		if (addr <= address{ 0x1FFF })
		{
			auto const rel = addr.get_absolute();
			if (rel >= cartridge_.get_chr_rom_length()) { return 0x0; }

			return cartridge_.get_chr_rom()[rel];
		}
		if (addr <= address{ 0x3EFF })
		{
			return vram[nrom_mirrored_vram_address(addr)];
		}

		return 0x0;
	}

	auto mapper::nrom_write_ppu(address const addr, std::uint8_t* vram, std::uint8_t const value) -> void
	{
		if (addr <= address{ 0x1FFF })
		{
			return;
		}
		if (addr <= address{ 0x3EFF })
		{
			vram[nrom_mirrored_vram_address(addr)] = value;
			return;
		}
	}

	auto mapper::nrom_mirrored_vram_address(address const addr) const -> unsigned
	{
		auto const rel = addr.get_absolute() % 0x1000;

		switch (cartridge_.get_name_table_arrangement())
		{
			case name_table_arrangement::horizontal:
				// $2000 = $2400, $2800 = $2c00
				if (rel <= 0x3FF) { return 0x000 + (rel % 0x400); }
				if (rel <= 0x7FF) { return 0x000 + (rel % 0x400); }
				if (rel <= 0xBFF) { return 0x400 + (rel % 0x400); }
				if (rel <= 0xFFF) { return 0x400 + (rel % 0x400); }
				break;
			case name_table_arrangement::vertical:
				// $2000 = $2800, $2400 = $2c00
				if (rel <= 0x3FF) { return 0x000 + (rel % 0x400); }
				if (rel <= 0x7FF) { return 0x400 + (rel % 0x400); }
				if (rel <= 0xBFF) { return 0x000 + (rel % 0x400); }
				if (rel <= 0xFFF) { return 0x400 + (rel % 0x400); }
				break;
		}

		return 0;
	}
} // namespace nes::ref