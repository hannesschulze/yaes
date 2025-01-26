#include "nes/cartridge.hh"
#include "nes/util/status.hh"
#include <fstream>

namespace nes
{
	cartridge::cartridge(status const status)
		: status_{ status }
	{
	}

	auto cartridge::from_data(void const* data, std::size_t const length) -> cartridge
	{
		// See: https://www.nesdev.org/wiki/INES

		auto const bytes = static_cast<std::uint8_t const*>(data);
		auto res = cartridge{ status::success };

		// Parse header.
		if (length < 16) { return cartridge{ status::error_invalid_ines_data }; }
		if (bytes[0] != 'N' || bytes[1] != 'E' || bytes[2] != 'S' || bytes[3] != 0x1A)
		{
			return cartridge{ status::error_invalid_ines_data };
		}
		auto const prg_rom_banks = bytes[4];
		auto const chr_rom_banks = bytes[5];
		auto const control_1 = bytes[6];
		auto const control_2 = bytes[7];
		auto const ram_banks = (bytes[8] != 0) ? bytes[8] : 1u;
		auto const has_trainer = (control_1 & 0b00000100) != 0;
		auto const mapper_number = static_cast<std::uint8_t>(
			(control_1 & 0b11110000) >> 4 |
			(control_2 & 0b11110000) >> 0);

		res.mapper_ = &mapper::get(mapper_number);
		res.name_table_arrangement_ = (control_1 & 0b00000001) != 0
			? name_table_arrangement::vertical
			: name_table_arrangement::horizontal;

		// Load program data.
		auto offset = std::size_t{ 16 };
		if (has_trainer) { offset += 512; }

		auto const prg_rom_size = prg_rom_banks * prg_rom_bank_size;
		if (length < offset + prg_rom_size) { return cartridge{ status::error_invalid_ines_data }; }
		res.prg_rom_ = std::vector(&bytes[offset], &bytes[offset + prg_rom_size]);

		offset += prg_rom_size;

		auto const chr_rom_size = chr_rom_banks * chr_rom_bank_size;
		if (length < offset + chr_rom_size) { return cartridge{ status::error_invalid_ines_data }; }
		res.chr_rom_ = std::vector(&bytes[offset], &bytes[offset + chr_rom_size]);

		res.ram_ = std::vector(ram_banks * ram_bank_size, std::uint8_t{ 0 });

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
		auto data = std::vector<std::uint8_t>(static_cast<std::size_t>(length));
		stream.read(reinterpret_cast<char*>(data.data()), length);

		return from_data(data.data(), data.size());
	}
} // namespace nes