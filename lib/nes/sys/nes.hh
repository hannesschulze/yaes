#pragma once

#include "nes/sys/types/cycle-count.hh"
#include "nes/sys/cartridge.hh"
#include "nes/sys/controller.hh"
#include "nes/sys/cpu.hh"
#include "nes/sys/ppu.hh"

namespace nes
{
	class display;
} // namespace nes

namespace nes::sys
{
	struct snapshot;

	/// The main console abstraction.
	class nes
	{
		cartridge cartridge_;
		display& display_;
		controller controller_1_;
		controller controller_2_;
		ppu ppu_;
		cpu cpu_;
		cycle_count current_cycles_;
		status status_{ status::success };

	public:
		explicit nes(cartridge, display&);

		nes(nes const&) = delete;
		nes(nes&&) = delete;
		auto operator=(nes const&) -> nes& = delete;
		auto operator=(nes&&) -> nes& = delete;

		auto get_status() const -> status { return status_; }
		auto get_controller_1() const -> controller const& { return controller_1_; }
		auto ref_controller_1() -> controller& { return controller_1_; }
		auto get_controller_2() const -> controller const& { return controller_2_; }
		auto ref_controller_2() -> controller& { return controller_2_; }

		auto step() -> void;
		auto step(std::chrono::microseconds delta) -> void;
		auto step(cycle_count delta) -> void;
		auto step_to_nmi() -> void;

		auto get_snapshot() -> snapshot;
	};
} // namespace nes::sys