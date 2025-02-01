#pragma once

#include "nes/common/types.hh"

namespace nes::app
{
	/// An 8x8 tile without intrinsic coloring, only providing the alpha mask.
	///
	/// The tile is stored as an array of 8 8-bit integers, where each bit represents a horizontal pixel in the
	/// integer's row.
	struct mask_tile
	{
		u8 data[8]{};
	};
} // namespace nes::app