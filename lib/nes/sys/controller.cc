#include "nes/sys/controller.hh"

namespace nes::sys
{
	auto controller::read() -> u8
	{
		auto const res = index_ < 8 ? (pressed_.get_raw_value() >> index_) & 1 : 0;
		index_ += 1;
		if (strobing_) { index_ = 0; }
		return static_cast<u8>(res);
	}

	auto controller::write(u8 const value) -> void
	{
		strobing_ = value & 1;
		if (strobing_) { index_ = 0; }
	}
} // namespace nes::sys