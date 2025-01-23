#pragma once

#include <cstdint>

namespace nes
{
	/// An RGB value.
	class rgb
	{
	public:
		explicit rgb() = default;

		explicit rgb(std::uint8_t const r, std::uint8_t const g, std::uint8_t const b)
			: r{ r }
			, g{ g }
			, b{ b }
		{
		}

		static auto from_hex(std::uint32_t const hex) -> rgb
		{
			auto res = rgb{};
			res.r = (hex & 0x00FF0000) >> 16;
			res.g = (hex & 0x0000FF00) >> 8;
			res.b = (hex & 0x000000FF) >> 0;
			return res;
		}

		std::uint8_t r{ 0 };
		std::uint8_t g{ 0 };
		std::uint8_t b{ 0 };
	};
} // namespace nes