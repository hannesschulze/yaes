#include "nes/sys/mapper.hh"
#include "nes/sys/cartridge.hh"

namespace nes::sys
{
	namespace
	{
		// -------------------------------------------------------------------------------------------------------------
		// NROM
		// -------------------------------------------------------------------------------------------------------------

		class mapper_nrom final : public mapper
		{
		public:
			explicit mapper_nrom() = default;

			auto validate(cartridge& cartridge) -> status override
			{
				if (cartridge.get_ram().get_length() != 0x2000)
				{
					return status::error_invalid_ines_data;
				}

				if (cartridge.get_prg_rom().get_length() != 0x4000 && cartridge.get_prg_rom().get_length() != 0x8000)
				{
					return status::error_invalid_ines_data;
				}

				if (cartridge.get_chr_rom().get_length() != 0x2000)
				{
					return status::error_invalid_ines_data;
				}

				return status::success;
			}

			auto read_cpu(address const addr, cartridge& cartridge) -> u8 override
			{
				if (addr <= address{ 0x5FFF })
				{
					return 0x0;
				}
				if (addr <= address{ 0x7FFF })
				{
					return cartridge.get_ram()[addr.get_absolute() - 0x6000];
				}
				if (addr <= address{ 0xBFFF })
				{
					auto const rel = addr.get_absolute() - 0x8000u;
					return cartridge.get_prg_rom()[rel];
				}
				if (addr <= address{ 0xFFFF })
				{
					auto rel = addr.get_absolute() - 0xC000u;
					if (cartridge.get_prg_rom().get_length() > 0x4000) { rel += 0x4000; }
					return cartridge.get_prg_rom()[rel];
				}

				return 0x0;
			}

			auto write_cpu(address const addr, u8 const value, cartridge& cartridge) -> void override
			{
				if (addr <= address{ 0x5FFF })
				{
					return;
				}
				if (addr <= address{ 0x7FFF })
				{
					cartridge.ref_ram()[addr.get_absolute() - 0x6000] = value;
					return;
				}
			}

			auto read_ppu(address const addr, cartridge& cartridge, span<u8 const> const vram) -> u8 override
			{
				if (addr <= address{ 0x1FFF })
				{
					auto const rel = addr.get_absolute();
					return cartridge.get_chr_rom()[rel];
				}
				if (addr <= address{ 0x3EFF })
				{
					return vram[mirrored_vram_address(addr, cartridge)];
				}

				return 0x0;
			}

			auto write_ppu(address const addr, u8 const value, cartridge& cartridge, span<u8> const vram) -> void override
			{
				if (addr <= address{ 0x1FFF })
				{
					return;
				}
				if (addr <= address{ 0x3EFF })
				{
					vram[mirrored_vram_address(addr, cartridge)] = value;
					return;
				}
			}

		private:
			static auto mirrored_vram_address(address const addr, cartridge const& cartridge) -> u32
			{
				auto const rel = addr.get_absolute() % 0x1000u;

				switch (cartridge.get_name_table_arrangement())
				{
					case name_table_arrangement::horizontal:
						// $2000 = $2400, $2800 = $2c00
						if (rel <= 0x3FF) { return 0x000u + (rel % 0x400u); }
						if (rel <= 0x7FF) { return 0x000u + (rel % 0x400u); }
						if (rel <= 0xBFF) { return 0x400u + (rel % 0x400u); }
						if (rel <= 0xFFF) { return 0x400u + (rel % 0x400u); }
						break;
					case name_table_arrangement::vertical:
						// $2000 = $2800, $2400 = $2c00
						if (rel <= 0x3FF) { return 0x000u + (rel % 0x400u); }
						if (rel <= 0x7FF) { return 0x400u + (rel % 0x400u); }
						if (rel <= 0xBFF) { return 0x000u + (rel % 0x400u); }
						if (rel <= 0xFFF) { return 0x400u + (rel % 0x400u); }
						break;
				}

				return 0;
			}
		};

		// -------------------------------------------------------------------------------------------------------------
		// Invalid
		// -------------------------------------------------------------------------------------------------------------

		class mapper_invalid final : public mapper
		{
		public:
			explicit mapper_invalid() = default;

			auto validate(cartridge&) -> status override { return status::error_unsupported_mapper; }
			auto read_cpu(address, cartridge&) -> u8 override { return 0; }
			auto write_cpu(address, u8, cartridge&) -> void override {}
			auto read_ppu(address, cartridge&, span<u8 const>) -> u8 override { return 0; }
			auto write_ppu(address, u8, cartridge&, span<u8>) -> void override {}
		};
	} // namespace

	// -----------------------------------------------------------------------------------------------------------------
	// Instances
	// -----------------------------------------------------------------------------------------------------------------

	auto mapper::invalid() -> mapper&
	{
		static auto instance = mapper_invalid{};
		return instance;
	}

	auto mapper::get(u8 const number) -> mapper&
	{
		switch (number)
		{
			case 0x00:
			{
				static auto instance = mapper_nrom{};
				return instance;
			}
			default:
			{
				return invalid();
			}
		}
	}
} // namespace nes::sys