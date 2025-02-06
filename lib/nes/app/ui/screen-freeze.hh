#pragma once

#include "nes/app/ui/screen.hh"
#include "nes/common/rgb.hh"
#include "nes/common/display.hh"

namespace nes::app
{
	/// Freeze screen which preserves the display buffer at a certain state.
	///
	/// This is usually used to preserve the image from the console when the game should not continue.
	class screen_freeze final : public screen
	{
		rgb frozen_data_[display::width * display::height]{};

	public:
		explicit screen_freeze() = default;

		auto render(renderer&) -> void override;
		auto process_events() -> action override;

		/// Update the image shown to the back buffer of the given display.
		auto freeze(display const&) -> void;
	};
} // namespace nes::app