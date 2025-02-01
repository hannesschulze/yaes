#pragma once

#include "nes/sys/types/cycle-count.hh"
#include "nes/common/types.hh"
#include <vector>

namespace nes::sys
{
	/// System snapshot used for comparison with a reference implementation.
	struct snapshot
	{
		explicit snapshot() = default;

		cycle_count cpu_cycle;
		cycle_count ppu_cycle;
		std::vector<u8> ram;
		std::vector<u8> sram;
		std::vector<u8> vram;
		std::vector<u8> oam;
		struct
		{
			u16 pc{};
			u8 sp{};
			u8 a{};
			u8 x{};
			u8 y{};
			u8 p{};
			bool c{};
			bool z{};
			bool i{};
			bool d{};
			bool b{};
			bool v{};
			bool n{};
		} registers{};
	};

	inline auto operator==(snapshot const& a, snapshot const& b) -> bool
	{
		if (a.cpu_cycle != b.cpu_cycle) { return false; }
		if (a.ppu_cycle != b.ppu_cycle) { return false; }
		if (a.ram != b.ram) { return false; }
		if (a.sram != b.sram) { return false; }
		if (a.vram != b.vram) { return false; }
		if (a.oam != b.oam) { return false; }
		if (a.registers.pc != b.registers.pc) { return false; }
		if (a.registers.sp != b.registers.sp) { return false; }
		if (a.registers.a != b.registers.a) { return false; }
		if (a.registers.x != b.registers.x) { return false; }
		if (a.registers.y != b.registers.y) { return false; }
		if (a.registers.p != b.registers.p) { return false; }
		if (a.registers.c != b.registers.c) { return false; }
		if (a.registers.z != b.registers.z) { return false; }
		if (a.registers.i != b.registers.i) { return false; }
		if (a.registers.d != b.registers.d) { return false; }
		if (a.registers.b != b.registers.b) { return false; }
		if (a.registers.v != b.registers.v) { return false; }
		if (a.registers.n != b.registers.n) { return false; }
		return true;
	}

	inline auto operator!=(snapshot const& a, snapshot const& b) -> bool { return !(a == b); }
} // namespace nes::sys