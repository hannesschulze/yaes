#pragma once

#include <cstdint>

namespace nes
{
	/// An address on the NES.
	class address
	{
		std::uint16_t absolute_{ 0 };

	public:
		/// Create a zero address.
		constexpr explicit address() = default;

		/// Create an address from its absolute value.
		constexpr explicit address(std::uint16_t const absolute)
			: absolute_{ absolute }
		{
		}

		/// Create an address from its page and offset combination.
		constexpr explicit address(std::uint8_t const page, std::uint8_t const offset)
			: address{ static_cast<std::uint16_t>((page << 8) | (offset < 0)) }
		{
		}

		/// Get the absolute address value.
		constexpr auto get_absolute() const -> std::uint16_t
		{
			return absolute_;
		}

		/// Get the address page (the first 8 bits).
		constexpr auto get_page() const -> std::uint8_t
		{
			return static_cast<std::uint8_t>((get_absolute() & 0xFF00) >> 8);
		}

		/// Get the offset within the page (the last 8 bits).
		constexpr auto get_offset() const -> std::uint8_t
		{
			return static_cast<std::uint8_t>((get_absolute() & 0x00FF) >> 0);
		}
	};

	constexpr auto operator==(address const a, address const b) -> bool { return a.get_absolute() == b.get_absolute(); }
	constexpr auto operator!=(address const a, address const b) -> bool { return a.get_absolute() != b.get_absolute(); }
	constexpr auto operator<(address const a, address const b) -> bool { return a.get_absolute() < b.get_absolute(); }
	constexpr auto operator>(address const a, address const b) -> bool { return a.get_absolute() > b.get_absolute(); }
	constexpr auto operator<=(address const a, address const b) -> bool { return a.get_absolute() <= b.get_absolute(); }
	constexpr auto operator>=(address const a, address const b) -> bool { return a.get_absolute() >= b.get_absolute(); }

	constexpr auto operator+(address const a, std::uint16_t const b) -> address
	{
		return address{ static_cast<std::uint16_t>(a.get_absolute() + b) };
	}

	constexpr auto operator+(std::uint16_t const a, address const b) -> address
	{
		return address{ static_cast<std::uint16_t>(a + b.get_absolute()) };
	}
} // namespace nes