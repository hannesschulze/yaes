#pragma once

#include "nes/sys/types/status.hh"
#include "nes/sys/mapper.hh"
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
		static constexpr auto header_length = std::size_t{ 16 };

		struct header
		{
			auto get_magic_0() const -> std::uint8_t { return value[0]; }
			auto get_magic_1() const -> std::uint8_t { return value[1]; }
			auto get_magic_2() const -> std::uint8_t { return value[2]; }
			auto get_magic_3() const -> std::uint8_t { return value[3]; }
			auto get_prg_rom_banks() const -> unsigned { return value[4]; }
			auto get_chr_rom_banks() const -> unsigned { return value[5]; }
			auto get_control_1() const -> std::uint8_t { return value[6]; }
			auto get_control_2() const -> std::uint8_t { return value[7]; }
			auto get_ram_banks() const -> unsigned { return value[8]; }
			auto get_has_trainer() const -> bool { return get_control_1() & 0b00000100; }
			auto get_mapper_low() const -> unsigned { return (get_control_1() & 0b11110000) >> 4; }
			auto get_mapper_high() const -> unsigned { return (get_control_2() & 0b11110000) >> 0; }
			auto get_name_table_arrangement() const -> name_table_arrangement { return static_cast<name_table_arrangement>((get_control_1() & 0b00000001) >> 0); }

			std::uint8_t value[header_length]{};

			explicit header(std::uint8_t const* v)
			{
				for (auto i = std::size_t{ 0 }; i < header_length; ++i)
				{
					value[i] = v[i];
				}
			}
		};

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