#pragma once

namespace nes
{
	class display;

	class nes
	{
		display& display_;

	public:
		explicit nes(display&);

		nes(nes const&) = delete;
		nes(nes&&) = delete;
		auto operator=(nes const&) -> nes& = delete;
		auto operator=(nes&&) -> nes& = delete;
	};
} // namespace nes