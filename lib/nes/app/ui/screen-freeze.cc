#include "nes/app/ui/screen-freeze.hh"
#include "nes/app/graphics/renderer.hh"

namespace nes::app
{
	auto screen_freeze::freeze(span<rgb, display::width * display::height> front_buffer) -> void
	{
		copy(front_buffer.get_data(), frozen_data_, front_buffer.get_length());
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