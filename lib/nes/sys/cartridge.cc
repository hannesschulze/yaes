#include "nes/sys/cartridge.hh"
#include "nes/sys/types/status.hh"
#include <fstream>

namespace nes::sys
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
		if (length < header_length) { return cartridge{ status::error_invalid_ines_data }; }
		auto const h = header{ bytes };
		if (h.get_magic_0() != 'N' || h.get_magic_1() != 'E' || h.get_magic_2() != 'S' || h.get_magic_3() != 0x1A)
		{
			return cartridge{ status::error_invalid_ines_data };
		}
		auto const mapper_number = static_cast<std::uint8_t>((h.get_mapper_high() << 4) | (h.get_mapper_low() << 0));

		res.mapper_ = &mapper::get(mapper_number);
		res.name_table_arrangement_ = h.get_name_table_arrangement();

		// Load program data.
		auto offset = std::size_t{ 16 };
		if (h.get_has_trainer()) { offset += 512; }

		auto const prg_rom_size = h.get_prg_rom_banks() * prg_rom_bank_size;
		if (length < offset + prg_rom_size) { return cartridge{ status::error_invalid_ines_data }; }
		res.prg_rom_ = std::vector(&bytes[offset], &bytes[offset + prg_rom_size]);

		offset += prg_rom_size;

		auto const chr_rom_size = h.get_chr_rom_banks() * chr_rom_bank_size;
		if (length < offset + chr_rom_size) { return cartridge{ status::error_invalid_ines_data }; }
		res.chr_rom_ = std::vector(&bytes[offset], &bytes[offset + chr_rom_size]);

		res.ram_ = std::vector(std::max(h.get_ram_banks(), 1u) * ram_bank_size, std::uint8_t{ 0 });

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
} // namespace nes::sys