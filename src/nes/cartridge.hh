#pragma once

#include <cstddef>
#include <vector>
#include <string_view>

namespace nes
{
	class cartridge
	{
		bool is_valid_{ false };
		std::vector<std::uint8_t> prg_rom_;
		std::vector<std::uint8_t> chr_rom_;
		std::vector<std::uint8_t> ram_;
		std::uint8_t mapper_number_{ 0 };

	public:
		explicit cartridge() = default;

		static auto from_data(void const* data, std::size_t length) -> cartridge;
		static auto from_file(std::string_view path) -> cartridge;

		auto is_valid() const -> bool { return is_valid_; }
		auto get_prg_rom() const -> std::uint8_t const* { return prg_rom_.data(); }
		auto get_prg_rom_length() const -> std::size_t { return prg_rom_.size(); }
		auto get_chr_rom() const -> std::uint8_t const* { return chr_rom_.data(); }
		auto get_chr_rom_length() const -> std::size_t { return chr_rom_.size(); }
		auto get_ram() const -> std::uint8_t const* { return ram_.data(); }
		auto get_ram_mut() -> std::uint8_t* { return ram_.data(); }
		auto get_ram_length() const -> std::size_t { return ram_.size(); }
		auto get_mapper_number() const -> std::uint8_t { return mapper_number_; }
	};
} // namespace nes