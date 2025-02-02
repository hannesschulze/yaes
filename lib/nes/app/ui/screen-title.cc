#include "nes/app/ui/screen-title.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/tiles/banner.hh"
#include "nes/app/input/input-device-keyboard.hh"

namespace nes::app
{
	auto screen_title::selection_impl::render_item(
		renderer& renderer, item const& item, i32 const x, i32 const y, u32 const width, color const color) const -> void
	{
		auto const attrs = text_attributes{}
			.set_max_width(width);
		switch (item)
		{
			case item::load_game: renderer.render_text(x, y, "Load Game", color, attrs); break;
			case item::settings: renderer.render_text(x, y, "Settings", color, attrs); break;
			case item::help: renderer.render_text(x, y, "Help", color, attrs); break;
		}
	}

	auto screen_title::selection_impl::load_page(item (&items)[3], u32) -> u32
	{
		items[0] = item::load_game;
		items[1] = item::settings;
		items[2] = item::help;
		return 3;
	}

	screen_title::screen_title(input_device_keyboard& keyboard)
		: keyboard_{ keyboard }
	{
		selection_.set_page_count(1);
	}

	auto screen_title::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_image(2, 4, tiles::banner);

		selection_.render(renderer);
	}

	auto screen_title::process_events() -> action
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
			else if (event == input_event::key_down(key::enter))
			{
				switch (*selection_.get_selected())
				{
					case item::load_game: return action::go_to_browser();
					case item::settings: return action::go_to_settings();
					case item::help: return action::go_to_help();
				}
			}
		}

		return action{};
	}
} // namespace nes::app