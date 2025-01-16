#pragma once

#include "nes/util/cycle_count.hh"

namespace nes
{
	class ppu
	{
	public:
		explicit ppu() = default;

		ppu(ppu const&) = delete;
		ppu(ppu&&) = delete;
		auto operator=(ppu const&) -> ppu& = delete;
		auto operator=(ppu&&) -> ppu& = delete;

		auto step_to(cycle_count) -> void;
	};
} // namespace nes