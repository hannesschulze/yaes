#include "nes/ppu.hh"
#include "nes/display.hh"
#include "nes/cpu.hh"
#include "nes/mapper.hh"
#include "nes/util/address.hh"
#include "nes/util/rgb.hh"
#include <iostream>

// #define ENABLE_TRACES

namespace nes
{
	ppu::ppu(cpu& cpu, mapper& mapper, display& display)
		: cpu_{ cpu }
		, mapper_{ mapper }
		, display_{ display }
	{
	}

	auto ppu::snapshot(test::status& snapshot) -> void
	{
		snapshot.vram = std::vector(std::begin(vram_), std::end(vram_));
		snapshot.oam = std::vector(std::begin(oam_), std::end(oam_));
	}

	auto ppu::step() -> void
	{
		// See https://www.nesdev.org/wiki/PPU_rendering
		// Based on: https://github.com/fogleman/nes/blob/master/nes/ppu.go

		auto const enable_rendering = mask_.enable_background || mask_.enable_sprites;

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
				tile_data_ <<= 4;
				switch (scanline_cycle_ % 8)
				{
					case 1:
						fetch_name_table_byte();
						break;
					case 3:
						fetch_attribute_table_byte();
						break;
					case 5:
						fetch_low_tile_byte();
						break;
					case 7:
						fetch_high_tile_byte();
						break;
					case 0:
						store_tile_data();
						break;
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
			status_.vblank = true;
			nmi_change();
		}
		if (pre_line && scanline_cycle_ == 1)
		{
			status_.vblank = false;
			nmi_change();
			status_.sprite_zero_hit = false;
			status_.sprite_overflow = false;
		}
	}

	auto ppu::render_pixel() -> void
	{
		auto const x = scanline_cycle_ - 1;
		auto const y = scanline_;

		auto background_color = color_index{ 0 };
		if (mask_.enable_background)
		{
			auto const tile_data = static_cast<std::uint32_t>(tile_data_ >> 32);
			background_color = color_index{ static_cast<std::uint8_t>((tile_data >> ((7 - internal_.x) * 4)) & 0xF) };
			background_color.role = role::background;
		}

		auto foreground = sprite{};
		auto foreground_color = color_index{ 0 };
		if (mask_.enable_sprites)
		{
			for (auto i = unsigned{ 0 }; i < sprite_count_; ++i)
			{
				auto const s = sprites_[i];
				auto const offset = static_cast<int>(x) - static_cast<int>(s.position);
				if (offset < 0 || offset > 7) { continue; }
				auto const color = color_index{ static_cast<std::uint8_t>((s.pattern >> ((7 - offset) * 4)) & 0xF) };
				if (color.color == 0) { continue; }

				foreground = s;
				foreground_color = color;
				foreground_color.role = role::sprite;
				break;
			}
		}

		auto has_background = background_color.color != 0;
		auto has_foreground = foreground_color.color != 0;

		if (x < 8 && !mask_.show_background_start) { has_background = false; }
		if (x < 8 && !mask_.show_sprites_start) { has_foreground = false; }

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
				status_.sprite_zero_hit = true;
			}

