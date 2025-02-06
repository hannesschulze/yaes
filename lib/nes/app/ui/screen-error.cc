#include "nes/app/ui/screen-error.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/input/input-device-keyboard.hh"

namespace nes::app
{
	screen_error::screen_error(input_device_keyboard& keyboard)
		: keyboard_{ keyboard }
	{
	}

	auto screen_error::render(renderer& renderer) -> void
	{
		renderer.render_rect(2, 8, 28, 10, color::background_secondary);
		renderer.render_border(2, 8, 28, 10, color::foreground_primary);

		auto const attrs = text_attributes{}
			.set_alignment(text_alignment::center)
			.set_ellipsize_mode(ellipsize_mode::end)
			.set_max_width(26);
		renderer.render_text_format(16, 10, color::foreground_primary, attrs, "{}:", message_);
		renderer.render_text(16, 12, to_string(error_), color::foreground_primary, attrs);
		renderer.render_text(16, 15, "> OK <", color::accent_primary, attrs);
	}

	auto screen_error::process_events() -> action
	{
		while (auto const event = keyboard_.poll_event())
		{
			if (event == input_event::key_down(key::enter))
			{
				return action_;
			}
		}

		return action{};
	}
} // namespace nes::app