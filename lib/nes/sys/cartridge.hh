#pragma once

#include "nes/sys/types/status.hh"
#include "nes/sys/mapper.hh"
#include "nes/common/types.hh"
#include <vector>
#include <string_view>

namespace nes::sys
{
	enum class name_table_arrangement
	{
		horizontal,
		vertical,
	};

	class cartridge
	{
		static constexpr auto prg_rom_bank_size = u32{ 16 * 1024 };
		static constexpr auto chr_rom_bank_size = u32{ 8 * 1024 };
		static constexpr auto ram_bank_size = u32{ 8 * 1024 };
		static constexpr auto max_prg_rom_size = u32{ 4 * prg_rom_bank_size };
		static constexpr auto max_chr_rom_size = u32{ 4 * chr_rom_bank_size };
		static constexpr auto max_ram_size = u32{ 4 * ram_bank_size };
		static constexpr auto header_length = u32{ 16 };

		struct header
		{
			auto get_magic_0() const -> u8 { return value[0]; }
			auto get_magic_1() const -> u8 { return value[1]; }
			auto get_magic_2() const -> u8 { return value[2]; }
			auto get_magic_3() const -> u8 { return value[3]; }
			auto get_prg_rom_banks() const -> u32 { return value[4]; }
			auto get_chr_rom_banks() const -> u32 { return value[5]; }
			auto get_control_1() const -> u8 { return value[6]; }
			auto get_control_2() const -> u8 { return value[7]; }
			auto get_ram_banks() const -> u32 { return value[8]; }
			auto get_has_trainer() const -> bool { return get_control_1() & 0b00000100; }
			auto get_mapper_low() const -> u32 { return (get_control_1() & 0b11110000) >> 4; }
			auto get_mapper_high() const -> u32 { return (get_control_2() & 0b11110000) >> 0; }
			auto get_name_table_arrangement() const -> name_table_arrangement { return static_cast<name_table_arrangement>((get_control_1() & 0b00000001) >> 0); }

			u8 value[header_length]{};

			explicit header(u8 const* v)
			{
				for (auto i = u32{ 0 }; i < header_length; ++i)
				{
					value[i] = v[i];
				}
			}
		};

		status status_{ status::error_invalid_ines_data };
		u8 prg_rom_[max_prg_rom_size]{};
		u8 chr_rom_[max_chr_rom_size]{};
		u8 ram_[max_ram_size]{};
		u32 prg_rom_size_{};
		u32 chr_rom_size_{};
		u32 ram_size_{};
		name_table_arrangement name_table_arrangement_{};
		mapper* mapper_{ &mapper::invalid() };

	public:
		explicit cartridge(u8 const* data, u32 length);

		auto get_status() const -> status { return status_; }
		auto get_prg_rom() const -> u8 const* { return prg_rom_; }
		auto get_prg_rom_length() const -> u32 { return prg_rom_size_; }
		auto get_chr_rom() const -> u8 const* { return chr_rom_; }
		auto get_chr_rom_length() const -> u32 { return chr_rom_size_; }
		auto get_ram() const -> u8 const* { return ram_; }
		auto get_ram_mut() -> u8* { return ram_; }
		auto get_ram_length() const -> u32 { return ram_size_; }
		auto get_mapper() const -> mapper& { return *mapper_; }
		auto get_name_table_arrangement() const -> name_table_arrangement { return name_table_arrangement_; }
	};
} // namespace nes::sys