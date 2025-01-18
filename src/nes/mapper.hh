#pragma once

#include "nes/util/address.hh"

namespace nes
{
	class cartridge;

	class mapper
	{
		using read_cpu_func = std::uint8_t(mapper::*)(address);
		using write_cpu_func = void(mapper::*)(address, std::uint8_t);
		using read_ppu_func = std::uint8_t(mapper::*)(address, std::uint8_t const* vram);
		using write_ppu_func = void(mapper::*)(address, std::uint8_t* vram, std::uint8_t);

		cartridge& cartridge_;
		read_cpu_func read_cpu_;
		write_cpu_func write_cpu_;
		read_ppu_func read_ppu_;
		write_ppu_func write_ppu_;

	public:
		static auto select(cartridge&) -> mapper;

		mapper(mapper const&) = delete;
		mapper(mapper&&) = delete;
		auto operator=(mapper const&) -> mapper& = delete;
		auto operator=(mapper&&) -> mapper& = delete;

		auto read_cpu(address const addr) -> std::uint8_t
		{
			return (this->*read_cpu_)(addr);
		}

		auto write_cpu(address const addr, std::uint8_t const value) -> void
		{
			(this->*write_cpu_)(addr, value);
		}

		auto read_ppu(address const addr, std::uint8_t const* vram) -> std::uint8_t
		{
			return (this->*read_ppu_)(addr, vram);
		}

		auto write_ppu(address const addr, std::uint8_t* vram, std::uint8_t const value) -> void
		{
			(this->*write_ppu_)(addr, vram, value);
		}

	private:
		explicit mapper(
			cartridge& cartridge,
			read_cpu_func read_cpu, write_cpu_func write_cpu,
			read_ppu_func read_ppu, write_ppu_func write_ppu);

		// Implementations

		auto nrom_read_cpu(address) -> std::uint8_t;
		auto nrom_write_cpu(address, std::uint8_t) -> void;
		auto nrom_read_ppu(address, std::uint8_t const* vram) -> std::uint8_t;
		auto nrom_write_ppu(address, std::uint8_t* vram, std::uint8_t) -> void;
		auto nrom_mirrored_vram_address(address) const -> unsigned;
	};
} // namespace nes