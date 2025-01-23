#include "nes/display.hh"
#include "nes/util/rgb.hh"

namespace nes
{
	auto display::clear(rgb const color) -> void
	{
		for (auto y = unsigned{ 0 }; y < height; ++y)
		{
			for (auto x = unsigned{ 0 }; x < width; ++x)
			{
				set(x, y, color);
			}
		}
	}
} // namespace nes