#pragma once

#include "nes/util/cycle_count.hh"

namespace nes
{
	class address;
	class cpu;
	class mapper;
	class display;
	class rgb;

	class ppu
	{
		static constexpr auto vram_size = std::size_t{ 0x800 };
		static constexpr auto oam_size = std::size_t{ 0x100 };
		static constexpr auto palette_buffer_size = std::size_t{ 0x20 };
		static constexpr auto status_mask = std::uint8_t{ 0b11100000 };
		// Writes to some registers are ignored until this clock cycle.
		static constexpr auto boot_up_cycles = cycle_count::from_ppu(29658);

		enum class vram_increment
		{
			add_1_across = 0,
			add_32_down = 1,
		};

		enum class sprite_size
		{
			_8x8 = 0,
			_8x16 = 1,
		};

		enum class role
		{
			background = 0,
			sprite = 1,
		};

		enum class color : std::uint8_t
		{
		};

		union color_index
		{
			struct __attribute__((packed))
			{
				unsigned color   : 2;
				unsigned palette : 2;
				role     role    : 1;
			};
			unsigned value;

			explicit color_index(unsigned const value)
				: value{ value }
			{
			}
		};

		struct sprite
		{
			std::uint32_t pattern{ 0 };
			std::uint8_t position{ 0 };
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
		union
		{
			struct __attribute__((packed))
			{
				bool                 : 1;
				bool                 : 1;
				bool                 : 1;
				bool                 : 1;
				bool                 : 1;
				bool sprite_overflow : 1;
				bool sprite_zero_hit : 1;
				bool vblank          : 1;
			};
			std::uint8_t value{ 0b00000000 };
		} status_{};
		union
		{
			struct __attribute__((packed))
			{
				std::uint8_t   base_name_table          : 2; // Base nametable address.
				vram_increment vram_increment           : 1; // VRAM increment per CPU read/write of PPUDATA.
				std::uint8_t   sprite_pattern_table     : 1; // Sprite pattern table address for 8x8 sprites.
				std::uint8_t   background_pattern_table : 1; // Background pattern table address.
				sprite_size    sprite_size              : 1; // Sprite size.
				bool           enable_ext_pin           : 1; // PPU master/slave select.
				bool           vblank_nmi               : 1; // Enable/disable Vblank NMI
			};
			std::uint8_t value{ 0b00000000 };
		} control_{};
		union
		{
			struct __attribute__((packed))
			{
				bool grayscale             : 1; // Enable/disable grayscale.
				bool show_background_start : 1; // Show background in leftmost 8 pixels of screen.
				bool show_sprites_start    : 1; // Show sprites in leftmost 8 pixels of screen.
				bool enable_background     : 1; // Enable background rendering.
				bool enable_sprites        : 1; // Enable sprite rendering.
				bool emphasize_red         : 1; // Emphasize red.
				bool emphasize_green       : 1; // Emphasize green.
				bool emphasize_blue        : 1; // Emphasize blue.
			};
			std::uint8_t value{ 0b00000000 };
		} mask_{};
		struct
		{
			// See https://www.nesdev.org/wiki/PPU_scrolling#PPU_internal_registers
			std::uint16_t v{}; // Current VRAM address (15 bits)
			std::uint16_t t{}; // Temporary VRAM address (15 bits)
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
		sprite sprites_[8]{}; // Evaluated sprites.
		unsigned sprite_count_{ 0 }; // Number of evaluated sprites in sprites_.
		unsigned nmi_delay_{ 0 }; // Delay until an NMI is requested if conditions still met.
		bool nmi_requested_{ false };

	public:
		explicit ppu(cpu&, mapper&, display&);

		ppu(ppu const&) = delete;
		ppu(ppu&&) = delete;
		auto operator=(ppu const&) -> ppu& = delete;
		auto operator=(ppu&&) -> ppu& = delete;

		auto step_to(cycle_count) -> void;

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
		auto step() -> void;
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
		auto fetch_sprite_pattern(unsigned i, unsigned row) -> std::uint32_t;

		// Helpers

		auto nmi_change() -> void;
		auto sprite_height() -> unsigned;
		auto increment_vram() -> void;
		auto get_color(color_index) -> color&;
		auto resolve_color(color) -> rgb;
	};
} // namespace nes