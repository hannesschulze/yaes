#pragma once

#include "nes/util/cycle-count.hh"

namespace nes
{
	class address;
	class cpu;
	class mapper;
	class display;
	class rgb;
	struct snapshot;

	class ppu
	{
		static constexpr auto vram_size = std::size_t{ 0x800 };
		static constexpr auto oam_size = std::size_t{ 0x100 };
		static constexpr auto palette_buffer_size = std::size_t{ 0x20 };
		static constexpr auto status_mask = std::uint8_t{ 0b11100000 };
		// Writes to some registers are ignored until this clock cycle.
		static constexpr auto boot_up_cycles = cycle_count::from_ppu(29658);

		enum class color : std::uint8_t
		{
		};

		struct color_index
		{
			auto get_color() const -> unsigned { return (value & 0b00000011) >> 0; }
			auto get_palette() const -> unsigned { return (value & 0b00001100) >> 2; }
			auto get_foreground() const -> bool { return value & 0b00010000; }

			auto set_color(unsigned const v) -> void { value = (value & ~0b00000011) | ((v << 0) & 0b00000011); }
			auto set_palette(unsigned const v) -> void { value = (value & ~0b00001100) | ((v << 2) & 0b00001100); }
			auto set_foreground(bool const v) -> void { value = (value & ~0b00010000) | (v ? 0b00010000 : 0); }

			std::uint8_t value{};

			explicit color_index(std::uint8_t const value)
				: value{ value }
			{
			}
		};

		struct sprite
		{
			// Tile index and pattern table for large sprites
			auto get_pattern_table() const -> unsigned { return (tile_index & 0b00000001) >> 0; }
			auto get_top_tile() const -> unsigned { return (tile_index & 0b11111110) >> 1; }

			auto set_pattern_table(unsigned const v) -> void { tile_index = (tile_index & ~0b00000001) | ((v << 0) & 0b00000001); }
			auto set_top_tile(unsigned const v) -> void { tile_index = (tile_index & ~0b11111110) | ((v << 1) & 0b11111110); }

			// Attributes
			auto get_palette() const -> unsigned { return (attributes & 0b00000011) >> 0; }
			auto get_behind_background() const -> bool { return attributes & 0b00100000; }
			auto get_flip_horizontal() const -> bool { return attributes & 0b01000000; }
			auto get_flip_vertical() const -> bool { return attributes & 0b10000000; }

			auto set_palette(unsigned const v) -> void { attributes = (attributes & ~0b00000011) | ((v << 0) & 0b00000011); }
			auto set_behind_background(bool const v) -> void { attributes = (attributes & ~0b00100000) | (v ? 0b00100000 : 0); }
			auto set_flip_horizontal(bool const v) -> void { attributes = (attributes & ~0b01000000) | (v ? 0b01000000 : 0); }
			auto set_flip_vertical(bool const v) -> void { attributes = (attributes & ~0b10000000) | (v ? 0b10000000 : 0); }

			std::uint8_t y{};
			std::uint8_t tile_index{};
			std::uint8_t attributes{};
			std::uint8_t x{};

			explicit sprite(std::uint8_t const y, std::uint8_t const tile_index, std::uint8_t const attributes, std::uint8_t const x)
				: y{ y }
				, tile_index{ tile_index }
				, attributes{ attributes }
				, x{ x }
			{
			}
		};

		struct evaluated_sprite
		{
			std::uint32_t pattern{ 0 };
			std::uint8_t x{ 0 };
			bool is_in_front{ false };
			bool is_sprite_zero{ false };
		};

		cycle_count current_cycles_;
		cpu& cpu_;
		mapper& mapper_;
		display& display_;
		std::uint8_t vram_[vram_size]{};
		std::uint8_t oam_[oam_size]{};
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
		std::uint8_t latch_{}; // The last read/written IO register value, returned when reading write-only registers.
		struct
		{
			auto get_sprite_overflow() const -> bool { return value & 0b00100000; }
			auto get_sprite_zero_hit() const -> bool { return value & 0b01000000; }
			auto get_vblank() const -> bool { return value & 0b10000000; }

			auto set_sprite_overflow(bool const v) -> void { value = (value & ~0b00100000) | (v ? 0b00100000 : 0); }
			auto set_sprite_zero_hit(bool const v) -> void { value = (value & ~0b01000000) | (v ? 0b01000000 : 0); }
			auto set_vblank(bool const v) -> void { value = (value & ~0b10000000) | (v ? 0b10000000 : 0); }

