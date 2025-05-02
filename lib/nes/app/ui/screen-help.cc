#include "nes/app/ui/screen-help.hh"
#include "nes/app/input/input-manager.hh"
#include "nes/app/preferences.hh"

namespace
{
	

}

namespace nes::app
{
	screen_help::screen_help(input_manager& input_manager)
		: input_manager_{ input_manager }
	{ }

	auto screen_help::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_rect(0, 0, 32, 3, color::background_secondary);

		renderer.render_text(
			2, 1, "Key Bindings", color::foreground_primary,
			text_attributes{}.set_max_width(26).set_ellipsize_mode(ellipsize_mode::middle));

		auto render_list_item = [&](unsigned& y, string_view name, string_view value, bool fullline = true, bool left = true) -> void
		{
			unsigned x_offset = 2;
			unsigned container_width = renderer::width - 2 * x_offset;
			if (!fullline)
			{
				container_width -= x_offset; // offset between columns
				container_width /= 2;
			}

			unsigned column_offset = 0;
			if (!fullline && !left)
			{
				column_offset = container_width + 2 * x_offset;
			}
			else
			{
				column_offset = x_offset;
			}

			unsigned max_val_width = (container_width - x_offset) / 2;
			unsigned value_width = value.get_length();
			if (value_width > max_val_width)
			{
				value_width = max_val_width;
			}

			unsigned name_width = container_width - value_width - x_offset;

			renderer.render_text(
				column_offset, y, name, color::foreground_primary,
				text_attributes{}.set_max_width(name_width).set_ellipsize_mode(ellipsize_mode::middle));

			renderer.render_text(
				column_offset + container_width, y, value, color::foreground_secondary,
				text_attributes{}.set_alignment(text_alignment::right).set_max_width(value_width).set_ellipsize_mode(ellipsize_mode::middle));
			
			if (fullline || !left)
			{
				y += 2;
			}
		};

		auto render_title = [&](unsigned& y, string_view title) -> void
		{
			renderer.render_text(
				14 + 2, y, title, color::accent_primary,
				text_attributes{}.set_alignment(nes::app::text_alignment::center).set_max_width(28).set_ellipsize_mode(ellipsize_mode::middle));
			y += 2;
		};

		unsigned y_offset = 5;

		render_title(y_offset, "> Launcher <");

		render_list_item(y_offset, "Select", "<Enter>");
		render_list_item(y_offset, "Navigation", "<Arrow Keys>");
		render_list_item(y_offset, "Go Back", "<Backspace>");

		y_offset += 1;
		render_title(y_offset, "> Games <");

		render_list_item(y_offset, "Up", "W", false, true);
		render_list_item(y_offset, "Left", "A", false, false);
		render_list_item(y_offset, "Down", "S", false, true);
		render_list_item(y_offset, "Right", "D", false, false);
		render_list_item(y_offset, "Action B", "K", false, true);
		render_list_item(y_offset, "Action A", "L", false, false);
		render_list_item(y_offset, "Start", "<Enter>");
		render_list_item(y_offset, "Select", "<Space>");
	}

	auto screen_help::process_events() -> action
	{
		while (auto const event = input_manager_.get_keyboard().poll_event())
		{
			if (event == input_event::key_down(key::backspace))
			{
				return action::go_to_title();
			}
		}

		return action{};
	}
} // namespace nes::app