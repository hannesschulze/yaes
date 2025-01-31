#pragma once

#include "nes/app/graphics/color.hh"

namespace nes::app
{
	/// An 8x8 tile with color data.
	///
	/// The tile is stored as a flattened two-dimensional array of 8x8 colors, stored in row-major format.
	struct image_tile
	{
		color data[8 * 8]{};
	};

	namespace tiles
	{
		inline constexpr auto test = image_tile
		{
			color{ 0 }, color{ 0 }, color{ 9 }, color{ 9 }, color{ 9 }, color{ 9 }, color{ 0 }, color{ 0 },
			color{ 0 }, color{ 9 }, color{ 9 }, color{ 8 }, color{ 8 }, color{ 9 }, color{ 9 }, color{ 0 },
			color{ 9 }, color{ 9 }, color{ 8 }, color{ 8 }, color{ 8 }, color{ 8 }, color{ 9 }, color{ 9 },
			color{ 9 }, color{ 8 }, color{ 8 }, color{ 9 }, color{ 9 }, color{ 8 }, color{ 8 }, color{ 9 },
			color{ 9 }, color{ 8 }, color{ 8 }, color{ 9 }, color{ 9 }, color{ 8 }, color{ 8 }, color{ 9 },
			color{ 9 }, color{ 9 }, color{ 8 }, color{ 8 }, color{ 8 }, color{ 8 }, color{ 9 }, color{ 9 },
			color{ 0 }, color{ 9 }, color{ 9 }, color{ 8 }, color{ 8 }, color{ 9 }, color{ 9 }, color{ 0 },
			color{ 0 }, color{ 0 }, color{ 9 }, color{ 9 }, color{ 9 }, color{ 9 }, color{ 0 }, color{ 0 },
		};
	} // namespace tiles
} // namespace nes::app