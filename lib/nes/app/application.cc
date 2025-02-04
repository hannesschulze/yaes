#include "nes/app/application.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-device-controller.hh"
#include "nes/app/ui/screen.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/action.hh"
#include <iostream>

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

		base.switch_buffers();
	}

	application::application(display& display, input_device_keyboard& keyboard, file_browser& file_browser)
		: display_{ display }
		, input_manager_{ keyboard }
		, file_browser_{ file_browser }
		, screen_title_{ keyboard }
		, screen_browser_{ keyboard, file_browser }
		, screen_error_{ keyboard }
	{
		display_.screen = &screen_title_;
	}

	auto application::frame(std::chrono::microseconds const elapsed_time) -> void
	{
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

		if (console_)
		{
			console_->ref_controller_1().set_pressed(input_manager_.get_input_1().read_buttons());
			console_->ref_controller_2().set_pressed(input_manager_.get_input_2().read_buttons());

			// The scene is rendered by the console after the PPU requests a new frame (thus calling
			// display_proxy::switch_buffers).
			console_->step(elapsed_time);

			if (console_->get_status() != status::success)
			{
				// TODO: Present as error popup
				std::cerr << "Invalid state: " << to_string(console_->get_status()) << std::endl;
				std::abort();
			}

			// TODO: Handle escape key
		}
		else
		{
			// Directly let the scene render itself without rendering any gameplay (the scene is responsible for
			// clearing the buffer).
			display_.switch_buffers();
		}
	}

	auto application::handle_action(action const a) -> void
	{
		switch (a.get_type())
		{
			case action::type::none:
			{
				break;
			}
			case action::type::go_to_title:
			{
				console_.clear();
				display_.screen = &screen_title_;
				display_.popup = nullptr;
				break;
			}
			case action::type::go_to_browser:
			{
				console_.clear();
				display_.screen = &screen_browser_;
				display_.popup = nullptr;
				break;
			}
			case action::type::go_to_settings:
			{
				// TODO
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

	auto application::show_error(std::string_view const message, status const error) -> void
	{
		screen_error_.set_message(message);
		screen_error_.set_error(error);
		display_.popup = &screen_error_;
	}

} // namespace nes::app