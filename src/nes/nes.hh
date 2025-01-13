#pragma once

#include "nes/cartridge.hh"

namespace nes
{
	class display;

	/// The main console abstraction.
	class nes
	{
		cartridge cartridge_;
		display& display_;

	public:
		explicit nes(cartridge, display&);

		nes(nes const&) = delete;
		nes(nes&&) = delete;
		auto operator=(nes const&) -> nes& = delete;
		auto operator=(nes&&) -> nes& = delete;
	};
} // namespace nes