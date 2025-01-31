#include "nes/app/ui/screen-title.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/mask-tile.hh"
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
		renderer.render_rect(2, 4, 28, 8, color::accent_primary);
		renderer.render_mask_tile(10, 16, tiles::letter_a, color::foreground_primary);
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