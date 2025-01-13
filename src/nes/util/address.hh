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
		explicit address() = default;

		/// Create an address from its absolute value.
		explicit address(std::uint16_t const absolute)
			: absolute_{ absolute }
		{
		}

		/// Get the absolute address value.
		auto get_absolute() const -> std::uint16_t
		{
			return absolute_;
		}

		/// Get the address page (the first 8 bits).
		auto get_page() const -> std::uint8_t
		{
			return static_cast<std::uint8_t>((get_absolute() & 0xFF00) >> 8);
		}

		/// Get the offset within the page (the last 8 bits).
		auto get_offset() const -> std::uint8_t
		{
			return static_cast<std::uint8_t>((get_absolute() & 0x00FF) >> 0);
		}
	};
} // namespace nes