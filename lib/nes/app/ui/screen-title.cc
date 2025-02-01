#include "nes/app/ui/screen-title.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/tiles/banner.hh"
#include "nes/app/input/input-manager.hh"

namespace nes::app
{
	screen_title::screen_title(input_manager& input_manager)
		: input_manager_{ input_manager }
	{
	}

	auto screen_title::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_image(2, 4, tiles::banner);

		if (selected_item_ == item::load_game) { renderer.render_text(10, 16, ">", color::accent_primary); }
		renderer.render_text(12, 16, "Load Game", selected_item_ == item::load_game ? color::accent_primary : color::foreground_primary);

		if (selected_item_ == item::settings) { renderer.render_text(10, 19, ">", color::accent_primary); }
		renderer.render_text(12, 19, "Settings", selected_item_ == item::settings ? color::accent_primary : color::foreground_primary);

		if (selected_item_ == item::help) { renderer.render_text(10, 22, ">", color::accent_primary); }
		renderer.render_text(12, 22, "Help", selected_item_ == item::help ? color::accent_primary : color::foreground_primary);
	}

	auto screen_title::process_events() -> action
	{
		while (auto event = input_manager_.get_keyboard().poll_event())
		{
			if (event == input_event::key_down(key::space))
			{
				switch (selected_item_)
				{
					case item::load_game: selected_item_ = item::settings; break;
					case item::settings: selected_item_ = item::help; break;
					case item::help: selected_item_ = item::load_game; break;
				}
			}
			else if (event == input_event::key_down(key::enter))
			{
				return action::load_game;
			}
		}

		return action::none;
	}
} // namespace nes::app