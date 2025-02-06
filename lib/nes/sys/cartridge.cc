#include "nes/sys/cartridge.hh"
#include "nes/common/status.hh"
#include "nes/common/utils.hh"

namespace nes::sys
{
	cartridge::cartridge(span<u8 const> const data)
	{
		// See: https://www.nesdev.org/wiki/INES

		// Parse header.
		if (data.get_length() < header_length) { return; }
		auto const h = header{ data.subspan<header_length>(0) };
		if (h.get_magic_0() != 'N' || h.get_magic_1() != 'E' || h.get_magic_2() != 'S' || h.get_magic_3() != 0x1A)
		{
			return;
		}
		auto const mapper_number = static_cast<u8>((h.get_mapper_high() << 4) | (h.get_mapper_low() << 0));

		mapper_ = &mapper::get(mapper_number);
		name_table_arrangement_ = h.get_name_table_arrangement();

		// Load program data.
		auto offset = u32{ 16 };
		if (h.get_has_trainer()) { offset += 512; }

		prg_rom_size_ = h.get_prg_rom_banks() * prg_rom_bank_size;
		if (data.get_length() < offset + prg_rom_size_) { return; }
		if (prg_rom_size_ > max_prg_rom_size) { return; }
		copy(&data[offset], prg_rom_, prg_rom_size_);

		offset += prg_rom_size_;

		chr_rom_size_ = h.get_chr_rom_banks() * chr_rom_bank_size;
		if (data.get_length() < offset + chr_rom_size_) { return; }
		if (chr_rom_size_ > max_chr_rom_size) { return; }
		copy(&data[offset], chr_rom_, chr_rom_size_);

		ram_size_ = max(h.get_ram_banks(), u32{ 1 }) * ram_bank_size;
		if (ram_size_ > max_ram_size) { return; }

		status_ = get_mapper().validate(*this);
		if (status_ != status::success) { return; }
	}
} // namespace nes::sys