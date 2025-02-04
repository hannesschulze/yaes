#pragma once

#include "nes/sys/mapper.hh"
#include "nes/common/status.hh"
#include "nes/common/types.hh"
#include "nes/common/span.hh"

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

			explicit header(span<u8 const, header_length> const v)
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
		static constexpr auto max_file_size = u32{ max_prg_rom_size + max_chr_rom_size + 512 + header_length };

		explicit cartridge(span<u8 const>);

		auto get_status() const -> status { return status_; }
		auto get_prg_rom() const -> span<u8 const> { return span{ prg_rom_, prg_rom_size_ }; }
		auto get_chr_rom() const -> span<u8 const> { return span{ chr_rom_, chr_rom_size_ }; }
		auto get_ram() const -> span<u8 const> { return span{ ram_, ram_size_ }; }
		auto ref_ram() -> span<u8> { return span{ ram_, ram_size_ }; }
		auto get_mapper() const -> mapper& { return *mapper_; }
		auto get_name_table_arrangement() const -> name_table_arrangement { return name_table_arrangement_; }
	};
} // namespace nes::sys