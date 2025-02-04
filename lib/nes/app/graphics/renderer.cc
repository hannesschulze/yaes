#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/mask-tile.hh"
#include "nes/app/graphics/image-tile.hh"
#include "nes/app/graphics/image-view.hh"
#include "nes/app/graphics/text-attributes.hh"
#include "nes/app/graphics/tiles/characters.hh"
#include "nes/common/display.hh"

namespace nes::app
{
	renderer::renderer(display& display)
		: display_{ display }
	{
	}

	auto renderer::render_image_tile(i32 const x, i32 const y, image_tile const& tile) -> void
	{
		if (x < 0 || y < 0 || x >= static_cast<i32>(width) || y >= static_cast<i32>(height)) { return; }

		for (auto y_px = u32{ 0 }; y_px < 8; ++y_px)
		{
			for (auto x_px = u32{ 0 }; x_px < 8; ++x_px)
			{
				auto const c = tile.data[y_px * 8 + x_px];
				if (c != color::transparent)
				{
					display_.set(8 * static_cast<u32>(x) + x_px, 8 * static_cast<u32>(y) + y_px, resolve_color(c));
				}
			}
		}
	}

	auto renderer::render_mask_tile(i32 const x, i32 const y, mask_tile const tile, color const c) -> void
	{
		if (c == color::transparent) { return; }
		if (x < 0 || y < 0 || x >= static_cast<i32>(width) || y >= static_cast<i32>(height)) { return; }

		auto const resolved = resolve_color(c);
		for (auto y_px = u32{ 0 }; y_px < 8; ++y_px)
		{
			auto row = tile.data[y_px];
			for (auto x_px = u32{ 0 }; x_px < 8; ++x_px)
			{
				if (row & 0b10000000)
				{
					display_.set(8 * static_cast<u32>(x) + x_px, 8 * static_cast<u32>(y) + y_px, resolved);
				}
				row <<= 1;
			}
		}
	}

	auto renderer::render_image(i32 const x, i32 const y, image_view const image) -> void
	{
		for (auto y_img = u32{ 0 }; y_img < image.get_height(); ++y_img)
		{
			for (auto x_img = u32{ 0 }; x_img < image.get_width(); ++x_img)
			{
				render_image_tile(x + static_cast<i32>(x_img), y + static_cast<i32>(y_img), image.get(x_img, y_img));
			}
		}
	}

	auto renderer::render_text(
		i32 x, i32 const y, std::string_view const str, color const c, text_attributes const attributes) -> void
	{
		// Enforce max length
		char buffer[width];
		auto const length = std::min(static_cast<u32>(str.length()), std::min(width, attributes.get_max_width()));
		auto const ellipsize_dots = std::min(length, u32{ 3 });

		// Enforce ellipsize mode
		if (str.length() > length)
		{
			switch (attributes.get_ellipsize_mode())
			{
				case ellipsize_mode::clip:
				{
					std::copy_n(str.begin(), length, buffer);
					break;
				}
				case ellipsize_mode::end:
				{
					std::copy_n(str.begin(), length, buffer);
					for (auto i = u32{ 0 }; i < ellipsize_dots; ++i) { buffer[length - 1 - i] = '.'; }
					break;
				}
				case ellipsize_mode::start:
				{
					std::copy_n(str.begin() + str.length() - length, length, buffer);
					for (auto i = u32{ 0 }; i < ellipsize_dots; ++i) { buffer[i] = '.'; }
					break;
				}
				case ellipsize_mode::middle:
				{
					auto const visible = length - ellipsize_dots;
					auto const start = visible / 2;
					auto const end = visible - start;
					std::copy_n(str.begin(), start, buffer);
					for (auto i = u32{ 0 }; i < ellipsize_dots; ++i) { buffer[start + i] = '.'; }
					std::copy_n(str.begin() + str.length() - end, end, buffer + start + ellipsize_dots);
					break;
				}
			}
		}
		else
		{
			std::copy_n(str.begin(), length, buffer);
		}

		// Enforce alignment
		switch (attributes.get_alignment())
		{
			case text_alignment::left: break;
			case text_alignment::center: x -= length / 2; break;
			case text_alignment::right: x -= length; break;
		}

		// Draw the text
		for (auto i = u32{ 0 }; i < length; ++i)
		{
			auto const character = static_cast<u32>(buffer[i]);
			render_mask_tile(x + static_cast<i32>(i), y, resolve_character(character), c);
		}
	}

