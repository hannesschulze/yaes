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
} // namespace nes::app