#pragma once

#include "nes/util/cycle-count.hh"
#include "nes/cartridge.hh"
#include "nes/controller.hh"
#include "nes/cpu.hh"
#include "nes/ppu.hh"
#include "nes/mapper.hh"

namespace nes
{
	class display;
	struct snapshot;

	/// The main console abstraction.
	class nes
	{
		cartridge cartridge_;
		display& display_;
		controller controller_1_;
		controller controller_2_;
		mapper mapper_;
		ppu ppu_;
		cpu cpu_;
		cycle_count current_cycles_;

	public:
		explicit nes(cartridge, display&);

		nes(nes const&) = delete;
		nes(nes&&) = delete;
		auto operator=(nes const&) -> nes& = delete;
		auto operator=(nes&&) -> nes& = delete;

		auto get_controller_1() const -> controller const& { return controller_1_; }
		auto get_controller_1() -> controller& { return controller_1_; }
		auto get_controller_2() const -> controller const& { return controller_2_; }
		auto get_controller_2() -> controller& { return controller_2_; }

		auto step() -> void;
		auto step(std::chrono::microseconds delta) -> void;
		auto step(cycle_count delta) -> void;
		auto step_to_nmi() -> void;

		auto get_snapshot() -> snapshot;
	};
} // namespace nes