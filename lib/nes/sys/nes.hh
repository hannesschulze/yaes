#pragma once

#include "nes/sys/types/cycle-count.hh"
#include "nes/sys/types/snapshot.hh"
#include "nes/sys/cartridge.hh"
#include "nes/sys/controller.hh"
#include "nes/sys/cpu.hh"
#include "nes/sys/ppu.hh"
#include "nes/common/containers/span.hh"

namespace nes
{
	class display;
} // namespace nes

namespace nes::sys
{
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
		status status_{ status::error_invalid_ines_data };

	public:
		explicit nes(display&, span<u8 const> rom_data);

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
		auto step(cycle_count delta) -> void;
		auto step_to_nmi() -> void;

#ifdef NES_ENABLE_SNAPSHOTS
		auto get_snapshot() -> snapshot;
#endif
	};
} // namespace nes::sys