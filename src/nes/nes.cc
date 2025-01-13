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
} // namespace nes