			std::uint8_t value{ 0b00000000 };
		} status_{};
		struct
		{
			// Base nametable address.
			auto get_base_name_table() const -> unsigned { return (value & 0b00000011u) >> 0; }
			// Increment VRAM addresses downward (+ 32) instead of moving to the right (+ 1) on each read/write.
			auto get_vram_row_increment() const -> bool { return value & 0b00000100; }
			// Sprite pattern table address for 8x8 sprites.
			auto get_sprite_pattern_table() const -> unsigned { return (value & 0b00001000u) >> 3; }
			// Background pattern table address.
			auto get_background_pattern_table() const -> unsigned { return (value & 0b00010000u) >> 4; }
			// Whether to use double-height (8x16) instead of 8x8 sprites.
			auto get_large_sprites() const -> bool { return value & 0b00100000; }
			// PPU master/slave select.
			auto get_enable_ext_pin() const -> bool { return value & 0b01000000; }
			// Enable/disable vblank NMI.
			auto get_vblank_nmi() const -> bool { return value & 0b10000000; }

			auto set_base_name_table(unsigned const v) -> void { value = (value & ~0b00000011) | ((v << 0) & 0b00000011); }
			auto set_vram_row_increment(bool const v) -> void { value = (value & ~0b00000100) | (v ? 0b00000100 : 0); }
			auto set_sprite_pattern_table(unsigned const v) -> void { value = (value & ~0b00001000) | ((v << 3) & 0b00001000); }
			auto set_background_pattern_table(unsigned const v) -> void { value = (value & ~0b00010000) | ((v << 4) & 0b00010000); }
			auto set_large_sprites(bool const v) -> void { value = (value & ~0b00100000) | (v ? 0b00100000 : 0); }
			auto set_enable_ext_pin(bool const v) -> void { value = (value & ~0b01000000) | (v ? 0b01000000 : 0); }
			auto set_vblank_nmi(bool const v) -> void { value = (value & ~0b10000000) | (v ? 0b10000000 : 0); }

			std::uint8_t value{ 0b00000000 };
		} control_{};
		struct
		{
			// Enable/disable grayscale.
			auto get_grayscale() const -> bool { return value & 0b00000001; }
			// Show background in leftmost 8 pixels of screen.
			auto get_show_background_start() const -> bool { return value & 0b00000010; }
			// Show sprites in leftmost 8 pixels of screen.
			auto get_show_sprites_start() const -> bool { return value & 0b00000100; }
			// Enable background rendering.
			auto get_enable_background() const -> bool { return value & 0b00001000; }
			// Enable sprite rendering.
			auto get_enable_sprites() const -> bool { return value & 0b00010000; }
			// Emphasize red.
			auto get_emphasize_red() const -> bool { return value & 0b00100000; }
			// Emphasize green.
			auto get_emphasize_green() const -> bool { return value & 0b01000000; }
			// Emphasize blue.
			auto get_emphasize_blue() const -> bool { return value & 0b10000000; }

			auto set_grayscale(bool const v) -> void { value = (value & ~0b00000001) | (v ? 0b00000001 : 0); }
			auto set_show_background_start(bool const v) -> void { value = (value & ~0b00000010) | (v ? 0b00000010 : 0); }
			auto set_show_sprites_start(bool const v) -> void { value = (value & ~0b00000100) | (v ? 0b00000100 : 0); }
			auto set_enable_background(bool const v) -> void { value = (value & ~0b00001000) | (v ? 0b00001000 : 0); }
			auto set_enable_sprites(bool const v) -> void { value = (value & ~0b00010000) | (v ? 0b00010000 : 0); }
			auto set_emphasize_red(bool const v) -> void { value = (value & ~0b00100000) | (v ? 0b00100000 : 0); }
			auto set_emphasize_green(bool const v) -> void { value = (value & ~0b01000000) | (v ? 0b01000000 : 0); }
			auto set_emphasize_blue(bool const v) -> void { value = (value & ~0b10000000) | (v ? 0b10000000 : 0); }

			std::uint8_t value{ 0b00000000 };
		} mask_{};
		struct
		{
			// See https://www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
			struct
			{
				// Scroll position
				auto get_coarse_x() const -> unsigned { return (value & 0b0000000000011111) >> 0; }
				auto get_coarse_y() const -> unsigned { return (value & 0b0000001111100000) >> 5; }
				auto get_horizontal_name_table() const -> unsigned { return (value & 0b0000010000000000) >> 10; }
				auto get_vertical_name_table() const -> unsigned { return (value & 0b0000100000000000) >> 11; }
				auto get_fine_y() const -> unsigned { return (value & 0b0111000000000000) >> 12; }

