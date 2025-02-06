#pragma once

#include "nes/app/input/input-manager.hh"
#include "nes/app/ui/screen-title.hh"
#include "nes/app/ui/screen-browser.hh"
#include "nes/app/ui/screen-freeze.hh"
#include "nes/app/ui/screen-error.hh"
#include "nes/app/ui/screen-confirm-quit.hh"
#include "nes/sys/nes.hh"
#include "nes/common/box.hh"
#include "nes/common/display.hh"
#include "nes/common/types.hh"
#include <chrono>

namespace nes::app
{
	class screen;

	/// Entrypoint for the application.
	///
	/// This class takes care of presenting a UI screen or the emulator, handling any errors and implementing the main
	/// application logic.
	///
	/// It delegates some platform-specific tasks to interfaces.
	class application
	{
		/// Acts like a regular display, but renders a screen on top of the back buffer when requested to switch
		/// buffers.
		class display_proxy final : public display
		{
		public:
			display& base;
			screen* popup{ nullptr };
			screen* screen{ nullptr };

			explicit display_proxy(display& base)
				: base{ base }
			{
			}

			auto get(u32 const x, u32 const y) const -> rgb override { return base.get(x, y); }
			auto set(u32 const x, u32 const y, rgb const value) -> void override { base.set(x, y, value); }
			auto switch_buffers() -> void override;
		};

		display_proxy display_;
		input_manager input_manager_;
		file_browser& file_browser_;
		box<sys::nes> console_{};
		screen_title screen_title_;
		screen_browser screen_browser_;
		screen_freeze screen_freeze_;
		screen_error screen_error_;
		screen_confirm_quit screen_confirm_quit_;

	public:
		explicit application(display&, input_device_keyboard&, file_browser& file_browser);

		application(application const&) = delete;
		application(application&&) = delete;
		auto operator=(application const&) -> application& = delete;
		auto operator=(application&&) -> application& = delete;

		/// Request a new frame for the application.
		auto frame(std::chrono::microseconds elapsed_time) -> void;

		auto add_controller(input_device_controller& c) -> void { input_manager_.add_controller(c); }
		auto remove_controller(input_device_controller& c) -> void { input_manager_.remove_controller(c); }

	private:
		auto handle_action(action const&) -> void;
		auto show_error(std::string_view message, status error, action const& action = action::close_popup()) -> void;
	};
} // namespace nes::app