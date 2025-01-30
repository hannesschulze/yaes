#pragma once

#include "nes/app/input/input-manager.hh"
#include "nes/sys/nes.hh"
#include <chrono>

namespace nes
{
	class display;

	/// Entrypoint for the application.
	///
	/// This class takes care of presenting a UI screen or the emulator, handling any errors and implementing the main
	/// application logic.
	///
	/// It delegates some platform-specific tasks to interfaces.
	class application
	{
		display& display_;
		input_manager input_manager_;
		nes console_;

	public:
		explicit application(display&, input_device_keyboard&);

		application(application const&) = delete;
		application(application&&) = delete;
		auto operator=(application const&) -> application& = delete;
		auto operator=(application&&) -> application& = delete;

		/// Request a new frame for the application.
		auto frame(std::chrono::microseconds elapsed_time) -> void;

		auto add_controller(input_device_controller& c) -> void { input_manager_.add_controller(c); }
		auto remove_controller(input_device_controller& c) -> void { input_manager_.remove_controller(c); }
	};
} // namespace nes