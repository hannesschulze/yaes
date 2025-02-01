#include "nes/sys/cartridge.hh"
#include "nes/sys/types/status.hh"
#include <fstream>
#include <algorithm>

namespace nes::sys
{
	cartridge::cartridge(status const status)
		: status_{ status }
	{
	}

	auto cartridge::from_data(void const* data, u32 const length) -> cartridge
	{
		// See: https://www.nesdev.org/wiki/INES

		auto const bytes = static_cast<u8 const*>(data);
		auto res = cartridge{ status::success };

		// Parse header.
		if (length < header_length) { return cartridge{ status::error_invalid_ines_data }; }
		auto const h = header{ bytes };
		if (h.get_magic_0() != 'N' || h.get_magic_1() != 'E' || h.get_magic_2() != 'S' || h.get_magic_3() != 0x1A)
		{
			return cartridge{ status::error_invalid_ines_data };
		}
		auto const mapper_number = static_cast<u8>((h.get_mapper_high() << 4) | (h.get_mapper_low() << 0));

		res.mapper_ = &mapper::get(mapper_number);
		res.name_table_arrangement_ = h.get_name_table_arrangement();

		// Load program data.
		auto offset = u32{ 16 };
		if (h.get_has_trainer()) { offset += 512; }

		res.prg_rom_size_ = h.get_prg_rom_banks() * prg_rom_bank_size;
		if (length < offset + res.prg_rom_size_) { return cartridge{ status::error_invalid_ines_data }; }
		if (res.prg_rom_size_ > max_prg_rom_size) { return cartridge{ status::error_invalid_ines_data }; }
		std::copy_n(&bytes[offset], res.prg_rom_size_, res.prg_rom_);

		offset += res.prg_rom_size_;

		res.chr_rom_size_ = h.get_chr_rom_banks() * chr_rom_bank_size;
		if (length < offset + res.chr_rom_size_) { return cartridge{ status::error_invalid_ines_data }; }
		if (res.chr_rom_size_ > max_chr_rom_size) { return cartridge{ status::error_invalid_ines_data }; }
		std::copy_n(&bytes[offset], res.chr_rom_size_, res.chr_rom_);

		res.ram_size_ = std::max(h.get_ram_banks(), 1u) * ram_bank_size;
		if (res.ram_size_ > max_ram_size) { return cartridge{ status::error_invalid_ines_data }; }

		if (auto const status = res.get_mapper().validate(res); status != status::success)
		{
			return cartridge{ status };
		}

		return res;
	}

	auto cartridge::from_file(std::string_view const path) -> cartridge
	{
		auto stream = std::ifstream{ path, std::ios::binary };
		if (!stream.good()) { return cartridge{ status::error_system_error }; }

		stream.seekg(0, std::ios::end);
		auto const length = stream.tellg();
		stream.seekg(0, std::ios::beg);
		auto data = std::vector<u8>(static_cast<u32>(length));
		stream.read(reinterpret_cast<char*>(data.data()), length);

		return from_data(data.data(), static_cast<u32>(length));
	}
} // namespace nes::sys