#include "nes/app/ui/screen-title.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/tiles/banner.hh"
#include "nes/app/graphics/tiles/icons.hh"
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
		renderer.render_text(10, 16, "Hello, world!", color::foreground_primary);
		renderer.render_mask_tile(14, 18, tiles::icon_directory, color::accent_primary);
		renderer.render_mask_tile(16, 18, tiles::icon_cartridge, color::accent_primary);
		renderer.render_image(2, 4, tiles::banner);
	}

	auto screen_title::process_events() -> action
	{
		if (input_manager_.get_keyboard().read_key_start())
		{
			return action::load_game;
		}

		return action::none;
	}
} // namespace nes::app