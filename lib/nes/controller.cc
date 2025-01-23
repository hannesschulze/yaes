#include "nes/controller.hh"

namespace nes
{
	auto controller::read() -> std::uint8_t
	{
		auto const res = index_ < 8 ? (pressed_.get_raw_value() >> index_) & 1 : 0;
		index_ += 1;
		if (strobing_) { index_ = 0; }
		return static_cast<std::uint8_t>(res);
	}

	auto controller::write(std::uint8_t const value) -> void
	{
		strobing_ = value & 1;
		if (strobing_) { index_ = 0; }
	}
} // namespace nes