				auto set_coarse_x(unsigned const v) -> void { value = (value & ~0b0000000000011111) | ((v << 0) & 0b0000000000011111); }
				auto set_coarse_y(unsigned const v) -> void { value = (value & ~0b0000001111100000) | ((v << 5) & 0b0000001111100000); }
				auto set_horizontal_name_table(unsigned const v) -> void { value = (value & ~0b0000010000000000) | ((v << 10) & 0b0000010000000000); }
				auto set_vertical_name_table(unsigned const v) -> void { value = (value & ~0b0000100000000000) | ((v << 11) & 0b0000100000000000); }
				auto set_fine_y(unsigned const v) -> void { value = (value & ~0b0111000000000000) | ((v << 12) & 0b0111000000000000); }

				// Address
				auto get_address_low() const -> unsigned { return (value & 0b0000000011111111) >> 0; }
				auto get_address_high() const -> unsigned { return (value & 0b0111111100000000) >> 8; }

				auto set_address_low(unsigned const v) -> void { value = (value & ~0b0000000011111111) | ((v << 0) & 0b0000000011111111); }
				auto set_address_high(unsigned const v) -> void { value = (value & ~0b0111111100000000) | ((v << 8) & 0b0111111100000000); }

				std::uint16_t value{};
			} v{}, t{}; // Current and temporary VRAM address and scroll position (15 bits)
			std::uint8_t  x{}; // Fine X scroll (3 bits)
			bool          w{}; // Second write toggle (1 bit)
		} internal_{};
		std::uint8_t oamaddr_{}; // Address accessed by IO registers
		std::uint8_t ppudata_read_buffer_{}; // Delays PPUDATA reads by one.
		unsigned scanline_{ 240 };
		unsigned scanline_cycle_{ 340 };
		bool even_frame_{ true };
		std::uint64_t tile_data_{ 0 }; // Data for the current tile (higher 32 bits) and the next tile (lower 32 bits).
		std::uint8_t name_table_byte_{ 0 };
		std::uint8_t attribute_table_byte_{ 0 };
		std::uint8_t low_tile_byte_{ 0 };
		std::uint8_t high_tile_byte_{ 0 };
		evaluated_sprite sprites_[8]{}; // Evaluated sprites.
		unsigned sprite_count_{ 0 }; // Number of evaluated sprites in sprites_.

	public:
		explicit ppu(cpu&, mapper&, display&);

		ppu(ppu const&) = delete;
		ppu(ppu&&) = delete;
		auto operator=(ppu const&) -> ppu& = delete;
		auto operator=(ppu&&) -> ppu& = delete;

		auto get_cycles() const -> cycle_count { return current_cycles_; }
		auto build_snapshot(snapshot&) -> void;
		auto step() -> void;

		// Memory access

		auto read8(address) -> std::uint8_t;
		auto write8(address, std::uint8_t) -> void;

		// IO registers

		auto read_latch() -> std::uint8_t;
		auto read_ppustatus() -> std::uint8_t;
		auto read_oamdata() -> std::uint8_t;
		auto read_ppudata() -> std::uint8_t;

		auto write_latch(std::uint8_t) -> void;
		auto write_ppuctrl(std::uint8_t) -> void;
		auto write_ppuscroll(std::uint8_t) -> void;
		auto write_ppumask(std::uint8_t) -> void;
		auto write_ppuaddr(std::uint8_t) -> void;
		auto write_ppudata(std::uint8_t) -> void;
		auto write_oamaddr(std::uint8_t) -> void;
		auto write_oamdata(std::uint8_t) -> void;
		auto write_oamdma(std::uint8_t) -> void;

	private:
		auto render_pixel() -> void;
		auto fetch_name_table_byte() -> void;
		auto fetch_attribute_table_byte() -> void;
		auto fetch_low_tile_byte() -> void;
		auto fetch_high_tile_byte() -> void;
		auto store_tile_data() -> void;
		auto increment_x() -> void;
		auto increment_y() -> void;
		auto copy_x() -> void;
		auto copy_y() -> void;
		auto evaluate_sprites() -> void;
		auto fetch_sprite_pattern(sprite, unsigned row) -> std::uint32_t;

		// Helpers

		auto sprite_height() const -> unsigned;
		auto increment_vram() -> void;
		auto get_sprite(unsigned) const -> sprite;
		auto ref_color(color_index) -> color&;
		auto resolve_color(color) const -> rgb;
	};
} // namespace nes