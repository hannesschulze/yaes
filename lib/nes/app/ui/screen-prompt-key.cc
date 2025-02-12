#include "nes/app/ui/screen-prompt-key.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/input/input-device-keyboard.hh"

namespace nes::app
{
	screen_prompt_key::screen_prompt_key(input_device_keyboard& keyboard)
		: keyboard_{ keyboard }
	{
	}

	auto screen_prompt_key::reset(string_view const file_name) -> void
	{
		file_name_ = file_name;
		key_.clear();
	}

	auto screen_prompt_key::render(renderer& renderer) -> void
	{
		renderer.render_rect(2, 8, 28, 10, color::background_secondary);
		renderer.render_border(2, 8, 28, 10, color::foreground_primary);
		renderer.render_rect(4, 12, 24, 1, color::background_primary);

		auto const attrs = text_attributes{}
			.set_alignment(text_alignment::center)
			.set_ellipsize_mode(ellipsize_mode::end)
			.set_max_width(26);
		renderer.render_text(16, 10, "Key:", color::foreground_primary, attrs);
		auto const key_attrs = text_attributes{}
			.set_alignment(text_alignment::left)
			.set_ellipsize_mode(ellipsize_mode::clip)
			.set_max_width(24);
		renderer.render_text_format(4, 12, color::foreground_primary, key_attrs, "{}_", key_.get_result());
		renderer.render_text(16, 15, "> OK <", color::accent_primary, attrs);
	}

	auto screen_prompt_key::process_events() -> action
	{
		while (auto const event = keyboard_.poll_event())
		{
			if (event == input_event::key_down(key::escape))
			{
				return action::close_popup();
			}
			else if (event == input_event::key_down(key::enter))
			{
				return action::launch_game(file_name_, key_.get_result());
			}
			else if (event == input_event::key_down(key::backspace))
			{
				key_.remove_last();
			}
			else if (event.get_type() == input_event::type::key_down)
			{
				auto const symbol = to_ascii(event.get_key(), keyboard_.read_modifiers());
				if (symbol != 0)
				{
					key_.append_char(symbol);
				}
			}
		}

		return action{};
	}
} // namespace nes::app