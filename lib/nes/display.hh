#pragma once

namespace nes
{
	class rgb;

	/// Abstraction of the display logic.
	class display
	{
	public:
		static constexpr auto width = unsigned{ 256 };
		static constexpr auto height = unsigned{ 240 };

		virtual ~display() = default;

		display(display const&) = delete;
		display(display&&) = delete;
		auto operator=(display const&) -> display& = delete;
		auto operator=(display&&) -> display& = delete;

		/// Switch between front and back buffers.
		virtual auto switch_buffers() -> void = 0;
		/// Update the pixel at the given position in the back buffer.
		virtual auto set(unsigned x, unsigned y, rgb value) -> void = 0;
		/// Load the pixel value at the given position in the back buffer.
		virtual auto get(unsigned x, unsigned y) const -> rgb = 0;
		/// Clear the entire back buffer with a color.
		virtual auto clear(rgb color) -> void;

	protected:
		explicit display() = default;
	};
} // namespace nes