			color = foreground.is_in_front ? foreground_color : background_color;
		}

		display_.set(x, y, resolve_color(get_color(color)));
	}

	auto ppu::fetch_name_table_byte() -> void
	{
		auto const addr = static_cast<std::uint16_t>(0x2000 | (internal_.v & 0x0FFF));
		name_table_byte_ = read8(address{ addr });
	}

	auto ppu::fetch_attribute_table_byte() -> void
	{
		auto const v = internal_.v;
		auto const addr = static_cast<std::uint16_t>(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
		auto const shift = ((v >> 4) & 4) | (v & 2);
		attribute_table_byte_ = static_cast<std::uint8_t>(((read8(address{ addr }) >> shift) & 3) << 2);
	}

	auto ppu::fetch_low_tile_byte() -> void
	{
		auto const fine_y = (internal_.v >> 12) & 7;
		auto const table = control_.background_pattern_table;
		auto const tile = name_table_byte_;
		auto const addr = static_cast<std::uint16_t>(0x1000 * table + 16 * tile + fine_y);
		low_tile_byte_ = read8(address{ addr });
	}

	auto ppu::fetch_high_tile_byte() -> void
	{
		auto const fine_y = (internal_.v >> 12) & 7;
		auto const table = control_.background_pattern_table;
		auto const tile = name_table_byte_;
		auto const addr = static_cast<std::uint16_t>(0x1000 * table + 16 * tile + fine_y);
		high_tile_byte_ = read8(address{ addr } + 8);
	}

	auto ppu::store_tile_data() -> void
	{
		auto data = std::uint32_t{ 0 };
		for (auto i = unsigned{ 0 }; i < 8; ++i)
		{
			auto const p1 = (low_tile_byte_ & 0x80) >> 7;
			auto const p2 = (high_tile_byte_ & 0x80) >> 6;
			low_tile_byte_ <<= 1;
			high_tile_byte_ <<= 1;
			data <<= 4;
			data |= attribute_table_byte_ | p1 | p2;
		}
		tile_data_ |= data;
	}

	auto ppu::increment_x() -> void
	{
		// If coarse X == 31
		if ((internal_.v & 0x1F) == 0x1F)
		{
			// Coarse X = 0
			// TODO: Check
			// internal_.v &= ~0x1F;
			   internal_.v &= 0xFFE0;
			// Switch horizontal nametable
			internal_.v ^= 0x400;
		}
		else
		{
			// Increment coarse X
			internal_.v += 1;
		}
	}

	auto ppu::increment_y() -> void
	{
		// If fine Y == 7
		if ((internal_.v & 0x7000) == 0x7000)
		{
			// Fine Y = 0
			internal_.v &= 0x8FFF;
			// Let y = coarse Y
			auto y = (internal_.v & 0x03E0) >> 5;
			if (y == 29)
			{
				// Coarse Y = 0
				y = 0;
				// Switch vertical nametable
				internal_.v ^= 0x0800;
			}
			else if (y == 31)
			{
				// Coarse Y = 0, nametable not switched
				y = 0;
			}
			else
			{
				// Increment coarse Y
				y += 1;
			}
			// Put coarse Y back into v
			// TODO: Check
			// internal_.v &= ~0x03E0;
			   internal_.v &= 0xFC1F;
			internal_.v |= y << 5;
		}
		else
		{
			// Increment fine Y
			internal_.v += 0x1000;
		}
	}

	auto ppu::copy_x() -> void
	{
		internal_.v = (internal_.v & 0xFBE0) | (internal_.t & 0x041F);
	}

	auto ppu::copy_y() -> void
	{
		internal_.v = (internal_.v & 0x841F) | (internal_.t & 0x7BE0);
	}

	auto ppu::evaluate_sprites() -> void
	{
		auto const height = sprite_height();
		sprite_count_ = 0;
		for (auto i = unsigned{ 0 }; i < 64; ++i)
		{
			auto const y = oam_[i * 4 + 0];
			auto const a = oam_[i * 4 + 2];
			auto const x = oam_[i * 4 + 3];
			auto const row = static_cast<int>(scanline_) - static_cast<int>(y);
			if (row < 0 || row >= height) { continue; }

			if (sprite_count_ < 8)
			{
				sprites_[sprite_count_].pattern = fetch_sprite_pattern(i, row);
				sprites_[sprite_count_].position = x;
				// TODO: Check
				// sprites_[sprite_count_].is_in_front = a & 0b00100000;
				   sprites_[sprite_count_].is_in_front = ((a >> 5) & 1) == 0;
				sprites_[sprite_count_].is_sprite_zero = i == 0;
				sprite_count_ += 1;
			}
			else
			{
				status_.sprite_overflow = true;
				break;
			}
		}
	}

	auto ppu::fetch_sprite_pattern(unsigned const i, unsigned row) -> std::uint32_t
	{
		auto tile = oam_[i * 4 + 1];
		auto const attributes = oam_[i * 4 + 2];
		auto const flip_vertical = (attributes & 0x80) == 0x80;
		if (flip_vertical) { row = sprite_height() - 1 - row; }
		auto addr = address{};
		switch (control_.sprite_size)
		{
			case sprite_size::_8x8:
			{
				auto const table = control_.sprite_pattern_table;
				addr = address{ static_cast<std::uint16_t>(0x1000 * table + 16 * tile + row) };
				break;
			}
			case sprite_size::_8x16:
			{
				auto const table = tile & 1;
				tile &= 0xFE;
				if (row > 7)
				{
					tile += 1;
					row -= 8;
				}
				addr = address{ static_cast<std::uint16_t>(0x1000 * table + 16 * tile + row) };
				break;
			}
		}

		auto const a = (attributes & 3) << 2;
		auto low_tile_byte = read8(addr);
		auto high_tile_byte = read8(addr + 8);
		auto data = std::uint32_t{ 0 };
		for (auto j = unsigned{ 0 }; j < 8; ++j)
		{
			auto const flip_horizontal = (attributes & 0x40) == 0x40;
			std::uint8_t p1, p2;
			if (flip_horizontal)
			{
				p1 = (low_tile_byte & 1) << 0;
				p2 = (high_tile_byte & 1) << 1;
				low_tile_byte >>= 1;
				high_tile_byte >>= 1;
			}
			else
			{
				p1 = (low_tile_byte & 0x80) >> 7;
				p2 = (high_tile_byte & 0x80) >> 6;
				low_tile_byte <<= 1;
				high_tile_byte <<= 1;
			}
			data <<= 4;
			data |= a | p1 | p2;
		}

		return data;
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
		if (addr <= address{ 0x3EFF }) { return mapper_.read_ppu(addr, vram_); }
		if (addr <= address{ 0x3FFF })
		{
			auto const index = color_index{ static_cast<std::uint8_t>(addr.get_absolute() % 0x20) };
			auto const color = get_color(index);
			return static_cast<std::uint8_t>(color);
		}

		return 0x0;
	}

	auto ppu::write8(address addr, std::uint8_t const value) -> void
	{
		addr = addr % 0x4000; // PPU only has 16 KiB addresses.
		if (addr <= address{ 0x3EFF })
		{
			mapper_.write_ppu(addr, vram_, value);
			return;
		}
		if (addr <= address{ 0x3FFF })
		{
			auto const index = color_index{ static_cast<std::uint8_t>(addr.get_absolute() % 0x20) };
			get_color(index) = color{ value };
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
		auto const base = latch_ & ~status_mask;
		auto const res = base | (status_.value & status_mask);
#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::read_ppustatus, res, current_cycles_);
#endif

		status_.vblank = false;
		internal_.w = false;
		nmi_change();

		write_latch(res);
		return res;
	}

	auto ppu::read_oamdata() -> std::uint8_t
	{
		auto res = oam_[oamaddr_];
		if ((oamaddr_ & 0x3) == 0x2)
		{
			res &= 0xE3;
		}
#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::read_oamdata, res, current_cycles_);
#endif
		write_latch(res);
		return res;
	}

	auto ppu::read_ppudata() -> std::uint8_t
	{
		auto const addr = address{ internal_.v };
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

#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::read_ppudata, res, current_cycles_);
#endif
		increment_vram();
		return res;
	}

	auto ppu::write_latch(std::uint8_t const value) -> void
	{
		latch_ = value;
	}

	auto ppu::write_ppuctrl(std::uint8_t const value) -> void
	{
		if (current_cycles_ > boot_up_cycles)
		{
			write_latch(value);
			control_.value = value;
			internal_.t = (internal_.t & 0xF3FF) | ((value & 0x03) << 10);
#ifdef ENABLE_TRACES
			memory_operations.emplace_back(test::memory_operation::type::write_ppuctrl, value, current_cycles_);
#endif
			nmi_change();
		}
	}

	auto ppu::write_ppuscroll(std::uint8_t const value) -> void
	{
		if (current_cycles_ > boot_up_cycles)
		{
			write_latch(value);
#ifdef ENABLE_TRACES
			memory_operations.emplace_back(test::memory_operation::type::write_ppuscroll, value, current_cycles_);
#endif
			if (!internal_.w)
			{
				// First write -> x value.
				internal_.t = (internal_.t & 0b1111111111100000) | (value >> 3);
				internal_.x = value & 0b00000111;
			}
			else
			{
				// Second write -> y value.
				internal_.t = (internal_.t & 0b1000111111111111) | ((value & 0b00000111) << 12);
				internal_.t = (internal_.t & 0b1111110000011111) | ((value & 0b11111000) << 2);
			}

			internal_.w = !internal_.w;
		}
	}

	auto ppu::write_ppumask(std::uint8_t const value) -> void
	{
		if (current_cycles_ > boot_up_cycles)
		{
			write_latch(value);
#ifdef ENABLE_TRACES
			memory_operations.emplace_back(test::memory_operation::type::write_ppumask, value, current_cycles_);
#endif
			mask_.value = value;
		}
	}

	auto ppu::write_ppuaddr(std::uint8_t const value) -> void
	{
		if (current_cycles_ > boot_up_cycles)
		{
			write_latch(value);
#ifdef ENABLE_TRACES
			memory_operations.emplace_back(test::memory_operation::type::write_ppuaddr, value, current_cycles_);
#endif
			if (!internal_.w)
			{
				// First write.
				internal_.t = (internal_.t & 0b1000000011111111) | ((value & 0b00111111) << 8);
			}
			else
			{
				// Second write.
				internal_.t = (internal_.t & 0b1111111100000000) | ((value & 0b11111111) << 0);
				internal_.v = internal_.t;
			}

			internal_.w = !internal_.w;
		}
	}

	auto ppu::write_ppudata(std::uint8_t const value) -> void
	{
#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::write_ppudata, value, current_cycles_);
#endif
		write_latch(value);
		write8(address{ internal_.v }, value);
		increment_vram();
	}

	auto ppu::write_oamaddr(std::uint8_t const value) -> void
	{
#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::write_oamaddr, value, current_cycles_);
#endif
		write_latch(value);
		oamaddr_ = value;
	}

	auto ppu::write_oamdata(std::uint8_t const value) -> void
	{
#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::write_oamdata, value, current_cycles_);
#endif
		write_latch(value);
		oam_[oamaddr_] = value;
		oamaddr_ += 1;
	}

	auto ppu::write_oamdma(std::uint8_t const value) -> void
	{
		// TODO: Do this in CPU::step instead in case there's an NMI?
#ifdef ENABLE_TRACES
		memory_operations.emplace_back(test::memory_operation::type::write_oamdma, value, current_cycles_);
#endif
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

	auto ppu::nmi_change() -> void
	{
		auto const should_trigger = control_.vblank_nmi && status_.vblank;
		if (should_trigger)
		{
			cpu_.trigger_nmi();
		}
	}

	auto ppu::sprite_height() -> unsigned
	{
		switch (control_.sprite_size)
		{
			case sprite_size::_8x8:
				return 8;
			case sprite_size::_8x16:
				return 16;
		}

		return 8;
	}

	auto ppu::increment_vram() -> void
	{
		switch (control_.vram_increment)
		{
			case vram_increment::add_1_across:
				internal_.v += 1;
				break;
			case vram_increment::add_32_down:
				internal_.v += 32;
				break;
		}
	}

	auto ppu::get_color(color_index index) -> color&
	{
		// See https://www.nesdev.org/wiki/PPU_palettes
		index.value = index.value % 0x20;
		if (index.color == 0)
		{
			// The first color is mirrored between background and foreground palettes.
			index.role = role::background;
		}
		return palette_buffer_[index.value];
	}

	auto ppu::resolve_color(color const color) -> rgb
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