#include "nes/app/ui/screen-settings.hh"
#include "nes/app/input/input-manager.hh"
#include "nes/app/preferences.hh"

namespace nes::app
{
	auto screen_settings::selection_impl::render_item(
		renderer& renderer, item const& item, i32 const x, i32 const y, u32 const width, color const c) const -> void
	{
		auto const render = [&](std::string_view const label, std::string_view const value)
		{
			auto const label_length = renderer.render_text(x, y, label, c);
			auto const value_attrs = text_attributes{}
				.set_alignment(text_alignment::right)
				.set_ellipsize_mode(ellipsize_mode::end)
				.set_max_width(width - label_length - 2);
			renderer.render_text(x + static_cast<i32>(width), y, value, color::foreground_secondary, value_attrs);
		};
		switch (item)
		{
			case item::controller_1:
				render("Controller 1:", input_manager_.get_input_1().get_name());
				break;
			case item::controller_2:
				render("Controller 2:", input_manager_.get_input_2().get_name());
				break;
			case item::fps_counter:
				render("FPS Counter:", preferences_.get_fps_counter() ? "Yes" : "No");
				break;
		}
	}

	auto screen_settings::selection_impl::load_page(item (&items)[page_size], u32) -> u32
	{
		items[0] = item::controller_1;
		items[1] = item::controller_2;
		items[2] = item::fps_counter;
		return 3;
	}

	screen_settings::screen_settings(input_manager& input_manager, preferences& preferences)
		: input_manager_{ input_manager }
		, preferences_{ preferences }
		, selection_{ input_manager, preferences }
	{
		selection_.set_page_count(1);
	}

	auto screen_settings::render(renderer& renderer) -> void
	{
		renderer.render_fill(color::background_primary);
		renderer.render_rect(0, 0, 32, 3, color::background_secondary);

		auto const page_length = renderer.render_text_format(
			30, 1, color::foreground_secondary, text_attributes{}.set_alignment(text_alignment::right),
			"Page {}/{}", selection_.get_current_page() + 1, selection_.get_page_count());

		renderer.render_text(
			2, 1, "Settings", color::foreground_primary,
			text_attributes{}.set_max_width(26 - page_length).set_ellipsize_mode(ellipsize_mode::middle));

		selection_.render(renderer);
	}

	auto screen_settings::process_events() -> action
	{
		while (auto const event = input_manager_.get_keyboard().poll_event())
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
				return action::go_to_title();
			}
			else if (event == input_event::key_down(key::enter))
			{
				switch (*selection_.get_selected())
				{
					case item::controller_1: input_manager_.toggle_input_1(); break;
					case item::controller_2: input_manager_.toggle_input_2(); break;
					case item::fps_counter: preferences_.set_fps_counter(!preferences_.get_fps_counter()); break;
				}
			}
		}

		return action{};
	}
} // namespace nes::app