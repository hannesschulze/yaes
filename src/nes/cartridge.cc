#include "nes/cartridge.hh"
#include <fstream>

namespace nes
{
	namespace
	{
		constexpr auto prg_rom_bank_size = std::size_t{ 16 * 1024 };
		constexpr auto chr_rom_bank_size = std::size_t{ 8 * 1024 };
		constexpr auto ram_bank_size = std::size_t{ 8 * 1024 };
	} // namespace

	auto cartridge::from_data(void const* data, std::size_t const length) -> cartridge
	{
		// See: https://www.nesdev.org/wiki/INES

		auto const bytes = static_cast<std::uint8_t const*>(data);
		auto res = cartridge{};
		res.is_valid_ = true;

		// Parse header.
		if (length < 16) { return cartridge{}; }
		if (bytes[0] != 'N' || bytes[1] != 'E' || bytes[2] != 'S' || bytes[3] != 0x1A) { return cartridge{}; }
		auto const prg_rom_banks = bytes[4];
		auto const chr_rom_banks = bytes[5];
		auto const control_1 = bytes[6];
		auto const control_2 = bytes[7];
		auto const ram_banks = (bytes[8] != 0) ? bytes[8] : 1;
		auto const has_trainer = (control_1 & 0b00000100) != 0;

		res.mapper_number_ =
			(control_1 & 0b11110000) >> 4 |
			(control_2 & 0b11110000) >> 0;
		res.name_table_arrangement_ = (control_1 & 0b00000001) != 0
			? name_table_arrangement::vertical
			: name_table_arrangement::horizontal;

		// Load program data.
		auto offset = std::size_t{ 16 };
		if (has_trainer) { offset += 512; }

		auto const prg_rom_size = prg_rom_banks * prg_rom_bank_size;
		if (length < offset + prg_rom_size) { return cartridge{}; }
		res.prg_rom_ = std::vector(&bytes[offset], &bytes[offset + prg_rom_size]);

		offset += prg_rom_size;

		auto const chr_rom_size = chr_rom_banks * chr_rom_bank_size;
		if (length < offset + chr_rom_size) { return cartridge{}; }
		res.chr_rom_ = std::vector(&bytes[offset], &bytes[offset + chr_rom_size]);

		res.ram_ = std::vector(ram_banks * ram_bank_size, std::uint8_t{ 0 });

		return res;
	}

	auto cartridge::from_file(std::string_view const path) -> cartridge
	{
		auto stream = std::ifstream{ path, std::ios::binary };
		if (!stream.good()) { return cartridge{}; }

		stream.seekg(0, std::ios::end);
		auto const length = stream.tellg();
		stream.seekg(0, std::ios::beg);
		auto data = std::vector<std::uint8_t>(length);
		stream.read(reinterpret_cast<char*>(data.data()), data.size());

		return from_data(data.data(), data.size());
	}
} // namespace nes