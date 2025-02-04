#include "nes/app/application.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-device-controller.hh"
#include "nes/app/ui/screen.hh"
#include "nes/app/graphics/renderer.hh"
#include "ui/action.hh"
#include <iostream>

namespace nes::app
{
	auto application::display_proxy::switch_buffers() -> void
	{
		if (screen)
		{
			auto r = renderer{ base };
			screen->render(r);
		}

		base.switch_buffers();
	}

	application::application(display& display, input_device_keyboard& keyboard, file_browser& file_browser)
		: display_{ display }
		, input_manager_{ keyboard }
		, file_browser_{ file_browser }
		, screen_title_{ keyboard }
		, screen_browser_{ keyboard, file_browser }
	{
		display_.screen = &screen_title_;
	}

	auto application::frame(std::chrono::microseconds const elapsed_time) -> void
	{
		if (display_.screen)
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

			if (console_->get_status() != sys::status::success)
			{
				std::cerr << "Invalid state: " << to_string(console_->get_status()) << std::endl;
				std::abort();
			}
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
				break;
			}
			case action::type::go_to_browser:
			{
				console_.clear();
				display_.screen = &screen_browser_;
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
			case action::type::launch_game:
			{
				u8 buffer[sys::cartridge::max_file_size];
				auto const length = file_browser_.load(a.get_file_name(), buffer);

				console_.emplace(display_, span<u8 const>{ buffer, length });
				if (console_->get_status() != sys::status::success)
				{
					std::cerr << "Unable to load cartridge: " << to_string(console_->get_status()) << std::endl;
					std::abort();
				}

				display_.screen = nullptr;
				break;
			}
		}
	}
} // namespace nes::app