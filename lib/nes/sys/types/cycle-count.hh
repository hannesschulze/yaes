#pragma once

#include "nes/common/types.hh"

namespace nes::sys
{
	/// A cycle count that can be used for multiple components.
	///
	/// Each component can have a different clock frequency and this structure offers conversions between different
	/// cycle counts.
	class cycle_count
	{
		friend constexpr auto operator+(cycle_count, cycle_count) -> cycle_count;
		friend constexpr auto operator-(cycle_count, cycle_count) -> cycle_count;
		friend constexpr auto operator*(u64, cycle_count) -> cycle_count;
		friend constexpr auto operator*(cycle_count, u64) -> cycle_count;
		friend constexpr auto operator/(cycle_count, cycle_count) -> u64;

		u64 units_{ 0 }; // The most fine-granular cycle count (here: PPU cycles).

		static constexpr auto ppu_cycle() -> cycle_count { return cycle_count{ 1 }; }
		static constexpr auto cpu_cycle() -> cycle_count { return cycle_count{ 3 }; }
		static constexpr auto second() -> cycle_count { return 1789773 * cpu_cycle(); }

	public:
		constexpr explicit cycle_count() = default;

		constexpr auto operator+=(cycle_count const other) -> cycle_count& { return *this = *this + other; }
		constexpr auto operator*=(u64 const other) -> cycle_count& { return *this = *this * other; }

		static constexpr auto from_units(u64 const units) -> cycle_count
		{
			return cycle_count{ units };
		}

		static constexpr auto from_microseconds(u64 const microseconds) -> cycle_count
		{
			return from_units(microseconds * second().get_units() / (1000 * 1000));
		}

		static constexpr auto from_cpu(u64 const cpu_cycles) -> cycle_count
		{
			return cpu_cycles * cpu_cycle();
		}

		static constexpr auto from_ppu(u64 const ppu_cycles) -> cycle_count
		{
			return ppu_cycles * ppu_cycle();
		}

		/// Value in implementation-defined units.
		constexpr auto get_units() const -> u64 { return units_; }

		constexpr auto to_microseconds() const -> u64
		{
			return get_units() * 1000 * 1000 / second().get_units();
		}

		constexpr auto to_cpu() const -> u64
		{
			return *this / cpu_cycle();
		}

		constexpr auto to_ppu() const -> u64
		{
			return *this / ppu_cycle();
		}

	private:
		constexpr explicit cycle_count(u64 const units)
			: units_{ units }
		{
		}
	};

	constexpr auto operator==(cycle_count const a, cycle_count const b) -> bool { return a.get_units() == b.get_units(); }
	constexpr auto operator!=(cycle_count const a, cycle_count const b) -> bool { return a.get_units() != b.get_units(); }
	constexpr auto operator<(cycle_count const a, cycle_count const b) -> bool { return a.get_units() < b.get_units(); }
	constexpr auto operator>(cycle_count const a, cycle_count const b) -> bool { return a.get_units() > b.get_units(); }
	constexpr auto operator<=(cycle_count const a, cycle_count const b) -> bool { return a.get_units() <= b.get_units(); }
	constexpr auto operator>=(cycle_count const a, cycle_count const b) -> bool { return a.get_units() >= b.get_units(); }
	constexpr auto operator+(cycle_count const a, cycle_count const b) -> cycle_count { return cycle_count{ a.get_units() + b.get_units() }; }
	constexpr auto operator-(cycle_count const a, cycle_count const b) -> cycle_count { return cycle_count{ a.get_units() - b.get_units() }; }
	constexpr auto operator*(u64 const a, cycle_count const b) -> cycle_count { return cycle_count{ a * b.get_units() }; }
	constexpr auto operator*(cycle_count const a, u64 const b) -> cycle_count { return cycle_count{ a.get_units() * b }; }
	constexpr auto operator/(cycle_count const a, cycle_count const b) -> u64 { return a.get_units() / b.get_units(); }
} // namespace nes::sys