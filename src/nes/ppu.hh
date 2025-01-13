#pragma once

namespace nes
{
	class ppu
	{
	public:
		explicit ppu() = default;

		ppu(ppu const&) = delete;
		ppu(ppu&&) = delete;
		auto operator=(ppu const&) -> ppu& = delete;
		auto operator=(ppu&&) -> ppu& = delete;
	};
} // namespace nes