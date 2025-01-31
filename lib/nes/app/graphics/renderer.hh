#pragma once

#include <string_view>
#include <cstdint>

namespace nes
{
	class display;
	class rgb;
} // namespace nes

namespace nes::app
{
	struct image_tile;
	struct mask_tile;
	class image_view;
	enum class color : std::uint8_t;

	/// Renders tiles and other primitive shapes onto a display.
	///
	/// The renderer sees the display as a canvas divided into 8x8 tiles. All elements (except shadows and borders) must
	/// be arranged  on this 8x8 grid.
	class renderer
	{
		display& display_;

	public:
		static constexpr auto width = unsigned{ 32 };
		static constexpr auto height = unsigned{ 30 };

		explicit renderer(display&);

		renderer(renderer const&) = delete;
		renderer(renderer&&) = delete;
		auto operator=(renderer const&) -> renderer& = delete;
		auto operator=(renderer&&) -> renderer& = delete;

		/// If set, add a drop shadow behind newly drawn opaque elements.
		auto set_shadow(unsigned x_px, unsigned y_px, color) -> void;
		/// If set, add an inner border inside newly drawn opaque elements.
		auto set_border(unsigned px, color) -> void;

		/// Render a colored tile at the given tile coordinates.
		auto render_image_tile(unsigned x, unsigned y, image_tile) -> void;
		/// Render an alpha-only tile at the given tile coordinates with the given color.
		auto render_mask_tile(unsigned x, unsigned y, mask_tile, color) -> void;
		/// Render an image at the given tile coordinates.
		auto render_image(unsigned x, unsigned y, image_view) -> void;
		/// Render a string of text at the given tile coordinates (one tile per character).
		auto render_text(unsigned x, unsigned y, std::string_view, color) -> void;
		/// Render a string of text at the given tile coordinates, limiting its length to the given width and
		/// ellipsizing afterward.
		auto render_text(unsigned x, unsigned y, std::string_view, unsigned width, color) -> void;
		/// Render a rectangle at the given tile coordinates with a given size (measured in tiles).
		auto render_rect(unsigned x, unsigned y, unsigned width, unsigned height, color) -> void;
		/// Completely fill the screen.
		auto render_fill(color) -> void;

	private:
		auto resolve_color(color) const -> rgb;
	};
} // namespace nes::app