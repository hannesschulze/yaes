#include "nes/nes.hh"
#include "nes/mapper.hh"
#include "nes/display.hh"
#include "nes/util/rgb.hh"
#include <iostream>

namespace nes
{
	nes::nes(cartridge cartridge, display& display)
		: cartridge_{ std::move(cartridge) }
		, display_{ display }
		, mapper_{ mapper::select(cartridge_) }
		, cpu_{ ppu_, mapper_, controller_1_, controller_2_ }
	{
		if (!cartridge_.is_valid())
		{
			std::cerr << "Cartridge not valid!" << std::endl;
			std::abort();
		}

		display_.clear(rgb{ animation_progress_, animation_progress_, animation_progress_ });
		display_.switch_buffers();
	}

	auto nes::step(std::chrono::microseconds const delta) -> void
	{
		current_cycles_ += cycle_count::from_duration(delta);
		cpu_.step_to(current_cycles_);
		ppu_.step_to(current_cycles_);

		animation_progress_ -= 2;
		display_.clear(rgb{ animation_progress_, animation_progress_, animation_progress_ });
		display_.switch_buffers();
	}
} // namespace nes