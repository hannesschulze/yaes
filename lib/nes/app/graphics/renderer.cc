#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/mask-tile.hh"
#include "nes/app/graphics/image-tile.hh"
#include "nes/app/graphics/image-view.hh"
#include "nes/common/display.hh"

namespace nes::app
{
	renderer::renderer(display& display)
		: display_{ display }
	{
	}

	auto renderer::render_image_tile(unsigned const x, unsigned const y, image_tile const tile) -> void
	{
		if (x >= width || y >= height) { return; }

		for (auto y_px = unsigned{ 0 }; y_px < 8; ++y_px)
		{
			for (auto x_px = unsigned{ 0 }; x_px < 8; ++x_px)
			{
				auto const c = tile.data[y_px * 8 + x_px];
				if (c != color::transparent)
				{
					display_.set(8 * x + x_px, 8 * y + y_px, resolve_color(c));
				}
			}
		}
	}

	auto renderer::render_mask_tile(unsigned const x, unsigned const y, mask_tile const tile, color const c) -> void
	{
		if (c == color::transparent) { return; }
		if (x >= width || y >= height) { return; }

		auto const resolved = resolve_color(c);
		for (auto y_px = unsigned{ 0 }; y_px < 8; ++y_px)
		{
			auto row = tile.data[y_px];
			for (auto x_px = unsigned{ 0 }; x_px < 8; ++x_px)
			{
				if (row & 0b10000000)
				{
					display_.set(8 * x + x_px, 8 * y + y_px, resolved);
				}
				row <<= 1;
			}
		}
	}

	auto renderer::render_image(unsigned const x, unsigned const y, image_view const image) -> void
	{
		for (auto y_img = unsigned{ 0 }; y_img < image.get_height(); ++y_img)
		{
			for (auto x_img = unsigned{ 0 }; x_img < image.get_width(); ++x_img)
			{
				render_image_tile(x + x_img, y + y_img, image.get(x, y));
			}
		}
	}

	auto renderer::render_rect(
		unsigned const x, unsigned const y, unsigned const width, unsigned const height, color const c) -> void
	{
		if (c == color::transparent) { return; }
		if (x >= width || y >= height) { return; }

		auto const resolved = resolve_color(c);
		auto const x_stop = std::min(x + width, renderer::width);
		auto const y_stop = std::min(y + width, renderer::height);
		for (auto y_px = unsigned{ y * 8 }; y_px < y_stop * 8; ++y_px)
		{
			for (auto x_px = unsigned{ x * 8 }; x_px < x_stop * 8; ++x_px)
			{
				display_.set(x_px, y_px, resolved);
			}
		}
	}

	auto renderer::render_fill(color const c) -> void
	{
		render_rect(0, 0, width, height, c);
	}

	auto renderer::resolve_color(color const c) const -> rgb
	{
		switch (c)
		{
			case color::transparent: break;
			case color::background_primary: return rgb::from_hex(0x004058);
			case color::background_secondary: return rgb::from_hex(0x002B3B);
			case color::foreground_primary: return rgb::from_hex(0xFCFCFC);
			case color::foreground_secondary: return rgb::from_hex(0x008888);
			case color::accent_primary: return rgb::from_hex(0xE45C10);
			case color::accent_secondary: return rgb::from_hex(0x881400);
			case color::fixed_white: return rgb::from_hex(0xFCFCFC);
			case color::fixed_gray: return rgb::from_hex(0xBCBCBC);
			case color::fixed_black: return rgb::from_hex(0x000000);
		}

		return rgb::from_hex(0x000000);
	}
} // namespace nes::app