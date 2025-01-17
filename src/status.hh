#pragma once

#include "nes/util/cycle_count.hh"
#include <vector>
#include <cstdint>

namespace test
{
	/// System status used for comparison.
	struct status
	{
		explicit status() = default;

		nes::cycle_count cpu_cycle;
		std::vector<std::uint8_t> ram;
		std::vector<std::uint8_t> sram;
		struct
		{
			std::uint16_t pc{};
			std::uint8_t sp{};
			std::uint8_t a{};
			std::uint8_t x{};
			std::uint8_t y{};
			std::uint8_t p{};
			bool c{};
			bool z{};
			bool i{};
			bool d{};
			bool b{};
			bool v{};
			bool n{};
		} registers{};

		static auto equal(status const& a, status const& b) -> bool
		{
			if (a.cpu_cycle != b.cpu_cycle) { return false; }
			if (a.ram != b.ram) { return false; }
			if (a.sram != b.sram) { return false; }
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
	};
} // namespace test