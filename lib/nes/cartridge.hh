#pragma once

#include "nes/util/status.hh"
#include "nes/mapper.hh"
#include <cstddef>
#include <vector>
#include <string_view>

namespace nes
{
	enum class name_table_arrangement
	{
		horizontal,
		vertical,
	};

	class cartridge
	{
		static constexpr auto prg_rom_bank_size = std::size_t{ 16 * 1024 };
		static constexpr auto chr_rom_bank_size = std::size_t{ 8 * 1024 };
		static constexpr auto ram_bank_size = std::size_t{ 8 * 1024 };

		status status_{ status::error_uninitialized };
		std::vector<std::uint8_t> prg_rom_;
		std::vector<std::uint8_t> chr_rom_;
		std::vector<std::uint8_t> ram_;
		name_table_arrangement name_table_arrangement_{};
		mapper* mapper_{ &mapper::invalid() };

	public:
		explicit cartridge() = default;

		static auto from_data(void const* data, std::size_t length) -> cartridge;
		static auto from_file(std::string_view path) -> cartridge;

		auto get_status() const -> status { return status_; }
		auto get_prg_rom() const -> std::uint8_t const* { return prg_rom_.data(); }
		auto get_prg_rom_length() const -> std::size_t { return prg_rom_.size(); }
		auto get_chr_rom() const -> std::uint8_t const* { return chr_rom_.data(); }
		auto get_chr_rom_length() const -> std::size_t { return chr_rom_.size(); }
		auto get_ram() const -> std::uint8_t const* { return ram_.data(); }
		auto get_ram_mut() -> std::uint8_t* { return ram_.data(); }
		auto get_ram_length() const -> std::size_t { return ram_.size(); }
		auto get_mapper() const -> mapper& { return *mapper_; }
		auto get_name_table_arrangement() const -> name_table_arrangement { return name_table_arrangement_; }

	private:
		explicit cartridge(status);
	};
} // namespace nes