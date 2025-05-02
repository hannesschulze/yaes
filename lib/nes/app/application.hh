#pragma once

#include "nes/app/input/input-manager.hh"
#include "nes/app/ui/screen-title.hh"
#include "nes/app/ui/screen-browser.hh"
#include "nes/app/ui/screen-settings.hh"
#include "nes/app/ui/screen-help.hh"
#include "nes/app/ui/screen-freeze.hh"
#include "nes/app/ui/screen-error.hh"
#include "nes/app/ui/screen-confirm-quit.hh"
#include "nes/app/ui/screen-prompt-key.hh"
#include "nes/app/ui/screen-file-viewer.hh"
#include "nes/app/preferences.hh"
#include "nes/sys/nes.hh"
#include "nes/common/containers/box.hh"
#include "nes/common/display.hh"
#include "nes/common/fps-counter.hh"
#include "nes/common/types.hh"

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
			preferences& preferences_;
			rgb buffers_[2][display::width * display::height]{};
			span<rgb, display::width * display::height> buffer_front_ = buffers_[0];
			span<rgb, display::width * display::height> buffer_back_ = buffers_[1];

		public:
			fps_counter fps;
			display& base;
			screen* visible_popup{ nullptr };
			screen* visible_screen{ nullptr };

			explicit display_proxy(preferences& preferences, display& base)
				: preferences_{ preferences }
				, base{ base }
			{
			}

			auto set(u32 const x, u32 const y, rgb const value) -> void override;
			auto get_front() const -> span<rgb, display::width * display::height>;

			auto switch_buffers() -> void override;
		};

		input_manager input_manager_;
		preferences preferences_;
		file_browser& file_browser_;
		display_proxy display_;
		box<sys::nes> console_{};
		screen_title screen_title_;
		screen_browser screen_browser_;
		screen_settings screen_settings_;
		screen_help screen_help_;
		screen_freeze screen_freeze_;
		screen_error screen_error_;
		screen_confirm_quit screen_confirm_quit_;
		screen_prompt_key screen_prompt_key_;
		screen_file_viewer screen_file_viewer_;

	public:
		explicit application(display&, input_device_keyboard&, file_browser&);

		application(application const&) = delete;
		application(application&&) = delete;
		auto operator=(application const&) -> application& = delete;
		auto operator=(application&&) -> application& = delete;

		/// Request a new frame for the application.
		auto frame(u32 elapsed_time_us) -> void;

		auto add_controller(input_device_controller& c) -> void { input_manager_.add_controller(c); }
		auto remove_controller(input_device_controller& c) -> void { input_manager_.remove_controller(c); }

	private:
		auto handle_action(action const&) -> void;
		auto show_error(string_view message, status error, action const& action = action::close_popup()) -> void;
		auto go_to_screen(screen*) -> void;
	};
} // namespace nes::app
