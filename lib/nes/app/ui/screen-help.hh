#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/app/ui/selection.hh"

namespace nes::app
{
	class input_manager;

	/// Screen used to configure various emulator options.
	class screen_help final : public screen
	{
		static constexpr auto page_size = u32{ 8 };

		input_manager& input_manager_;

	public:
		explicit screen_help(input_manager&);

		auto render(renderer&) -> void override;
		auto process_events() -> action override;
	};
} // namespace nes::app