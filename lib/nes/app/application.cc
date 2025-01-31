#include "nes/app/application.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/input/input-device-controller.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/graphics/color.hh"
#include "nes/app/graphics/mask-tile.hh"
#include "nes/app/graphics/image-tile.hh"
#include "nes/common/display.hh"
#include <iostream>

namespace nes::app
{
	namespace
	{
		constexpr auto file = std::string_view{ "/Users/hannes/Documents/temp/supermario.nes" };
	} // namespace

	application::application(display& display, input_device_keyboard& keyboard)
		: display_{ display }
		, input_manager_{ keyboard }
		, console_{ sys::cartridge::from_file(file), display_ }
	{
		if (console_.get_status() != sys::status::success)
		{
			std::cerr << "Unable to load cartridge: " << to_string(console_.get_status()) << std::endl;
			std::abort();
		}
	}

	auto application::frame(std::chrono::microseconds const elapsed_time) -> void
	{
		console_.ref_controller_1().set_pressed(input_manager_.get_input_1().read_mask());
		console_.ref_controller_2().set_pressed(input_manager_.get_input_2().read_mask());

		console_.step(elapsed_time);

		if (console_.get_status() != sys::status::success)
		{
			std::cerr << "Invalid state: " << to_string(console_.get_status()) << std::endl;
			std::abort();
		}
	}
} // namespace nes::app