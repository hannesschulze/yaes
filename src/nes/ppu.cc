#include "nes/ppu.hh"

namespace nes
{
	ppu::ppu(cpu& cpu, mapper& mapper)
		: cpu_{ cpu }
		, mapper_{ mapper }
	{
	}

	auto ppu::step_to(cycle_count const cycle) -> void
	{
		while (current_cycles_ < cycle)
		{
			current_cycles_ += cycle_count::from_ppu(1);
		}
	}
} // namespace nes