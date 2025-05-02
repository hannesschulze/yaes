#include "nes/app/application.hh"
#include "nes/app/input/input-device-keyboard.hh"
#include "nes/app/ui/screen.hh"
#include "nes/app/graphics/renderer.hh"
#include "nes/app/action.hh"
#include "nes/app/crypto/aes256.hh"
#include "nes/app/crypto/sha256.hh"

namespace nes::app
{
	auto application::display_proxy::get_front() const -> span<rgb, display::width * display::height>
	{
		return buffer_front_;
	}

	auto application::display_proxy::set(u32 const x, u32 const y, rgb const value) -> void
	{
		buffer_back_[y * display::width + x] = value;
		base.set(x, y, value);
	}

	auto application::display_proxy::switch_buffers() -> void
	{
		auto r = renderer{ base };

		if (visible_screen)
		{
			visible_screen->render(r);
		}

		if (visible_popup)
		{
			visible_popup->render(r);
		}

		if (preferences_.get_fps_counter())
		{
			r.render_rect(30, 29, 2, 1, color::fixed_black);
			auto const attrs = text_attributes{}
				.set_max_width(2)
				.set_alignment(text_alignment::right);
			r.render_text_format(32, 29, color::fixed_white, attrs, "{}", fps.get_fps());
		}

		swap(buffer_front_, buffer_back_);
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
		, screen_prompt_key_{ keyboard }
		, screen_file_viewer_{ keyboard, file_browser }
	{
		display_.visible_screen = &screen_title_;
	}

	auto application::frame(u32 const elapsed_time_us) -> void
	{
		display_.fps.frame(elapsed_time_us);

		if (display_.visible_popup)
		{
			auto const a = display_.visible_popup->process_events();
			handle_action(a);
		}
		else if (display_.visible_screen)
		{
			auto const a = display_.visible_screen->process_events();
			handle_action(a);
		}

		if (display_.visible_screen)
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
					screen_freeze_.freeze(display_.get_front());
					screen_confirm_quit_.set_confirm(false);
					display_.visible_screen = &screen_freeze_;
					display_.visible_popup = &screen_confirm_quit_;
					display_.switch_buffers();
					return;
				}
			}

			// Forward current input state to the NES.
			console_->ref_controller_1().set_pressed(input_manager_.get_input_1().read_buttons());
			console_->ref_controller_2().set_pressed(input_manager_.get_input_2().read_buttons());

			// The scene is rendered by the console after the PPU requests a new frame (thus calling
			// display_proxy::switch_buffers).
			console_->step(sys::cycle_count::from_microseconds(elapsed_time_us));

			if (console_->get_status() != status::success)
			{
				screen_freeze_.freeze(display_.get_front());
				display_.visible_screen = &screen_freeze_;
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
				display_.visible_popup = nullptr;
				break;
			}
			case action::type::cancel_quit:
			{
				display_.visible_popup = nullptr;
				display_.visible_screen = nullptr;
				break;
			}
			case action::type::prompt_key:
			{
				screen_prompt_key_.reset(a.get_file_name());
				display_.visible_popup = &screen_prompt_key_;
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

				if (!a.get_key().is_empty())
				{
					auto const key = span{ reinterpret_cast<u8 const*>(a.get_key().get_data()), a.get_key().get_length() };
					auto const key_hash = sha256::hash(key);
					aes256::decrypt(span{ buffer, length }, key_hash.get_data());
				}

				console_.emplace(display_, span<u8 const>{ buffer, length });
				if (console_->get_status() != status::success)
				{
					show_error("Unable to load cartridge", console_->get_status());
					console_.clear();
					break;
				}

				display_.visible_screen = nullptr;
				display_.visible_popup = nullptr;
				break;
			}
			case action::type::view_file:
			{
				if (auto const s = screen_file_viewer_.load(a.get_file_name()); s != status::success)
				{
					show_error("Unable to open file", s);
				}
				else
				{
					display_.visible_popup = &screen_file_viewer_;
				}
				break;
			}
			case action::type::show_error:
			{
				show_error(a.get_message(), a.get_error());
				break;
			}
		}
	}

	auto application::show_error(string_view const message, status const error, action const& action) -> void
	{
		screen_error_.set_message(message);
		screen_error_.set_error(error);
		screen_error_.set_action(action);
		display_.visible_popup = &screen_error_;
	}

	auto application::go_to_screen(screen* screen) -> void
	{
		console_.clear();
		display_.visible_screen = screen;
		display_.visible_popup = nullptr;
	}
} // namespace nes::app
