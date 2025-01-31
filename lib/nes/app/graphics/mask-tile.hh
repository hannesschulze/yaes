#pragma once

#include <cstdint>

namespace nes::app
{
	/// An 8x8 tile without intrinsic coloring, only providing the alpha mask.
	///
	/// The tile is stored as an array of 8 8-bit integers, where each bit represents a horizontal pixel in the
	/// integer's row.
	struct mask_tile
	{
		std::uint8_t data[8]{};
	};

	namespace tiles
	{
		inline constexpr auto letter_a = mask_tile
		{
			0b00111000,
			0b01101100,
			0b11000110,
			0b11000110,
			0b11111110,
			0b11000110,
			0b11000110,
			0b00000000,
		};
	} // namespace tiles
} // namespace nes::app