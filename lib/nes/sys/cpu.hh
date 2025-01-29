#pragma once

#include "nes/sys/types/cycle-count.hh"
#include "nes/sys/types/address.hh"
#include "nes/sys/types/status.hh"

namespace nes
{
	class ppu;
	class controller;
	class cartridge;
	struct snapshot;

	class cpu
	{
		static constexpr auto ram_size = std::size_t{ 0x800 };
		static constexpr auto stack_offset = address{ 0x100 };

		cycle_count current_cycles_;
		ppu& ppu_;
		cartridge& cartridge_;
		controller& controller_1_;
		controller& controller_2_;
		std::uint8_t ram_[ram_size]{};
		bool nmi_pending_{ false };

		// Registers
		struct
		{
			std::uint16_t pc{ 0 };
			std::uint8_t sp{ 0xFD };
			std::uint8_t a{ 0 };
			std::uint8_t x{ 0 };
			std::uint8_t y{ 0 };
			struct
			{
				auto get_c() const -> bool { return value & 0b00000001; } // carry
				auto get_z() const -> bool { return value & 0b00000010; } // zero
				auto get_i() const -> bool { return value & 0b00000100; } // interrupt inhibit
				auto get_d() const -> bool { return value & 0b00001000; } // decimal
				auto get_b() const -> bool { return value & 0b00010000; } // break
				auto get_v() const -> bool { return value & 0b01000000; } // overflow
				auto get_n() const -> bool { return value & 0b10000000; } // negative

				auto set_c(bool const v) -> void { value = (value & ~0b00000001) | (v ? 0b00000001 : 0); }
				auto set_z(bool const v) -> void { value = (value & ~0b00000010) | (v ? 0b00000010 : 0); }
				auto set_i(bool const v) -> void { value = (value & ~0b00000100) | (v ? 0b00000100 : 0); }
				auto set_d(bool const v) -> void { value = (value & ~0b00001000) | (v ? 0b00001000 : 0); }
				auto set_b(bool const v) -> void { value = (value & ~0b00010000) | (v ? 0b00010000 : 0); }
				auto set_v(bool const v) -> void { value = (value & ~0b01000000) | (v ? 0b01000000 : 0); }
				auto set_n(bool const v) -> void { value = (value & ~0b10000000) | (v ? 0b10000000 : 0); }

				std::uint8_t value{ 0b00100100 };
			} p{};
		} registers_{};

		enum class addressing_mode
		{
			accumulator,          // A
			immediate,            // #v
			zero_page,            // d
			absolute,             // a
			relative,             // *+d
			indirect,             // (a)
			zero_page_indexed_x,  // d,x
			zero_page_indexed_y,  // d,y
			absolute_indexed_x,   // a,x
			absolute_indexed_y,   // a,y
			indexed_indirect,     // (d,x)
			indirect_indexed,     // (d),y
		};

		enum class force_page_crossing
		{
			no,
			yes,
		};

		template<addressing_mode>
		class operand
		{
			cpu& cpu_;
			address address_;
			cycle_count cycles_;

		public:
			explicit operand(cpu& cpu, address const address, cycle_count const cycles)
				: cpu_{ cpu }
				, address_{ address }
				, cycles_{ cycles }
			{
			}

			auto read() -> std::uint8_t { return cpu_.read8(address_); }
			auto write(std::uint8_t const value) -> void { cpu_.write8(address_, value); }
			auto get_address() -> address { return address_; }
			auto get_cycles() -> cycle_count { return cycles_; }
		};

		template<>
		class operand<addressing_mode::accumulator>
		{
			cpu& cpu_;

		public:
			explicit operand(cpu& cpu)
				: cpu_{ cpu }
			{
			}

			auto read() -> std::uint8_t { return cpu_.registers_.a; }
			auto write(std::uint8_t const value) -> void { cpu_.registers_.a = value; }
			auto get_cycles() -> cycle_count { return cycle_count::from_cpu(2); }
		};

		template<>
		class operand<addressing_mode::immediate>
		{
			std::uint8_t value_{ 0 };

		public:
			explicit operand(std::uint8_t const value)
				: value_{ value }
			{
			}

			auto read() -> std::uint8_t { return value_; }
			auto get_cycles() -> cycle_count { return cycle_count::from_cpu(2); }
		};

	public:
		explicit cpu(ppu&, cartridge&, controller& controller_1, controller& controller_2);

		cpu(cpu const&) = delete;
		cpu(cpu&&) = delete;
		auto operator=(cpu const&) -> cpu& = delete;
		auto operator=(cpu&&) -> cpu& = delete;

