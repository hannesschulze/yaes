#pragma once

#include "nes/common/types.hh"

namespace nes::sys
{
	/// An address on the NES (for simplicity, we share the same type for CPU and PPU, even though the PPU only uses
	/// 14 bit addresses).
	class address
	{
		u16 absolute_{ 0 };

	public:
		/// Create a zero address.
		constexpr explicit address() = default;

		/// Create an address from its absolute value.
		constexpr explicit address(u16 const absolute)
			: absolute_{ absolute }
		{
		}

		/// Create an address from its page and offset combination.
		constexpr explicit address(u8 const page, u8 const offset)
			: address{ static_cast<u16>((page << 8) | (offset << 0)) }
		{
		}

		/// Get the absolute address value.
		constexpr auto get_absolute() const -> u16
		{
			return absolute_;
		}

		/// Get the address page (the first 8 bits).
		constexpr auto get_page() const -> u8
		{
			return static_cast<u8>((get_absolute() & 0xFF00) >> 8);
		}

		/// Get the offset within the page (the last 8 bits).
		constexpr auto get_offset() const -> u8
		{
			return static_cast<u8>((get_absolute() & 0x00FF) >> 0);
		}
	};

	constexpr auto operator==(address const a, address const b) -> bool { return a.get_absolute() == b.get_absolute(); }
	constexpr auto operator!=(address const a, address const b) -> bool { return a.get_absolute() != b.get_absolute(); }
	constexpr auto operator<(address const a, address const b) -> bool { return a.get_absolute() < b.get_absolute(); }
	constexpr auto operator>(address const a, address const b) -> bool { return a.get_absolute() > b.get_absolute(); }
	constexpr auto operator<=(address const a, address const b) -> bool { return a.get_absolute() <= b.get_absolute(); }
	constexpr auto operator>=(address const a, address const b) -> bool { return a.get_absolute() >= b.get_absolute(); }

	constexpr auto operator+(address const a, u32 const b) -> address
	{
		return address{ static_cast<u16>(a.get_absolute() + b) };
	}

	constexpr auto operator+(u32 const a, address const b) -> address
	{
		return address{ static_cast<u16>(a + b.get_absolute()) };
	}

	constexpr auto operator-(address const a, u32 const b) -> address
	{
		return address{ static_cast<u16>(a.get_absolute() - b) };
	}

	constexpr auto operator%(address const a, u32 const b) -> address
	{
		return address{ static_cast<u16>(a.get_absolute() % b) };
	}
} // namespace nes::sys