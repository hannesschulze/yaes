#pragma once

#include "nes/app/ui/screen.hh"

namespace nes::app
{
	class input_manager;

	/// The main title screen for the emulator, used as an entrypoint for the UI.
	///
	/// Presents options to load games, adjust settings, and more.
	class screen_title final : public screen
	{
		input_manager& input_manager_;

	public:
		explicit screen_title(input_manager&);

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app