#include "nes/app/application.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-device-controller.hh"
#include "nes/app/ui/screen.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/action.hh"

namespace nes::app
{
	auto application::display_proxy::switch_buffers() -> void
	{
		auto r = renderer{ base };

		if (screen)
		{
			screen->render(r);
		}

		if (popup)
		{
			popup->render(r);
		}

		if (preferences_.get_fps_counter())
		{
			r.render_rect(30, 29, 2, 1, color::fixed_black);
			auto const attrs = text_attributes{}
				.set_max_width(2)
				.set_alignment(text_alignment::right);
			r.render_text_format(32, 29, color::fixed_white, attrs, "{}", fps_counter.get_fps());
		}

		base.switch_buffers();
	}

	application::application(display& display, input_device_keyboard& keyboard, file_browser& file_browser)
		: input_manager_{ keyboard }
		, file_browser_{ file_browser }
		, display_{ preferences_, display }
		, screen_title_{ keyboard }
		, screen_browser_{ keyboard, file_browser }
		, screen_settings_{ input_manager_, preferences_ }
		, screen_error_{ keyboard }
		, screen_confirm_quit_{ keyboard }
	{
		display_.screen = &screen_title_;
	}

	auto application::frame(std::chrono::microseconds const elapsed_time) -> void
	{
		display_.fps_counter.frame(elapsed_time);

		if (display_.popup)
		{
			auto const a = display_.popup->process_events();
			handle_action(a);
		}
		else if (display_.screen)
		{
			auto const a = display_.screen->process_events();
			handle_action(a);
		}

		if (display_.screen)
		{
			// Directly let the scene render itself without rendering any gameplay (the scene is responsible for
			// clearing the buffer).
			display_.switch_buffers();
		}
		else if (console_)
		{
			// Handle escape key.
			while (auto const event = input_manager_.get_keyboard().poll_event())
			{
				if (event == input_event::key_down(key::escape))
				{
					screen_freeze_.freeze(display_);
					screen_confirm_quit_.set_confirm(false);
					display_.screen = &screen_freeze_;
					display_.popup = &screen_confirm_quit_;
					display_.switch_buffers();
					return;
				}
			}

			// Forward current input state to the NES.
			console_->ref_controller_1().set_pressed(input_manager_.get_input_1().read_buttons());
			console_->ref_controller_2().set_pressed(input_manager_.get_input_2().read_buttons());

			// The scene is rendered by the console after the PPU requests a new frame (thus calling
			// display_proxy::switch_buffers).
			console_->step(elapsed_time);

			if (console_->get_status() != status::success)
			{
				screen_freeze_.freeze(display_);
				display_.screen = &screen_freeze_;
				show_error("Runtime error", console_->get_status(), action::go_to_browser());
				console_.clear();
			}
		}
	}

	auto application::handle_action(action const& a) -> void
	{
		switch (a.get_type())
		{
			case action::type::none:
			{
				break;
			}
			case action::type::go_to_title:
			{
				go_to_screen(&screen_title_);
				break;
			}
			case action::type::go_to_browser:
			case action::type::confirm_quit:
			{
				go_to_screen(&screen_browser_);
				break;
			}
			case action::type::go_to_settings:
			{
				go_to_screen(&screen_settings_);
				break;
			}
			case action::type::go_to_help:
			{
				// TODO
				break;
			}
			case action::type::close_popup:
			{
				display_.popup = nullptr;
				break;
			}
			case action::type::cancel_quit:
			{
				display_.popup = nullptr;
				display_.screen = nullptr;
				break;
			}
			case action::type::launch_game:
			{
				u8 buffer[sys::cartridge::max_file_size];
				auto length = u32{ 0 };
				if (auto const s = file_browser_.load(a.get_file_name(), buffer, &length); s != status::success)
				{
					show_error("Unable to open file", s);
					break;
				}

				console_.emplace(display_, span{ buffer, length });
				if (console_->get_status() != status::success)
				{
					show_error("Unable to load cartridge", console_->get_status());
					console_.clear();
					break;
				}

				display_.screen = nullptr;
				display_.popup = nullptr;
				break;
			}
			case action::type::show_error:
			{
				show_error(a.get_message(), a.get_error());
				break;
			}
		}
	}

	auto application::show_error(std::string_view const message, status const error, action const& action) -> void
	{
		screen_error_.set_message(message);
		screen_error_.set_error(error);
		screen_error_.set_action(action);
		display_.popup = &screen_error_;
	}

	auto application::go_to_screen(screen* screen) -> void
	{
		console_.clear();
		display_.screen = screen;
		display_.popup = nullptr;
	}
} // namespace nes::app