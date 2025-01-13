#pragma once

namespace nes
{
	class cpu
	{
	public:
		explicit cpu() = default;

		cpu(cpu const&) = delete;
		cpu(cpu&&) = delete;
		auto operator=(cpu const&) -> cpu& = delete;
		auto operator=(cpu&&) -> cpu& = delete;
	};
} // namespace nes