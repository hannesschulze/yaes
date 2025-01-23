#include "nes/nes.hh"
#include "nes/mapper.hh"
#include "nes/util/display.hh"
#include "nes/util/rgb.hh"
#include "nes/util/snapshot.hh"
#include <iostream>

namespace nes
{
	nes::nes(cartridge cartridge, display& display)
		: cartridge_{ std::move(cartridge) }
		, display_{ display }
		, mapper_{ mapper::select(cartridge_) }
		, ppu_{ cpu_, mapper_, display_ }
		, cpu_{ ppu_, mapper_, controller_1_, controller_2_ }
	{
		if (!cartridge_.is_valid())
		{
			std::cerr << "Cartridge not valid!" << std::endl;
			std::abort();
		}

		display_.clear(rgb{});
		display_.switch_buffers();
		display_.clear(rgb{});
	}

	auto nes::step(std::chrono::microseconds const delta) -> void
	{
		step(cycle_count::from_duration(delta));
	}

	auto nes::step() -> void
	{
		cpu_.step();
		while (ppu_.get_cycles() < cpu_.get_cycles())
		{
			ppu_.step();
		}
	}

	auto nes::step(cycle_count const delta) -> void
	{
		current_cycles_ += delta;
		while (cpu_.get_cycles() < current_cycles_)
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

	auto nes::get_snapshot() -> snapshot
	{
		auto res = snapshot{};
		res.sram = std::vector(cartridge_.get_ram(), cartridge_.get_ram() + cartridge_.get_ram_length());
		cpu_.build_snapshot(res);
		ppu_.build_snapshot(res);
		return res;
	}
} // namespace nes