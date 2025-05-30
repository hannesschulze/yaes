#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/common/containers/string-view.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// An error popup.
	class screen_error final : public screen
	{
		input_device_keyboard& keyboard_;
		string_view message_;
		status error_{ status::success };
		action action_{};

	public:
		explicit screen_error(input_device_keyboard&);

		auto get_message() const -> string_view { return message_; }
		auto get_error() const -> status { return error_; }
		auto get_action() const -> action { return action_; }
		auto set_message(string_view const v) -> void { message_ = v; }
		auto set_error(status const v) -> void { error_ = v; }
		auto set_action(action const& v) -> void { action_ = v; }

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app