#include "nes/app/application.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-device-controller.hh"
#include "nes/app/ui/screen.hh"
#include "nes/app/graphics/renderer.hh"
#include "ui/action.hh"

#include <iostream>

namespace nes::app
{
	namespace
	{
		constexpr auto file = std::string_view{ "/Users/hannes/Documents/temp/supermario.nes" };
	} // namespace

	auto application::display_proxy::switch_buffers() -> void
	{
		if (screen)
		{
			auto r = renderer{ base };
			screen->render(r);
		}

		base.switch_buffers();
	}

	application::application(display& display, input_device_keyboard& keyboard)
		: display_{ display }
		, input_manager_{ keyboard }
		, screen_title_{ input_manager_ }
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
			console_->ref_controller_1().set_pressed(input_manager_.get_input_1().read_mask());
			console_->ref_controller_2().set_pressed(input_manager_.get_input_2().read_mask());

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
		switch (a)
		{
			case action::none:
			{
				break;
			}
			case action::load_game:
			{
				console_.emplace(sys::cartridge::from_file(file), display_);
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