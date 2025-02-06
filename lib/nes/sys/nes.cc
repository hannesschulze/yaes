#include "nes/sys/nes.hh"

namespace nes::sys
{
	nes::nes(display& display, span<u8 const> const rom_data)
		: cartridge_{ rom_data }
		, display_{ display }
		, ppu_{ cpu_, cartridge_, display_ }
		, cpu_{ ppu_, cartridge_, controller_1_, controller_2_ }
		, status_{ cartridge_.get_status() }
	{
	}

	auto nes::step() -> void
	{
		if (get_status() != status::success) { return; }

		status_ = cpu_.step();
		while (ppu_.get_cycles() < cpu_.get_cycles())
		{
			ppu_.step();
		}
	}

	auto nes::step(cycle_count const delta) -> void
	{
		current_cycles_ += delta;
		while (cpu_.get_cycles() < current_cycles_ && get_status() == status::success)
		{
			step();
		}
	}

	auto nes::step_to_nmi() -> void
	{
		while (cpu_.is_nmi_pending())
		{
			step();
		}
		while (!cpu_.is_nmi_pending())
		{
			step();
		}
	}

#ifdef NES_ENABLE_SNAPSHOTS
	auto nes::get_snapshot() -> snapshot
	{
		auto res = snapshot{};
		res.sram = std::vector(cartridge_.get_ram().begin(), cartridge_.get_ram().end());
		cpu_.build_snapshot(res);
		ppu_.build_snapshot(res);
		return res;
	}
#endif
} // namespace nes::sys