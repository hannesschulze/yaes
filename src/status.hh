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
		std::vector<std::uint8_t> vram;
		std::vector<std::uint8_t> oam;
		std::vector<std::uint8_t> chrrom;
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

	struct memory_operation
	{
		enum class type
		{
			read_ppustatus,
			read_oamdata,
			read_ppudata,
			write_ppuctrl,
			write_ppuscroll,
			write_ppumask,
			write_ppuaddr,
			write_ppudata,
			write_oamaddr,
			write_oamdata,
			write_oamdma,
		};

		explicit constexpr memory_operation(type const t, std::uint8_t const v, nes::cycle_count cycles)
			: t{ t }
			, v{ v }
			, cycles{ cycles }
		{
		}

		type t{};
		std::uint8_t v{};
		nes::cycle_count cycles;
	};

	constexpr auto operator==(memory_operation const a, memory_operation const b) -> bool
	{
		return a.t == b.t && a.v == b.v;
	}

	constexpr auto operator!=(memory_operation const a, memory_operation const b) -> bool
	{
		return a.t != b.t || a.v != b.v;
	}
} // namespace test