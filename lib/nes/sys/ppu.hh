#pragma once

#include "nes/sys/types/cycle-count.hh"
#include "nes/common/types.hh"
#include "nes/common/span.hh"

namespace nes
{
	class display;
	class rgb;
} // namespace nes

namespace nes::sys
{
	class address;
	class cpu;
	class cartridge;
	struct snapshot;

	class ppu
	{
		static constexpr auto sprite_max_count = u32{ 64 };
		static constexpr auto vram_size = u32{ 0x800 };
		static constexpr auto oam_size = u32{ sprite_max_count * 4 };
		static constexpr auto palette_buffer_size = u32{ 0x20 };
		static constexpr auto tile_size = u32{ 8 };
		// Writes to some registers are ignored until this clock cycle.
		static constexpr auto boot_up_cycles = cycle_count::from_ppu(29658);

		enum class bitplane : u32 { _0, _1 };
		enum class palette : u32 { _0, _1, _2, _3 };
		enum class palette_color : u32 { _0, _1, _2, _3 };
		enum class name_table : u32 { _0, _1, _2, _3 };
		enum class role : u32 { background, foreground };
		enum class pattern_table : u32 { _0, _1 };
		enum class vram_increment : u32 { forward, downward };
		enum class sprite_size : u32 { single_height, double_height };
		enum class tile : u8 {};
		enum class color : u8 {};

#define BITFIELD_VALIDATE(mask, offset_from_right) \
	static_assert((mask & (1 << offset_from_right)) != 0); \
	static_assert(offset_from_right == 0 || (mask & (1 << (offset_from_right - 1))) == 0);
#define BITFIELD_VALUE(name, base, mask, offset_from_right) \
	BITFIELD_VALIDATE(mask, offset_from_right) \
	auto get_##name() const -> u32 { return (base & mask) >> offset_from_right; } \
	auto set_##name(u32 const v) -> void { base = (base & ~mask) | ((v << offset_from_right) & mask); }
#define BITFIELD_ENUM(name, type, base, mask, offset_from_right) \
	BITFIELD_VALIDATE(mask, offset_from_right) \
	auto get_##name() const -> type { return static_cast<type>((base & mask) >> offset_from_right); } \
	auto set_##name(type const v) -> void { base = (base & ~mask) | ((static_cast<u32>(v) << offset_from_right) & mask); }
#define BITFIELD_FLAG(name, base, mask, offset_from_right) \
	BITFIELD_VALIDATE(mask, offset_from_right) \
	auto get_##name() const -> bool { return base & mask; } \
	auto set_##name(u32 const v) -> void { base = (base & ~mask) | (v ? mask : 0); }

		struct color_index
		{
			BITFIELD_ENUM(color, palette_color, value, 0b00000011, 0)
			BITFIELD_ENUM(palette, palette, value, 0b00001100, 2)
			BITFIELD_ENUM(role, role, value, 0b00010000, 4)

			u8 value{};

			color_index() = default;

			explicit color_index(u8 const value)
				: value{ value }
			{
			}
		};

		struct sprite
		{
			// Tile index (and pattern table) for sprites
			BITFIELD_ENUM(large_pattern_table, pattern_table, value[1], 0b00000001, 0)
			BITFIELD_ENUM(large_top_tile, tile, value[1], 0b11111110, 1)
			BITFIELD_ENUM(small_tile, tile, value[1], 0b11111111, 0)

			// Attributes
			BITFIELD_ENUM(palette, palette, value[2], 0b00000011, 0)
			BITFIELD_FLAG(behind_background, value[2], 0b00100000, 5)
			BITFIELD_FLAG(flip_horizontal, value[2], 0b01000000, 6)
			BITFIELD_FLAG(flip_vertical, value[2], 0b10000000, 7)

			auto get_y() const -> u8 { return value[0]; }
			auto get_tile_index() const -> u8 { return value[1]; }
			auto get_attributes() const -> u8 { return value[2]; }
			auto get_x() const -> u8 { return value[3]; }

