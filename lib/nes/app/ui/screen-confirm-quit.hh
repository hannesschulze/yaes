#pragma once

#include "nes/app/ui/screen.hh"

namespace nes::app
{
	class input_device_keyboard;

	/// A popup asking the user if they really want to quit the game.
	class screen_confirm_quit final : public screen
	{
		input_device_keyboard& keyboard_;
		bool confirm_{ false };

	public:
		explicit screen_confirm_quit(input_device_keyboard&);

		auto get_confirm() const -> bool { return confirm_; }
		auto set_confirm(bool const v) -> void { confirm_ = v; }

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app