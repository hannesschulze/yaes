#include "nes/app/ui/screen-confirm-quit.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/input/input-device-keyboard.hh"

namespace nes::app
{
	screen_confirm_quit::screen_confirm_quit(input_device_keyboard& keyboard)
		: keyboard_{ keyboard }
	{
	}

	auto screen_confirm_quit::render(renderer& renderer) -> void
	{
		renderer.render_rect(2, 8, 28, 10, color::background_secondary);
		renderer.render_border(2, 8, 28, 10, color::foreground_primary);

		auto const attrs = text_attributes{}
			.set_alignment(text_alignment::center)
			.set_ellipsize_mode(ellipsize_mode::end)
			.set_max_width(26);
		renderer.render_text(16, 10, "Are you sure?", color::foreground_primary, attrs);
		renderer.render_text(16, 12, "All progress is lost.", color::foreground_primary, attrs);
		if (confirm_)
		{
			renderer.render_text(10, 15, "Cancel", color::foreground_secondary);
			renderer.render_text(18, 15, "> OK <", color::accent_primary);
		}
		else
		{
			renderer.render_text(8, 15, "> Cancel <", color::accent_primary);
			renderer.render_text(20, 15, "OK", color::foreground_secondary);
		}
	}

	auto screen_confirm_quit::process_events() -> action
	{
		while (auto const event = keyboard_.poll_event())
		{
			// Use key-up so the enter key is not immediately captured by the game.
			if (event == input_event::key_up(key::enter))
			{
				if (confirm_)
				{
					return action::confirm_quit();
				}
				else
				{
					return action::cancel_quit();
				}
			}
			else if (event == input_event::key_down(key::space))
			{
				confirm_ = !confirm_;
			}
			else if (event == input_event::key_down(key::arrow_left))
			{
				confirm_ = false;
			}
			else if (event == input_event::key_down(key::arrow_right))
			{
				confirm_ = true;
			}
		}

		return action{};
	}
} // namespace nes::app