		auto get_cycles() const -> cycle_count { return current_cycles_; }
		auto build_snapshot(snapshot&) -> void;
		auto stall_cycles(cycle_count) -> void;
		auto trigger_nmi() -> void;
		auto step() -> status;
		auto is_nmi_pending() const -> bool { return nmi_pending_; } // XXX: Debugging

		// Memory access

		auto read8(address) -> std::uint8_t;
		auto read16(address) -> std::uint16_t;
		auto write8(address, std::uint8_t) -> void;
		auto write16(address, std::uint16_t) -> void;

	private:
		// Instructions

#define DEFINE_SIMPLE_INSTRUCTION(name) \
	auto run_##name() -> status;
#define DEFINE_OPERAND_INSTRUCTION(name) \
	template<addressing_mode Mode> auto run_##name() -> status;

		DEFINE_OPERAND_INSTRUCTION(brk)
		DEFINE_OPERAND_INSTRUCTION(ora)
		DEFINE_SIMPLE_INSTRUCTION(stp)
		DEFINE_OPERAND_INSTRUCTION(slo)
		DEFINE_OPERAND_INSTRUCTION(nop)
		DEFINE_SIMPLE_INSTRUCTION(nop)
		DEFINE_SIMPLE_INSTRUCTION(php)
		DEFINE_OPERAND_INSTRUCTION(asl)
		DEFINE_OPERAND_INSTRUCTION(anc)
		DEFINE_OPERAND_INSTRUCTION(bpl)
		DEFINE_SIMPLE_INSTRUCTION(clc)
		DEFINE_OPERAND_INSTRUCTION(jsr)
		DEFINE_OPERAND_INSTRUCTION(and)
		DEFINE_OPERAND_INSTRUCTION(rla)
		DEFINE_OPERAND_INSTRUCTION(bit)
		DEFINE_OPERAND_INSTRUCTION(rol)
		DEFINE_SIMPLE_INSTRUCTION(plp)
		DEFINE_OPERAND_INSTRUCTION(bmi)
		DEFINE_SIMPLE_INSTRUCTION(sec)
		DEFINE_SIMPLE_INSTRUCTION(rti)
		DEFINE_OPERAND_INSTRUCTION(eor)
		DEFINE_OPERAND_INSTRUCTION(sre)
		DEFINE_OPERAND_INSTRUCTION(lsr)
		DEFINE_OPERAND_INSTRUCTION(alr)
		DEFINE_SIMPLE_INSTRUCTION(pha)
		DEFINE_OPERAND_INSTRUCTION(jmp)
		DEFINE_OPERAND_INSTRUCTION(bvc)
		DEFINE_SIMPLE_INSTRUCTION(cli)
		DEFINE_SIMPLE_INSTRUCTION(rts)
		DEFINE_OPERAND_INSTRUCTION(adc)
		DEFINE_OPERAND_INSTRUCTION(rra)
		DEFINE_OPERAND_INSTRUCTION(ror)
		DEFINE_OPERAND_INSTRUCTION(arr)
		DEFINE_SIMPLE_INSTRUCTION(pla)
		DEFINE_OPERAND_INSTRUCTION(bvs)
		DEFINE_SIMPLE_INSTRUCTION(sei)
		DEFINE_OPERAND_INSTRUCTION(sta)
		DEFINE_OPERAND_INSTRUCTION(ahx)
		DEFINE_OPERAND_INSTRUCTION(sax)
		DEFINE_OPERAND_INSTRUCTION(sty)
		DEFINE_OPERAND_INSTRUCTION(stx)
		DEFINE_SIMPLE_INSTRUCTION(dey)
		DEFINE_SIMPLE_INSTRUCTION(txa)
		DEFINE_OPERAND_INSTRUCTION(xaa)
		DEFINE_OPERAND_INSTRUCTION(bcc)
		DEFINE_SIMPLE_INSTRUCTION(tya)
		DEFINE_SIMPLE_INSTRUCTION(txs)
		DEFINE_OPERAND_INSTRUCTION(tas)
		DEFINE_OPERAND_INSTRUCTION(shy)
		DEFINE_OPERAND_INSTRUCTION(shx)
		DEFINE_OPERAND_INSTRUCTION(ldy)
		DEFINE_OPERAND_INSTRUCTION(lda)
		DEFINE_OPERAND_INSTRUCTION(ldx)
		DEFINE_OPERAND_INSTRUCTION(lax)
		DEFINE_SIMPLE_INSTRUCTION(tay)
		DEFINE_SIMPLE_INSTRUCTION(tax)
		DEFINE_OPERAND_INSTRUCTION(bcs)
		DEFINE_SIMPLE_INSTRUCTION(clv)
		DEFINE_SIMPLE_INSTRUCTION(tsx)
		DEFINE_OPERAND_INSTRUCTION(las)
		DEFINE_OPERAND_INSTRUCTION(cpy)
		DEFINE_OPERAND_INSTRUCTION(axs)
		DEFINE_OPERAND_INSTRUCTION(cmp)
		DEFINE_OPERAND_INSTRUCTION(dcp)
		DEFINE_OPERAND_INSTRUCTION(dec)
		DEFINE_SIMPLE_INSTRUCTION(iny)
		DEFINE_SIMPLE_INSTRUCTION(dex)
		DEFINE_OPERAND_INSTRUCTION(bne)
		DEFINE_SIMPLE_INSTRUCTION(cld)
		DEFINE_OPERAND_INSTRUCTION(cpx)
		DEFINE_OPERAND_INSTRUCTION(sbc)
		DEFINE_OPERAND_INSTRUCTION(isc)
		DEFINE_OPERAND_INSTRUCTION(inc)
		DEFINE_SIMPLE_INSTRUCTION(inx)
		DEFINE_OPERAND_INSTRUCTION(beq)
		DEFINE_SIMPLE_INSTRUCTION(sed)

#undef DEFINE_SIMPLE_INSTRUCTION
#undef DEFINE_OPERAND_INSTRUCTION

