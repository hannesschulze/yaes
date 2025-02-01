#pragma once

#include "nes/common/rgb.hh"
#include "nes/common/types.hh"

namespace nes
{
	class rgb;

	/// Abstraction of the display logic.
	class display
	{
	public:
		static constexpr auto width = u32{ 256 };
		static constexpr auto height = u32{ 240 };

		virtual ~display() = default;

		display(display const&) = delete;
		display(display&&) = delete;
		auto operator=(display const&) -> display& = delete;
		auto operator=(display&&) -> display& = delete;

		/// Switch between front and back buffers.
		virtual auto switch_buffers() -> void = 0;
		/// Update the pixel at the given position in the back buffer.
		virtual auto set(u32 x, u32 y, rgb value) -> void = 0;
		/// Load the pixel value at the given position in the back buffer.
		virtual auto get(u32 x, u32 y) const -> rgb = 0;

	protected:
		explicit display() = default;
	};
} // namespace nes