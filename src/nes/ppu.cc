#include "nes/ppu.hh"
#include "nes/cpu.hh"
#include "nes/mapper.hh"
#include "nes/util/address.hh"
#include "nes/util/rgb.hh"

namespace nes
{
	ppu::ppu(cpu& cpu, mapper& mapper)
		: cpu_{ cpu }
		, mapper_{ mapper }
	{
	}

	auto ppu::step_to(cycle_count const cycle) -> void
	{
		while (current_cycles_ < cycle)
		{
			current_cycles_ += cycle_count::from_ppu(1);
			step();
		}
	}

	auto ppu::step() -> void
	{
		// TODO
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
			auto const index = color_index{ addr.get_absolute() };
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
			auto const index = color_index{ addr.get_absolute() };
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

		status_.vblank = false;
		internal_.w = false;

		write_latch(res);
		return res;
	}

	auto ppu::read_oamdata() -> std::uint8_t
	{
		auto const res = oam_[oamaddr_];
		write_latch(res);
		return res;
	}

	auto ppu::read_ppudata() -> std::uint8_t
	{
		auto const addr = address{ internal_.v };
		auto res = read8(addr);

		if (addr <= address{ 0x3F00 })
		{
			std::swap(res, ppudata_read_buffer_);
		}
		else
		{
			// For palette buffers: buffer the mirrored nametable instead.
			ppudata_read_buffer_ = read8(addr - 0x1000);
		}

		write_latch(res);
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
			control_.value = value;
		}
	}

	auto ppu::write_ppuscroll(std::uint8_t const value) -> void
	{
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
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
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
			mask_.value = value;
		}
	}

	auto ppu::write_ppuaddr(std::uint8_t const value) -> void
	{
		write_latch(value);
		if (current_cycles_ > boot_up_cycles)
		{
			if (!internal_.w)
			{
				// First write.
				internal_.t = (internal_.t & 0b0000000011111111) | ((value & 0b00111111) << 8);
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
		write_latch(value);
		write8(address{ internal_.v }, value);
		increment_vram();
	}

	auto ppu::write_oamaddr(std::uint8_t const value) -> void
	{
		write_latch(value);
		oamaddr_ = value;
	}

	auto ppu::write_oamdata(std::uint8_t const value) -> void
	{
		write_latch(value);
		oam_[oamaddr_] = value;
		oamaddr_ += 1;
	}

	auto ppu::write_oamdma(std::uint8_t const value) -> void
	{
		// TODO: Do this in CPU::step instead in case there's an NMI?
		for (auto i = std::uint8_t{ 0x00 }; i < 0xff; ++i)
		{
			oam_[oamaddr_] = cpu_.read8(address{ value, i });
			oamaddr_ += 1;
		}

		auto const stalled_cycles = cycle_count::from_cpu((current_cycles_.to_ppu() % 2 == 0) ? 513 : 514);
		cpu_.stall_cycles(stalled_cycles);
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Helpers
	// -----------------------------------------------------------------------------------------------------------------

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

	auto ppu::resolve_color(color color) -> rgb
	{
		auto const index = static_cast<std::uint8_t>(color);
		switch (index & 0x3F)
		{
			case 0x00: return rgb::from_hex(0x666666);
			case 0x01: return rgb::from_hex(0x882A00);
			case 0x02: return rgb::from_hex(0xA71214);
			case 0x03: return rgb::from_hex(0xA4003B);
			case 0x04: return rgb::from_hex(0x7E005C);
			case 0x05: return rgb::from_hex(0x40006E);
			case 0x06: return rgb::from_hex(0x00066C);
			case 0x07: return rgb::from_hex(0x001D56);
			case 0x08: return rgb::from_hex(0x003533);
			case 0x09: return rgb::from_hex(0x00480B);
			case 0x0A: return rgb::from_hex(0x005200);
			case 0x0B: return rgb::from_hex(0x084F00);
			case 0x0C: return rgb::from_hex(0x4D4000);
			case 0x0D: return rgb::from_hex(0x000000);
			case 0x0E: return rgb::from_hex(0x000000);
			case 0x0F: return rgb::from_hex(0x000000);
			case 0x10: return rgb::from_hex(0xADADAD);
			case 0x11: return rgb::from_hex(0xD95F15);
			case 0x12: return rgb::from_hex(0xFF4042);
			case 0x13: return rgb::from_hex(0xFE2775);
			case 0x14: return rgb::from_hex(0xCC1AA0);
			case 0x15: return rgb::from_hex(0x7B1EB7);
			case 0x16: return rgb::from_hex(0x2031B5);
			case 0x17: return rgb::from_hex(0x004E99);
			case 0x18: return rgb::from_hex(0x006D6B);
			case 0x19: return rgb::from_hex(0x008738);
			case 0x1A: return rgb::from_hex(0x00930C);
			case 0x1B: return rgb::from_hex(0x328F00);
			case 0x1C: return rgb::from_hex(0x8D7C00);
			case 0x1D: return rgb::from_hex(0x000000);
			case 0x1E: return rgb::from_hex(0x000000);
			case 0x1F: return rgb::from_hex(0x000000);
			case 0x20: return rgb::from_hex(0xFFFEFF);
			case 0x21: return rgb::from_hex(0xFFB064);
			case 0x22: return rgb::from_hex(0xFF9092);
			case 0x23: return rgb::from_hex(0xFF76C6);
			case 0x24: return rgb::from_hex(0xFF6AF3);
			case 0x25: return rgb::from_hex(0xCC6EFE);
			case 0x26: return rgb::from_hex(0x7081FE);
			case 0x27: return rgb::from_hex(0x229EEA);
			case 0x28: return rgb::from_hex(0x00BEBC);
			case 0x29: return rgb::from_hex(0x00D888);
			case 0x2A: return rgb::from_hex(0x30E45C);
			case 0x2B: return rgb::from_hex(0x82E045);
			case 0x2C: return rgb::from_hex(0xDECD48);
			case 0x2D: return rgb::from_hex(0x4F4F4F);
			case 0x2E: return rgb::from_hex(0x000000);
			case 0x2F: return rgb::from_hex(0x000000);
			case 0x30: return rgb::from_hex(0xFFFEFF);
			case 0x31: return rgb::from_hex(0xFFDFC0);
			case 0x32: return rgb::from_hex(0xFFD2D3);
			case 0x33: return rgb::from_hex(0xFFC8E8);
			case 0x34: return rgb::from_hex(0xFFC2FB);
			case 0x35: return rgb::from_hex(0xEAC4FE);
			case 0x36: return rgb::from_hex(0xC5CCFE);
			case 0x37: return rgb::from_hex(0xA5D8F7);
			case 0x38: return rgb::from_hex(0x94E5E4);
			case 0x39: return rgb::from_hex(0x96EFCF);
			case 0x3A: return rgb::from_hex(0xABF4BD);
			case 0x3B: return rgb::from_hex(0xCCF3B3);
			case 0x3C: return rgb::from_hex(0xF2EBB5);
			case 0x3D: return rgb::from_hex(0xB8B8B8);
			case 0x3E: return rgb::from_hex(0x000000);
			case 0x3F: return rgb::from_hex(0x000000);
			default: return rgb::from_hex(0x000000);
		}
	}
} // namespace nes