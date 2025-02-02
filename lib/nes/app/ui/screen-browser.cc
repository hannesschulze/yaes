#include "nes/app/ui/screen-browser.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/common/string-builder.hh"

namespace nes::app
{
	auto screen_browser::selection_impl::render_item(
		renderer& renderer, i32 const& item, i32 const x, i32 const y, u32 const width, color const color) const -> void
	{
		renderer.render_text_format(x, y, color, text_attributes{}.set_max_width(width), "Item {}", item);
	}

	auto screen_browser::selection_impl::load_page(int (&items)[5], u32 const page) -> u32
	{
		for (auto i = u32{ 0 }; i < 5; ++i)
		{
			items[i] = static_cast<i32>(page * 5 + i);
		}
		return 5;
	}

	screen_browser::screen_browser(input_device_keyboard& keyboard)
		: keyboard_{ keyboard }
	{
		selection_.set_page_count(2);
	}

	auto screen_browser::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_rect(0, 0, 32, 3, color::background_secondary);

		renderer.render_text(2, 1, "Load Game", color::foreground_primary);
		renderer.render_text_format(
			30, 1, color::foreground_secondary, text_attributes{}.set_alignment(text_alignment::right),
			"Page {}/{}", selection_.get_current_page() + 1, selection_.get_page_count());

		selection_.render(renderer);
	}

	auto screen_browser::process_events() -> action
	{
		while (auto const event = keyboard_.poll_event())
		{
			if (event == input_event::key_down(key::space))
			{
				selection_.go_next();
			}
			else if (event == input_event::key_down(key::arrow_up))
			{
				selection_.go_up();
			}
			else if (event == input_event::key_down(key::arrow_down))
			{
				selection_.go_down();
			}
			else if (event == input_event::key_down(key::backspace))
			{
				return action::go_to_title;
			}
			else if (event == input_event::key_down(key::enter))
			{
				return action::launch_game;
			}
		}

		return action::none;
	}
} // namespace nes::app