#pragma once

#include "nes/util/address.hh"

namespace nes
{
	class cartridge;

	class mapper
	{
		using read_func = std::uint8_t(mapper::*)(address);
		using write_func = void(mapper::*)(address, std::uint8_t);

		cartridge& cartridge_;
		read_func read_;
		write_func write_;

	public:
		static auto select(cartridge&) -> mapper;

		mapper(mapper const&) = delete;
		mapper(mapper&&) = delete;
		auto operator=(mapper const&) -> mapper& = delete;
		auto operator=(mapper&&) -> mapper& = delete;

		auto read(address const addr) -> std::uint8_t
		{
			return (this->*read_)(addr);
		}

		auto write(address const addr, std::uint8_t const value) -> void
		{
			(this->*write_)(addr, value);
		}

	private:
		explicit mapper(cartridge& cartridge, read_func read, write_func write);

		// Implementations

		auto nrom_read(address) -> std::uint8_t;
		auto nrom_write(address, std::uint8_t) -> void;
	};
} // namespace nes