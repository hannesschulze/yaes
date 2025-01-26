#include "nes/ppu.hh"
#include "nes/cpu.hh"
#include "nes/cartridge.hh"
#include "nes/util/address.hh"
#include "nes/util/display.hh"
#include "nes/util/rgb.hh"
#include "nes/util/snapshot.hh"
#include "nes/util/debug.hh"

namespace nes
{
	ppu::ppu(cpu& cpu, cartridge& cartridge, display& display)
		: cpu_{ cpu }
		, cartridge_{ cartridge }
		, display_{ display }
	{
	}

	auto ppu::build_snapshot(snapshot& snapshot) -> void
	{
		snapshot.vram = std::vector(std::begin(vram_), std::end(vram_));
		snapshot.oam = std::vector(std::begin(oam_), std::end(oam_));
	}

	auto ppu::step() -> void
	{
		// See https://www.nesdev.org/wiki/PPU_rendering
		// Inspired by https://github.com/fogleman/nes/blob/master/nes/ppu.go

		auto const enable_rendering = mask_.get_enable_background() || mask_.get_enable_sprites();

		current_cycles_ += cycle_count::from_ppu(1);
		scanline_cycle_ += 1;
		if (enable_rendering && !even_frame_ && scanline_ == 261 && scanline_cycle_ == 340)
		{
			current_cycles_ += cycle_count::from_ppu(1);
			scanline_cycle_ += 1;
		}
		if (scanline_cycle_ == 341)
		{
			scanline_cycle_ = 0;
			scanline_ += 1;

			if (scanline_ == 262)
			{
				scanline_ = 0;
				even_frame_ = !even_frame_;
			}
		}

		auto const pre_line = scanline_ == 261;
		auto const visible_line = scanline_ < 240;
		auto const render_line = pre_line || visible_line;
		auto const pre_fetch_cycle = scanline_cycle_ >= 321 && scanline_cycle_ <= 336;
		auto const visible_cycle = scanline_cycle_ >= 1 && scanline_cycle_ <= 256;
		auto const fetch_cycle = pre_fetch_cycle || visible_cycle;

		// Background Logic
		if (enable_rendering)
		{
			if (visible_line && visible_cycle)
			{
				render_pixel();
			}

			if (render_line && fetch_cycle)
			{
				// Fetch cycle for the background.
				switch (scanline_cycle_ % 8)
				{
					case 1:
					{
						// Load the tile pattern for the background from the name table.
						fetch_cycle_.tile = tile{ read8(address{ 0x2000 } + internal_.v.get_tile_address()) };
						break;
					}
					case 3:
					{
						// Load the tile palette for the background from the attribute table.
						auto const addr =
							address{ 0x23C0 } +
							(static_cast<unsigned>(internal_.v.get_name_table()) * 0x400u) +
							((internal_.v.get_coarse_y() & 0b11100u) << 1) +
							((internal_.v.get_coarse_x() & 0b11100u) >> 2);
						auto const shift =
							((internal_.v.get_coarse_y() & 0b00010u) << 1) |
							((internal_.v.get_coarse_x() & 0b00010u) << 0);
						fetch_cycle_.palette = static_cast<palette>((read8(addr) >> shift) & 0b11);
						break;
					}
					case 5:
					{
						// Load bitplane 0 for the background tile's pattern.
						fetch_cycle_.bitplane_0 = get_tile_bitplane(
							control_.get_background_pattern_table(),
							fetch_cycle_.tile,
							internal_.v.get_fine_y(),
							bitplane::_0);
						break;
					}
					case 7:
					{
						// Load bitplane 1 for the background tile's pattern.
						fetch_cycle_.bitplane_1 = get_tile_bitplane(
							control_.get_background_pattern_table(),
							fetch_cycle_.tile,
							internal_.v.get_fine_y(),
							bitplane::_1);
						break;
					}
					case 0:
					{
						// Fetch cycle done -> build the tile row for the background.
						current_background_ = next_background_;
						next_background_ = get_tile_row(
							fetch_cycle_.palette, fetch_cycle_.bitplane_0, fetch_cycle_.bitplane_1);
						break;
					}
					default:
						break;
				}
			}

			if (pre_line && scanline_cycle_ >= 280 && scanline_cycle_ <= 304)
			{
				copy_y();
			}

			if (render_line)
			{
				if (fetch_cycle && scanline_cycle_ % 8 == 0)
				{
					increment_x();
				}
				if (scanline_cycle_ == 256)
				{
					increment_y();
				}
				if (scanline_cycle_ == 257)
				{
					copy_x();
				}
			}
		}

		// Sprite Logic
		if (enable_rendering)
		{
			if (scanline_cycle_ == 257)
			{
				if (visible_line)
				{
					evaluate_sprites();
				}
				else
				{
					sprite_count_ = 0;
				}
			}
		}

		// Vblank Logic
		if (scanline_ == 241 && scanline_cycle_ == 1)
		{
			display_.switch_buffers();
			status_.set_vblank(true);
			if (control_.get_vblank_nmi()) { cpu_.trigger_nmi(); }
		}
		if (pre_line && scanline_cycle_ == 1)
		{
			status_.set_vblank(false);
			status_.set_sprite_zero_hit(false);
			status_.set_sprite_overflow(false);
		}
	}