			auto set_y(u8 const v) -> void { value[0] = v; }
			auto set_tile_index(u8 const v) -> void { value[1] = v; }
			auto set_attributes(u8 const v) -> void { value[2] = v; }
			auto set_x(u8 const v) -> void { value[3] = v; }

			u8 value[4]{};

			explicit sprite() = default;

			explicit sprite(span<u8 const, 4> const data)
				: value{ data[0], data[1], data[2], data[3] }
			{
			}
		};

		struct tile_row
		{
			color_index colors[tile_size]{};
		};

		struct evaluated_sprite
		{
			tile_row pattern;
			u8 x{ 0 };
			bool is_in_front{ false };
			bool is_sprite_zero{ false };
		};

		cycle_count current_cycles_;
		cpu& cpu_;
		cartridge& cartridge_;
		display& display_;
		u8 vram_[vram_size]{};
		u8 oam_[oam_size]{};
		color palette_buffer_[palette_buffer_size]
		{
			color{ 0x09 }, color{ 0x01 }, color{ 0x00 }, color{ 0x01 },
			color{ 0x00 }, color{ 0x02 }, color{ 0x02 }, color{ 0x0D },
			color{ 0x08 }, color{ 0x10 }, color{ 0x08 }, color{ 0x24 },
			color{ 0x00 }, color{ 0x00 }, color{ 0x04 }, color{ 0x2C },
			color{ 0x09 }, color{ 0x01 }, color{ 0x34 }, color{ 0x03 },
			color{ 0x00 }, color{ 0x04 }, color{ 0x00 }, color{ 0x14 },
			color{ 0x08 }, color{ 0x3A }, color{ 0x00 }, color{ 0x02 },
			color{ 0x00 }, color{ 0x20 }, color{ 0x2C }, color{ 0x08 },
		};
		u8 latch_{}; // The last read/written IO register value, returned when reading write-only registers.
		struct
		{
			BITFIELD_FLAG(sprite_overflow, value, 0b00100000, 5)
			BITFIELD_FLAG(sprite_zero_hit, value, 0b01000000, 6)
			BITFIELD_FLAG(vblank, value, 0b10000000, 7)

			// Set the remaining (unused) bits based on the given value.
			auto set_remaining(u8 const v) -> void { value = (value & ~0b00011111) | (v & 0b00011111); }

			u8 value{ 0b00000000 };
		} status_{};
		struct
		{
			// Base nametable address.
			BITFIELD_ENUM(base_name_table, name_table, value, 0b00000011, 0)
			// How VRAM addresses should be incremented each read/write.
			BITFIELD_ENUM(vram_increment, vram_increment, value, 0b00000100, 2)
			// Sprite pattern table address for 8x8 sprites.
			BITFIELD_ENUM(sprite_pattern_table, pattern_table, value, 0b00001000, 3)
			// Background pattern table address.
			BITFIELD_ENUM(background_pattern_table, pattern_table, value, 0b00010000, 4)
			// The size of the sprites used (i.e. single-height or double-height).
			BITFIELD_ENUM(sprite_size, sprite_size, value, 0b00100000, 5)
			// PPU master/slave select.
			BITFIELD_FLAG(enable_ext_pin, value, 0b01000000, 6)
			// Enable/disable vblank NMI.
			BITFIELD_FLAG(vblank_nmi, value, 0b10000000, 7)

			u8 value{ 0b00000000 };
		} control_{};
		struct
		{
			// Enable/disable grayscale.
			BITFIELD_FLAG(grayscale, value, 0b00000001, 0)
			// Show background in leftmost 8 pixels of screen.
			BITFIELD_FLAG(show_background_start, value, 0b00000010, 1)
			// Show sprites in leftmost 8 pixels of screen.
			BITFIELD_FLAG(show_sprites_start, value, 0b00000100, 2)
			// Enable background rendering.
			BITFIELD_FLAG(enable_background, value, 0b00001000, 3)
			// Enable sprite rendering.
			BITFIELD_FLAG(enable_sprites, value, 0b00010000, 4)
			// Emphasize red.
			BITFIELD_FLAG(emphasize_red, value, 0b00100000, 5)
			// Emphasize green.
			BITFIELD_FLAG(emphasize_green, value, 0b01000000, 6)
			// Emphasize blue.
			BITFIELD_FLAG(emphasize_blue, value, 0b10000000, 7)

