#pragma once

#include "nes/app/ui/screen.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// An error popup.
	class screen_error final : public screen
	{
		input_device_keyboard& keyboard_;
		std::string_view message_;
		status error_{ status::success };

	public:
		explicit screen_error(input_device_keyboard&);

		auto get_message() const -> std::string_view { return message_; }
		auto get_error() const -> status { return error_; }
		auto set_message(std::string_view const v) { message_ = v; }
		auto set_error(status const v) { error_ = v; }

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app