	auto ppu::render_pixel() -> void
	{
		auto const x = scanline_cycle_ - 1;
		auto const y = scanline_;

		auto background_color = color_index{ 0 };
		if (mask_.get_enable_background())
		{
			background_color = current_background_.colors[internal_.x + x % tile_size];
			background_color.set_role(role::background);
		}

		auto foreground = evaluated_sprite{};
		auto foreground_color = color_index{ 0 };
		if (mask_.get_enable_sprites())
		{
			for (auto i = unsigned{ 0 }; i < sprite_count_; ++i)
			{
				auto const s = sprites_[i];
				auto const offset = static_cast<int>(x) - static_cast<int>(s.x);
				if (offset < 0 || static_cast<unsigned>(offset) >= tile_size) { continue; }
				auto const color = s.pattern.colors[offset];
				if (color.get_color() == palette_color::_0) { continue; }

				foreground = s;
				foreground_color = color;
				foreground_color.set_role(role::foreground);
				break;
			}
		}

		auto has_background = background_color.get_color() != palette_color::_0;
		auto has_foreground = foreground_color.get_color() != palette_color::_0;

		if (x < tile_size && !mask_.get_show_background_start()) { has_background = false; }
		if (x < tile_size && !mask_.get_show_sprites_start()) { has_foreground = false; }

		auto color = color_index{ 0 };
		if (!has_background && has_foreground)
		{
			color = foreground_color;
		}
		if (has_background && !has_foreground)
		{
			color = background_color;
		}
		if (has_background && has_foreground)
		{
			if (foreground.is_sprite_zero && x < 255)
			{
				status_.set_sprite_zero_hit(true);
			}

			color = foreground.is_in_front ? foreground_color : background_color;
		}

		display_.set(x, y, resolve_color(ref_color(color)));
	}

	auto ppu::evaluate_sprites() -> void
	{
		auto const height = get_sprite_height();
		sprite_count_ = 0;
		for (auto i = unsigned{ 0 }; i < sprite_max_count; ++i)
		{
			auto const s = get_sprite(i);
			auto const row = static_cast<int>(scanline_) - static_cast<int>(s.y);
			if (row < 0 || static_cast<unsigned>(row) >= height) { continue; }

			if (sprite_count_ < 8)
			{
				sprites_[sprite_count_].pattern = fetch_sprite_pattern(s, static_cast<unsigned>(row));
				sprites_[sprite_count_].x = s.x;
				sprites_[sprite_count_].is_in_front = !s.get_behind_background();
				sprites_[sprite_count_].is_sprite_zero = i == 0;
				sprite_count_ += 1;
			}
			else
			{
				status_.set_sprite_overflow(true);
				break;
			}
		}
	}

