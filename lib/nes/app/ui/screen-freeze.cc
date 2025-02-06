#include "nes/app/ui/screen-freeze.hh"
#include "nes/app/graphics/renderer.hh"

namespace nes::app
{
	auto screen_freeze::freeze(display const& disp) -> void
	{
		for (auto y = u32{ 0 }; y < display::height; ++y)
		{
			for (auto x = u32{ 0 }; x < display::width; ++x)
			{
				frozen_data_[y * display::width + x] = disp.get(x, y);
			}
		}
	}

	auto screen_freeze::render(renderer& renderer) -> void
	{
		auto& disp = renderer.get_display();
		for (auto y = u32{ 0 }; y < display::height; ++y)
		{
			for (auto x = u32{ 0 }; x < display::width; ++x)
			{
				disp.set(x, y, frozen_data_[y * display::width + x]);
			}
		}
	}

	auto screen_freeze::process_events() -> action
	{
		return action{};
	}
} // namespace nes::app