#include "nes/app/ui/screen-file-viewer.hh"
#include "nes/app/file-browser.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/common/containers/span.hh"

namespace nes::app
{
	screen_file_viewer::screen_file_viewer(input_device_keyboard& keyboard, file_browser& file_browser)
		: keyboard_{ keyboard }
		, file_browser_{ file_browser }
	{
	}

	auto screen_file_viewer::load(string_view const file_name) -> status
	{
		auto length = u32{ 0 };
		if (auto const s = file_browser_.load(file_name, buffer_, &length); s != status::success) { return s; }

		// Do some magical text wrapping
		auto const data = span{ buffer_ }.subspan(0, length);
		auto const src = string_view{ data.unsafe_bitcast<char const>() };
		line_count_ = 0;
		auto line_pos = u32{ 0 };
		auto line_start = u32{ 0 };
		auto const end_line = [&](bool const force)
		{
			auto const l = src.substring(line_start, line_pos);
			if (l.is_empty() && !force) { return; }

			if (line_count_ < max_line_count)
			{
				lines_[line_count_] = l;
				line_count_ += 1;
			}
			line_pos = 0;
		};
		for (auto i = u32{ 0 }; i < src.get_length(); ++i)
		{
			auto const c = src[i];
			if (c == '\n' || line_pos == line_length)
			{
				end_line(c == '\n');
				line_pos = 0;
				line_start = i + 1;
			}
			else if (c == '\r')
			{
				// ignore
			}
			else
			{
				line_pos += 1;
			}
		}
		end_line(false);

		return status::success;
	}

	auto screen_file_viewer::render(renderer& renderer) -> void
	{
		auto const width = line_length + 4;
		auto const height = line_count_ + 7;
		auto const y = (renderer::height - height) / 2 + 2;
		renderer.render_rect(2, y, width, height, color::background_secondary);
		renderer.render_border(2, y, width, height, color::foreground_primary);

		auto const attrs = text_attributes{}
			.set_alignment(text_alignment::left)
			.set_max_width(line_length);
		for (auto i = u32{ 0 }; i < line_count_; ++i)
		{
			renderer.render_text(4, y + 2 + i, lines_[i], color::foreground_primary, attrs);
		}
		auto const button_attrs = text_attributes{}
			.set_alignment(text_alignment::center)
			.set_ellipsize_mode(ellipsize_mode::end)
			.set_max_width(26);
		renderer.render_text(16, y + height - 3, "> Close <", color::accent_primary, button_attrs);
	}

	auto screen_file_viewer::process_events() -> action
	{
		while (auto const event = keyboard_.poll_event())
		{
			if (event == input_event::key_down(key::escape) || event == input_event::key_down(key::enter))
			{
				return action::close_popup();
			}
		}

		return action{};
	}
} // namespace nes::app