			u8 value{ 0b00000000 };
		} mask_{};
		struct
		{
			// See https://www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
			struct
			{
				// Scroll position
				BITFIELD_VALUE(coarse_x, value, 0b0000000000011111, 0)
				BITFIELD_VALUE(coarse_y, value, 0b0000001111100000, 5)
				BITFIELD_ENUM(name_table, name_table, value, 0b0000110000000000, 10)
				BITFIELD_VALUE(horizontal_name_table, value, 0b0000010000000000, 10)
				BITFIELD_VALUE(vertical_name_table, value, 0b0000100000000000, 11)
				BITFIELD_VALUE(tile_address, value, 0b0000111111111111, 0)
				BITFIELD_VALUE(fine_y, value, 0b0111000000000000, 12)

				// Address
				BITFIELD_VALUE(address_low, value, 0b0000000011111111, 0)
				BITFIELD_VALUE(address_high, value, 0b0111111100000000, 8)

				u16 value{};
			} v{}, t{}; // Current and temporary VRAM address and scroll position (15 bits)
			u8 x{}; // Fine X scroll (3 bits)
			bool w{}; // Second write toggle (1 bit)
		} internal_{};
		u8 oamaddr_{}; // Address accessed by IO registers
		u8 ppudata_read_buffer_{}; // Delays PPUDATA reads by one.
		u32 scanline_{ 240 };
		u32 scanline_cycle_{ 340 };
		bool even_frame_{ true };
		tile_row current_background_{};
		tile_row next_background_{};
		struct
		{
			tile tile{ 0 };
			palette palette{ 0 };
			u8 bitplane_0{ 0 };
			u8 bitplane_1{ 0 };
		} fetch_cycle_{}; // Data populated during the fetch cycle.
		evaluated_sprite sprites_[8]{}; // Evaluated sprites.
		u32 sprite_count_{ 0 }; // Number of evaluated sprites in sprites_.

#undef BITFIELD_VALIDATE
#undef BITFIELD_VALUE
#undef BITFIELD_ENUM
#undef BITFIELD_FLAG

	public:
		explicit ppu(cpu&, cartridge&, display&);

		ppu(ppu const&) = delete;
		ppu(ppu&&) = delete;
		auto operator=(ppu const&) -> ppu& = delete;
		auto operator=(ppu&&) -> ppu& = delete;

		auto get_cycles() const -> cycle_count { return current_cycles_; }
		auto build_snapshot(snapshot&) -> void;
		auto step() -> void;

		// Memory access

		auto read8(address) -> u8;
		auto write8(address, u8) -> void;

		// IO registers

		auto read_latch() -> u8;
		auto read_ppustatus() -> u8;
		auto read_oamdata() -> u8;
		auto read_ppudata() -> u8;

		auto write_latch(u8) -> void;
		auto write_ppuctrl(u8) -> void;
		auto write_ppuscroll(u8) -> void;
		auto write_ppumask(u8) -> void;
		auto write_ppuaddr(u8) -> void;
		auto write_ppudata(u8) -> void;
		auto write_oamaddr(u8) -> void;
		auto write_oamdata(u8) -> void;
		auto write_oamdma(u8) -> void;

	private:
		auto render_pixel() -> void;
		auto evaluate_sprites() -> void;
		auto fetch_sprite_pattern(sprite, u32 row) -> tile_row;

		// Helpers

		auto increment_vram() -> void;
		auto increment_x() -> void;
		auto increment_y() -> void;
		auto copy_x() -> void;
		auto copy_y() -> void;
		auto get_sprite_height() const -> u32;
		auto get_tile_bitplane(pattern_table, tile, u32 row, bitplane) -> u8;
		auto get_tile_row(palette, u8 bitplane_0, u8 bitplane_1) const -> tile_row;
		auto ref_color(color_index) -> color&;
		auto resolve_color(color) const -> rgb;
	};
} // namespace nes::sys