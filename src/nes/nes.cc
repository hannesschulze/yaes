#include "nes/nes.hh"
#include "nes/display.hh"
#include "nes/util/rgb.hh"

namespace nes
{
	nes::nes(display& display)
		: display_{ display }
	{
		display.clear(rgb::from_hex(0xFF0000));
		display.switch_buffers();
	}
} // namespace nes