	auto ppu::fetch_sprite_pattern(sprite const s, unsigned row) -> tile_row
	{
		if (s.get_flip_vertical()) { row = get_sprite_height() - 1 - row; }
		tile tile;
		pattern_table pattern_table;
		switch (control_.get_sprite_size())
		{
			case sprite_size::single_height:
				tile = s.get_small_tile();
				pattern_table = control_.get_sprite_pattern_table();
				break;
			case sprite_size::double_height:
				tile = s.get_large_top_tile();
				if (row >= tile_size)
				{
					tile = static_cast<ppu::tile>(1);
					row -= tile_size;
				}
				pattern_table = s.get_large_pattern_table();
				break;
			default:
				return tile_row{};
		}

		auto const bitplane_0 = get_tile_bitplane(pattern_table, tile, row, bitplane::_0);
		auto const bitplane_1 = get_tile_bitplane(pattern_table, tile, row, bitplane::_1);
		auto res = get_tile_row(s.get_palette(), bitplane_0, bitplane_1);

		if (s.get_flip_horizontal())
		{
			for (auto i = unsigned{ 0 }; i < tile_size / 2; ++i)
			{
				std::swap(res.colors[i], res.colors[tile_size - 1 - i]);
			}
		}

		return res;
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Memory Access
	// -----------------------------------------------------------------------------------------------------------------

	//
	// See: https://www.nesdev.org/wiki/PPU_memory_map
	//

	auto ppu::read8(address addr) -> std::uint8_t
	{
		addr = addr % 0x4000; // PPU only has 16 KiB addresses.
		if (addr <= address{ 0x3EFF }) { return cartridge_.get_mapper().read_ppu(addr, cartridge_, vram_); }
		if (addr <= address{ 0x3FFF })
		{
			auto const index = color_index{ static_cast<std::uint8_t>(addr.get_absolute() % 0x20) };
			auto const color = ref_color(index);
			return static_cast<std::uint8_t>(color);
		}

		return 0x0;
	}

	auto ppu::write8(address addr, std::uint8_t const value) -> void
	{
		addr = addr % 0x4000; // PPU only has 16 KiB addresses.
		if (addr <= address{ 0x3EFF })
		{
			cartridge_.get_mapper().write_ppu(addr, value, cartridge_, vram_);
			return;
		}
		if (addr <= address{ 0x3FFF })
		{
			auto const index = color_index{ static_cast<std::uint8_t>(addr.get_absolute() % 0x20) };
			ref_color(index) = color{ value };
			return;
		}
	}

	// -----------------------------------------------------------------------------------------------------------------
	// IO Registers
	// -----------------------------------------------------------------------------------------------------------------

	//
	// See: https://www.nesdev.org/wiki/PPU_registers
	//

	auto ppu::read_latch() -> std::uint8_t
	{
		return latch_;
	}

	auto ppu::read_ppustatus() -> std::uint8_t
	{
		auto res = status_;
		res.set_remaining(latch_);
		NES_DEBUG_LOG(ppu, "PPUSTATUS -> {:#2x}", res.value);

		status_.set_vblank(false);
		internal_.w = false;

		write_latch(res.value);
		return res.value;
	}

	auto ppu::read_oamdata() -> std::uint8_t
	{
		auto res = oam_[oamaddr_];
		if ((oamaddr_ & 0x3) == 0x2)
		{
			res &= 0xE3;
		}
		NES_DEBUG_LOG(ppu, "OAMDATA -> {:#2x}", res);
		write_latch(res);
		return res;
	}

	auto ppu::read_ppudata() -> std::uint8_t
	{
		auto const addr = address{ internal_.v.value };
		auto res = read8(addr);
		write_latch(ppudata_read_buffer_);

		if (addr % 0x4000 <= address{ 0x3EFF })
		{
			std::swap(res, ppudata_read_buffer_);
		}
		else
		{
			// For palette buffers: buffer the mirrored nametable instead.
			ppudata_read_buffer_ = read8(addr - 0x1000);
		}

		NES_DEBUG_LOG(ppu, "PPUDATA -> {:#2x} (address: {:#4x})", res, internal_.v);
		increment_vram();
		return res;
	}

	auto ppu::write_latch(std::uint8_t const value) -> void
	{
		latch_ = value;
	}

	auto ppu::write_ppuctrl(std::uint8_t const value) -> void
	{
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
			NES_DEBUG_LOG(ppu, "PPUCTRL <- {:#2x}", value);
			control_.value = value;
			internal_.t.set_name_table(control_.get_base_name_table());
			if (control_.get_vblank_nmi() && status_.get_vblank()) { cpu_.trigger_nmi(); }
		}
	}

