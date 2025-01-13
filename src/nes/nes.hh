#pragma once

#include "nes/cartridge.hh"
#include "nes/controller.hh"
#include "nes/cpu.hh"
#include "nes/ppu.hh"

namespace nes
{
	class display;

	/// The main console abstraction.
	class nes
	{
		cartridge cartridge_;
		display& display_;
		controller controller_1_;
		controller controller_2_;
		cpu cpu_;
		ppu ppu_;

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

		auto step(std::uint64_t delta_ms) -> void;
	};
} // namespace nes