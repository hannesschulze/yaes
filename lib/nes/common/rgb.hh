#pragma once

#include "nes/common/types.hh"

namespace nes
{
	/// An RGB value.
	class rgb
	{
	public:
		explicit rgb() = default;

		explicit rgb(u8 const r, u8 const g, u8 const b)
			: r{ r }
			, g{ g }
			, b{ b }
		{
		}

		static auto from_hex(u32 const hex) -> rgb
		{
			auto res = rgb{};
			res.r = (hex & 0x00FF0000) >> 16;
			res.g = (hex & 0x0000FF00) >> 8;
			res.b = (hex & 0x000000FF) >> 0;
			return res;
		}

		u8 r{ 0 };
		u8 g{ 0 };
		u8 b{ 0 };
	};
} // namespace nes