	auto ppu::write_ppuscroll(std::uint8_t const value) -> void
	{
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
			NES_DEBUG_LOG(ppu, "PPUSCROLL <- {:#2x}", value);
			if (!internal_.w)
			{
				// First write -> x value.
				internal_.t.set_coarse_x((value & 0b11111000) >> 3);
				internal_.x = (value & 0b00000111) >> 0;
			}
			else
			{
				// Second write -> y value.
				internal_.t.set_coarse_y((value & 0b11111000) >> 3);
				internal_.t.set_fine_y((value & 0b00000111) >> 0);
			}

			internal_.w = !internal_.w;
		}
	}

	auto ppu::write_ppumask(std::uint8_t const value) -> void
	{
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
			NES_DEBUG_LOG(ppu, "PPUMASK <- {:#2x}", value);
			mask_.value = value;
		}
	}

	auto ppu::write_ppuaddr(std::uint8_t const value) -> void
	{
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
			NES_DEBUG_LOG(ppu, "PPUADDR <- {:#2x}", value);
			if (!internal_.w)
			{
				// First write.
				internal_.t.set_address_high(value & 0b00111111);
			}
			else
			{
				// Second write.
				internal_.t.set_address_low(value);
				internal_.v = internal_.t;
			}

			internal_.w = !internal_.w;
		}
	}

	auto ppu::write_ppudata(std::uint8_t const value) -> void
	{
		NES_DEBUG_LOG(ppu, "PPUDATA <- {:#2x} (address: {:#4x})", value, internal_.v);
		write_latch(value);
		write8(address{ internal_.v.value }, value);
		increment_vram();
	}

	auto ppu::write_oamaddr(std::uint8_t const value) -> void
	{
		NES_DEBUG_LOG(ppu, "OAMADDR <- {:#2x}", value);
		write_latch(value);
		oamaddr_ = value;
	}

	auto ppu::write_oamdata(std::uint8_t const value) -> void
	{
		NES_DEBUG_LOG(ppu, "OAMDATA <- {:#2x}", value);
		write_latch(value);
		oam_[oamaddr_] = value;
		oamaddr_ += 1;
	}

	auto ppu::write_oamdma(std::uint8_t const value) -> void
	{
		NES_DEBUG_LOG(ppu, "OAMDMA <- {:#2x}", value);
		auto addr = address{ value, 0x00 };
		for (auto i = unsigned{ 0 }; i < 256; ++i)
		{
			oam_[oamaddr_] = cpu_.read8(addr);
			oamaddr_ += 1;
			addr = addr + 1;
		}

		auto const stalled_cycles = cycle_count::from_cpu((cpu_.get_cycles().to_cpu() % 2 == 0) ? 513 : 514);
		cpu_.stall_cycles(stalled_cycles);
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Helpers
	// -----------------------------------------------------------------------------------------------------------------

	auto ppu::increment_vram() -> void
	{
		switch (control_.get_vram_increment())
		{
			case vram_increment::forward:
				internal_.v.value += 1;
				break;
			case vram_increment::downward:
				internal_.v.value += 32;
				break;
		}
	}

	auto ppu::increment_x() -> void
	{
		if (internal_.v.get_coarse_x() == 31)
		{
			internal_.v.set_coarse_x(0);
			internal_.v.set_horizontal_name_table(1 ^ internal_.v.get_horizontal_name_table());
		}
		else
		{
			internal_.v.set_coarse_x(internal_.v.get_coarse_x() + 1);
		}
	}

	auto ppu::increment_y() -> void
	{
		if (internal_.v.get_fine_y() == 7)
		{
			internal_.v.set_fine_y(0);
			if (internal_.v.get_coarse_y() == 29)
			{
				internal_.v.set_coarse_y(0);
				internal_.v.set_vertical_name_table(1 ^ internal_.v.get_vertical_name_table());
			}
			else if (internal_.v.get_coarse_y() == 31)
			{
				internal_.v.set_coarse_y(0);
				// Nametable not switched
			}
			else
			{
				internal_.v.set_coarse_y(internal_.v.get_coarse_y() + 1);
			}
		}
		else
		{
			internal_.v.set_fine_y(internal_.v.get_fine_y() + 1);
		}
	}

	auto ppu::copy_x() -> void
	{
		internal_.v.set_coarse_x(internal_.t.get_coarse_x());
		internal_.v.set_horizontal_name_table(internal_.t.get_horizontal_name_table());
	}

	auto ppu::copy_y() -> void
	{
		internal_.v.set_coarse_y(internal_.t.get_coarse_y());
		internal_.v.set_fine_y(internal_.t.get_fine_y());
		internal_.v.set_vertical_name_table(internal_.t.get_vertical_name_table());
	}

	auto ppu::get_sprite_height() const -> unsigned
	{
		switch (control_.get_sprite_size())
		{
			case sprite_size::single_height:
				return tile_size;
			case sprite_size::double_height:
				return tile_size * 2;
		}

		return tile_size;
	}

	auto ppu::get_sprite(unsigned const i) const -> sprite
	{
		return sprite{ oam_[i * 4 + 0], oam_[i * 4 + 1], oam_[i * 4 + 2], oam_[i * 4 + 3] };
	}

	auto ppu::get_tile_bitplane(
		pattern_table const pattern_table, tile const tile, unsigned const row, bitplane const bitplane) -> std::uint8_t
	{
		auto const addr = static_cast<std::uint16_t>(
			0x1000 * static_cast<unsigned>(pattern_table) + 0x10 * static_cast<unsigned>(tile) + row);
		return read8(address{ addr } + (static_cast<unsigned>(bitplane) * 8u));
	}

	auto ppu::get_tile_row(palette const palette, std::uint8_t bitplane_0, std::uint8_t bitplane_1) const -> tile_row
	{
		auto res = tile_row{};
		for (auto i = unsigned{ 0 }; i < tile_size; ++i)
		{
			auto const bit_0 = (bitplane_0 & 0b10000000u) >> 7;
			auto const bit_1 = (bitplane_1 & 0b10000000u) >> 7;
			bitplane_0 <<= 1;
			bitplane_1 <<= 1;
			res.colors[i].set_palette(palette);
			res.colors[i].set_color(palette_color{ (bit_1 << 1) | (bit_0 << 0) });
		}
		return res;
	}

	auto ppu::ref_color(color_index index) -> color&
	{
		// See https://www.nesdev.org/wiki/PPU_palettes
		index.value = index.value % 0x20;
		if (index.get_color() == palette_color::_0)
		{
			// The first color is mirrored between background and foreground palettes.
			index.set_role(role::background);
		}
		return palette_buffer_[index.value];
	}

	auto ppu::resolve_color(color const color) const -> rgb
	{
		auto const index = static_cast<std::uint8_t>(color);
		switch (index & 0x3F)
		{
			case 0x00: return rgb::from_hex(0x666666);
			case 0x01: return rgb::from_hex(0x002A88);
			case 0x02: return rgb::from_hex(0x1412A7);
			case 0x03: return rgb::from_hex(0x3B00A4);
			case 0x04: return rgb::from_hex(0x5C007E);
			case 0x05: return rgb::from_hex(0x6E0040);
			case 0x06: return rgb::from_hex(0x6C0600);
			case 0x07: return rgb::from_hex(0x561D00);
			case 0x08: return rgb::from_hex(0x333500);
			case 0x09: return rgb::from_hex(0x0B4800);
			case 0x0A: return rgb::from_hex(0x005200);
			case 0x0B: return rgb::from_hex(0x004F08);
			case 0x0C: return rgb::from_hex(0x00404D);
			case 0x0D: return rgb::from_hex(0x000000);
			case 0x0E: return rgb::from_hex(0x000000);
			case 0x0F: return rgb::from_hex(0x000000);
			case 0x10: return rgb::from_hex(0xADADAD);
			case 0x11: return rgb::from_hex(0x155FD9);
			case 0x12: return rgb::from_hex(0x4240FF);
			case 0x13: return rgb::from_hex(0x7527FE);
			case 0x14: return rgb::from_hex(0xA01ACC);
			case 0x15: return rgb::from_hex(0xB71E7B);
			case 0x16: return rgb::from_hex(0xB53120);
			case 0x17: return rgb::from_hex(0x994E00);
			case 0x18: return rgb::from_hex(0x6B6D00);
			case 0x19: return rgb::from_hex(0x388700);
			case 0x1A: return rgb::from_hex(0x0C9300);
			case 0x1B: return rgb::from_hex(0x008F32);
			case 0x1C: return rgb::from_hex(0x007C8D);
			case 0x1D: return rgb::from_hex(0x000000);
			case 0x1E: return rgb::from_hex(0x000000);
			case 0x1F: return rgb::from_hex(0x000000);
			case 0x20: return rgb::from_hex(0xFFFEFF);
			case 0x21: return rgb::from_hex(0x64B0FF);
			case 0x22: return rgb::from_hex(0x9290FF);
			case 0x23: return rgb::from_hex(0xC676FF);
			case 0x24: return rgb::from_hex(0xF36AFF);
			case 0x25: return rgb::from_hex(0xFE6ECC);
			case 0x26: return rgb::from_hex(0xFE8170);
			case 0x27: return rgb::from_hex(0xEA9E22);
			case 0x28: return rgb::from_hex(0xBCBE00);
			case 0x29: return rgb::from_hex(0x88D800);
			case 0x2A: return rgb::from_hex(0x5CE430);
			case 0x2B: return rgb::from_hex(0x45E082);
			case 0x2C: return rgb::from_hex(0x48CDDE);
			case 0x2D: return rgb::from_hex(0x4F4F4F);
			case 0x2E: return rgb::from_hex(0x000000);
			case 0x2F: return rgb::from_hex(0x000000);
			case 0x30: return rgb::from_hex(0xFFFEFF);
			case 0x31: return rgb::from_hex(0xC0DFFF);
			case 0x32: return rgb::from_hex(0xD3D2FF);
			case 0x33: return rgb::from_hex(0xE8C8FF);
			case 0x34: return rgb::from_hex(0xFBC2FF);
			case 0x35: return rgb::from_hex(0xFEC4EA);
			case 0x36: return rgb::from_hex(0xFECCC5);
			case 0x37: return rgb::from_hex(0xF7D8A5);
			case 0x38: return rgb::from_hex(0xE4E594);
			case 0x39: return rgb::from_hex(0xCFEF96);
			case 0x3A: return rgb::from_hex(0xBDF4AB);
			case 0x3B: return rgb::from_hex(0xB3F3CC);
			case 0x3C: return rgb::from_hex(0xB5EBF2);
			case 0x3D: return rgb::from_hex(0xB8B8B8);
			case 0x3E: return rgb::from_hex(0x000000);
			case 0x3F: return rgb::from_hex(0x000000);
			default: return rgb::from_hex(0x000000);
		}
	}
} // namespace nes