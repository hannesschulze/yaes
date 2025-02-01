#pragma once

#include "nes/common/types.hh"
#include <string_view>

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
	enum class color : u8;

	/// Renders tiles and other primitive shapes onto a display.
	///
	/// The renderer sees the display as a canvas divided into 8x8 tiles. All elements (except shadows and borders) must
	/// be arranged  on this 8x8 grid.
	class renderer
	{
		display& display_;

	public:
		static constexpr auto width = u32{ 32 };
		static constexpr auto height = u32{ 30 };

		explicit renderer(display&);

		renderer(renderer const&) = delete;
		renderer(renderer&&) = delete;
		auto operator=(renderer const&) -> renderer& = delete;
		auto operator=(renderer&&) -> renderer& = delete;

		/// If set, add a drop shadow behind newly drawn opaque elements.
		auto set_shadow(u32 x_px, u32 y_px, color) -> void;
		/// If set, add an inner border inside newly drawn opaque elements.
		auto set_border(u32 px, color) -> void;

		/// Render a colored tile at the given tile coordinates.
		auto render_image_tile(u32 x, u32 y, image_tile const&) -> void;
		/// Render an alpha-only tile at the given tile coordinates with the given color.
		auto render_mask_tile(u32 x, u32 y, mask_tile, color) -> void;
		/// Render an image at the given tile coordinates.
		auto render_image(u32 x, u32 y, image_view) -> void;
		/// Render a string of text at the given tile coordinates (one tile per character).
		auto render_text(u32 x, u32 y, std::string_view, color) -> void;
		/// Render a string of text at the given tile coordinates, limiting its length to the given width and
		/// ellipsizing afterward.
		auto render_text(u32 x, u32 y, std::string_view, u32 width, color) -> void;
		/// Render a rectangle at the given tile coordinates with a given size (measured in tiles).
		auto render_rect(u32 x, u32 y, u32 width, u32 height, color) -> void;
		/// Completely fill the screen.
		auto render_fill(color) -> void;

	private:
		auto resolve_color(color) const -> rgb;
		auto resolve_character(u32) const -> mask_tile;
	};
} // namespace nes::app