		// Helpers

		auto advance_pc8() -> std::uint8_t;
		auto advance_pc16() -> std::uint16_t;
		auto push_stack8(std::uint8_t) -> void;
		auto push_stack16(std::uint16_t) -> void;
		auto pop_stack8() -> std::uint8_t;
		auto pop_stack16() -> std::uint16_t;
		auto update_zn(std::uint8_t value) -> void;
		template<addressing_mode Mode>
		auto branch(bool condition) -> void;
		auto execute_interrupt(address) -> void;

		auto eval_ror(std::uint8_t arg) -> std::uint8_t;
		auto eval_rol(std::uint8_t arg) -> std::uint8_t;
		auto eval_asl(std::uint8_t arg) -> std::uint8_t;
		auto eval_lsr(std::uint8_t arg) -> std::uint8_t;
		auto eval_adc(std::uint8_t arg) -> void;
		auto eval_and(std::uint8_t arg) -> void;
		auto eval_ora(std::uint8_t arg) -> void;
		auto eval_eor(std::uint8_t arg) -> void;
		auto eval_cmp(std::uint8_t a, std::uint8_t b) -> void;
		auto eval_plp() -> void;
		auto eval_php() -> void;

		// Operands

#define DEFINE_CYCLE_COUNT(name) \
	template<addressing_mode Mode> static auto name##_cycle_count() -> cycle_count;
#define DEFINE_CYCLE_COUNT_INSTANCE(name, mode, value) \
	template<> auto name##_cycle_count<addressing_mode::mode>() -> cycle_count { return cycle_count::from_cpu(value); }

		DEFINE_CYCLE_COUNT(jump)
		DEFINE_CYCLE_COUNT_INSTANCE(jump, absolute, 3)
		DEFINE_CYCLE_COUNT_INSTANCE(jump, indirect, 5)

		DEFINE_CYCLE_COUNT(shift)
		DEFINE_CYCLE_COUNT_INSTANCE(shift, accumulator, 2)
		DEFINE_CYCLE_COUNT_INSTANCE(shift, zero_page, 5)
		DEFINE_CYCLE_COUNT_INSTANCE(shift, zero_page_indexed_x, 6)
		DEFINE_CYCLE_COUNT_INSTANCE(shift, absolute, 6)
		DEFINE_CYCLE_COUNT_INSTANCE(shift, absolute_indexed_x, 7)

		DEFINE_CYCLE_COUNT(inc_dec)
		DEFINE_CYCLE_COUNT_INSTANCE(inc_dec, zero_page, 5)
		DEFINE_CYCLE_COUNT_INSTANCE(inc_dec, zero_page_indexed_x, 6)
		DEFINE_CYCLE_COUNT_INSTANCE(inc_dec, absolute, 6)
		DEFINE_CYCLE_COUNT_INSTANCE(inc_dec, absolute_indexed_x, 7)

#undef DEFINE_CYCLE_COUNT
#undef DEFINE_CYCLE_COUNT_INSTANCE

		template<addressing_mode Mode>
		auto fetch_operand(force_page_crossing = force_page_crossing::no) -> operand<Mode>;
		template<>
		auto fetch_operand<addressing_mode::accumulator>(force_page_crossing) -> operand<addressing_mode::accumulator>;
		template<>
		auto fetch_operand<addressing_mode::immediate>(force_page_crossing) -> operand<addressing_mode::immediate>;
	};
} // namespace nes