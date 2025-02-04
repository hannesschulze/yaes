#pragma once

#include "nes/common/types.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/text-attributes.hh"
#include "nes/common/string-builder.hh"
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
		auto render_image_tile(i32 x, i32 y, image_tile const&) -> void;
		/// Render an alpha-only tile at the given tile coordinates with the given color.
		auto render_mask_tile(i32 x, i32 y, mask_tile, color) -> void;
		/// Render an image at the given tile coordinates.
		auto render_image(i32 x, i32 y, image_view) -> void;
		/// Render a string of text at the given tile coordinates (one tile per character).
		auto render_text(i32 x, i32 y, std::string_view, color, text_attributes = text_attributes{}) -> void;

		template<typename... Args>
		auto render_text_format(
			i32 const x, i32 const y, color const c, text_attributes const attrs, std::string_view const fmt,
			Args... args) -> u32
		{
			string_buffer<width> buffer{};
			buffer.append_format(fmt, args...);
			render_text(x, y, buffer.get_result(), c, attrs);
			return static_cast<u32>(buffer.get_result().length());
		}

		template<typename... Args>
		auto render_text_format(
			i32 const x, i32 const y, color const c, std::string_view const fmt, Args... args) -> u32
		{
			return render_text_format(x, y, c, text_attributes{}, fmt, args...);
		}

		/// Render a rectangle at the given tile coordinates with a given size (measured in tiles).
		auto render_rect(i32 x, i32 y, u32 width, u32 height, color) -> void;
		/// Completely fill the screen.
		auto render_fill(color) -> void;

	private:
		auto resolve_color(color) const -> rgb;
		auto resolve_character(u32) const -> mask_tile;
	};
} // namespace nes::app