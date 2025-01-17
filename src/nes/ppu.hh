#pragma once

#include "nes/util/cycle_count.hh"

namespace nes
{
	class cpu;
	class mapper;

	class ppu
	{
		static constexpr auto vram_size = std::size_t{ 0x800 };

		cycle_count current_cycles_;
		cpu& cpu_;
		mapper& mapper_;
		std::uint8_t vram_[vram_size]{};

	public:
		explicit ppu(cpu& cpu, mapper& mapper);

		ppu(ppu const&) = delete;
		ppu(ppu&&) = delete;
		auto operator=(ppu const&) -> ppu& = delete;
		auto operator=(ppu&&) -> ppu& = delete;

		auto step_to(cycle_count) -> void;
	};
} // namespace nes