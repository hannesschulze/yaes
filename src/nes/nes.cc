#include "nes/nes.hh"
#include "nes/display.hh"
#include "nes/util/rgb.hh"
#include <iostream>

namespace nes
{
	nes::nes(cartridge cartridge, display& display)
		: cartridge_{ std::move(cartridge) }
		, display_{ display }
	{
		if (!cartridge_.is_valid())
		{
			std::cerr << "Cartridge not valid!" << std::endl;
			std::abort();
		}

		display.clear(rgb::from_hex(0xFF0000));
		display.switch_buffers();
	}

	auto nes::step(std::uint64_t delta_ms) -> void
	{
		std::cout << "Step: " << delta_ms << "ms\n";
	}
} // namespace nes