	auto renderer::render_rect(
		i32 const x, i32 const y, u32 const width, u32 const height, color const c) -> void
	{
		if (c == color::transparent) { return; }
		if (x >= static_cast<i32>(renderer::width) || y >= static_cast<i32>(renderer::height)) { return; }

		auto const resolved = resolve_color(c);
		auto const x_stop = std::min(x + static_cast<i32>(width), static_cast<i32>(renderer::width));
		auto const y_stop = std::min(y + static_cast<i32>(height), static_cast<i32>(renderer::height));
		auto const x_start = std::max(x, 0);
		auto const y_start = std::max(y, 0);
		for (auto y_px = i32{ y_start * 8 }; y_px < y_stop * 8; ++y_px)
		{
			for (auto x_px = i32{ x_start * 8 }; x_px < x_stop * 8; ++x_px)
			{
				display_.set(static_cast<u32>(x_px), static_cast<u32>(y_px), resolved);
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

	auto renderer::resolve_character(u32 const c) const -> mask_tile
	{
		switch (c)
		{
			case ' ': return mask_tile{};
			case 'a': case 'A': return tiles::letter_a;
			case 'b': case 'B': return tiles::letter_b;
			case 'c': case 'C': return tiles::letter_c;
			case 'd': case 'D': return tiles::letter_d;
			case 'e': case 'E': return tiles::letter_e;
			case 'f': case 'F': return tiles::letter_f;
			case 'g': case 'G': return tiles::letter_g;
			case 'h': case 'H': return tiles::letter_h;
			case 'i': case 'I': return tiles::letter_i;
			case 'j': case 'J': return tiles::letter_j;
			case 'k': case 'K': return tiles::letter_k;
			case 'l': case 'L': return tiles::letter_l;
			case 'm': case 'M': return tiles::letter_m;
			case 'n': case 'N': return tiles::letter_n;
			case 'o': case 'O': return tiles::letter_o;
			case 'p': case 'P': return tiles::letter_p;
			case 'q': case 'Q': return tiles::letter_q;
			case 'r': case 'R': return tiles::letter_r;
			case 's': case 'S': return tiles::letter_s;
			case 't': case 'T': return tiles::letter_t;
			case 'u': case 'U': return tiles::letter_u;
			case 'v': case 'V': return tiles::letter_v;
			case 'w': case 'W': return tiles::letter_w;
			case 'x': case 'X': return tiles::letter_x;
			case 'y': case 'Y': return tiles::letter_y;
			case 'z': case 'Z': return tiles::letter_z;
			case '0': return tiles::digit_0;
			case '1': return tiles::digit_1;
			case '2': return tiles::digit_2;
			case '3': return tiles::digit_3;
			case '4': return tiles::digit_4;
			case '5': return tiles::digit_5;
			case '6': return tiles::digit_6;
			case '7': return tiles::digit_7;
			case '8': return tiles::digit_8;
			case '9': return tiles::digit_9;
			case '&': return tiles::symbol_ampersand;
			case '\'': return tiles::symbol_apostrophe;
			case '[': return tiles::symbol_bracket_open;
			case ']': return tiles::symbol_bracket_close;
			case ':': return tiles::symbol_colon;
			case ',': return tiles::symbol_comma;
			case '$': return tiles::symbol_dollar;
			case '=': return tiles::symbol_equal;
			case '!': return tiles::symbol_exclamation_mark;
			case '>': return tiles::symbol_greater;
			case '#': return tiles::symbol_hashtag;
			case '<': return tiles::symbol_less;
			case '-': return tiles::symbol_minus;
			case '(': return tiles::symbol_paren_open;
			case ')': return tiles::symbol_paren_close;
			case '%': return tiles::symbol_percent;
			case '.': return tiles::symbol_period;
			case '+': return tiles::symbol_plus;
			case '?': return tiles::symbol_question_mark;
			case '"': return tiles::symbol_quotation_mark;
			case ';': return tiles::symbol_semicolon;
			case '/': return tiles::symbol_slash;
			case '*': return tiles::symbol_star;
			default: return tiles::symbol_unknown;
		}
	}
} // namespace nes::app