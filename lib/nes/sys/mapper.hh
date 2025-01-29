#pragma once

#include "nes/sys/types/address.hh"
#include "nes/sys/types/status.hh"

namespace nes
{
	class cartridge;

	class mapper
	{
	public:
		static auto invalid() -> mapper&;
		static auto get(std::uint8_t number) -> mapper&;

		virtual ~mapper() = default;

		mapper(mapper const&) = delete;
		mapper(mapper&&) = delete;
		auto operator=(mapper const&) -> mapper& = delete;
		auto operator=(mapper&&) -> mapper& = delete;

		virtual auto validate(cartridge&) -> status = 0;
		virtual auto read_cpu(address, cartridge&) -> std::uint8_t = 0;
		virtual auto write_cpu(address, std::uint8_t, cartridge&) -> void = 0;
		virtual auto read_ppu(address, cartridge&, std::uint8_t const* vram) -> std::uint8_t = 0;
		virtual auto write_ppu(address, std::uint8_t, cartridge&, std::uint8_t* vram) -> void = 0;

	protected:
		explicit mapper() = default;
	};
} // namespace nes