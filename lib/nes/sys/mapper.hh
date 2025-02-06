#pragma once

#include "nes/sys/types/address.hh"
#include "nes/common/containers/span.hh"
#include "nes/common/status.hh"
#include "nes/common/types.hh"

namespace nes::sys
{
	class cartridge;

	class mapper
	{
	public:
		static auto invalid() -> mapper&;
		static auto get(u8 number) -> mapper&;

		virtual ~mapper() = default;

		mapper(mapper const&) = delete;
		mapper(mapper&&) = delete;
		auto operator=(mapper const&) -> mapper& = delete;
		auto operator=(mapper&&) -> mapper& = delete;

		virtual auto validate(cartridge&) -> status = 0;
		virtual auto read_cpu(address, cartridge&) -> u8 = 0;
		virtual auto write_cpu(address, u8, cartridge&) -> void = 0;
		virtual auto read_ppu(address, cartridge&, span<u8 const> vram) -> u8 = 0;
		virtual auto write_ppu(address, u8, cartridge&, span<u8> vram) -> void = 0;

	protected:
		explicit mapper() = default;
	};
} // namespace nes::sys