#pragma once

#include "nes/sys/types/cycle-count.hh"
#include "nes/sys/types/address.hh"
#include "nes/sys/types/snapshot.hh"
#include "nes/common/status.hh"
#include "nes/common/types.hh"

namespace nes::sys
{
	class ppu;
	class cpu;
	class controller;
	class cartridge;

	namespace detail
	{
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

		template<detail::addressing_mode Mode>
		class operand;
		template<detail::addressing_mode Mode>
		auto fetch_operand(cpu&, force_page_crossing = force_page_crossing::no) -> operand<Mode>;
	} // namespace detail

	class cpu
	{
		template<detail::addressing_mode Mode>
		friend class detail::operand;
		template<detail::addressing_mode Mode>
		friend auto detail::fetch_operand(cpu&, detail::force_page_crossing) -> detail::operand<Mode>;

		static constexpr auto ram_size = u32{ 0x800 };
		static constexpr auto stack_offset = address{ 0x100 };

		cycle_count current_cycles_;
		ppu& ppu_;
		cartridge& cartridge_;
		controller& controller_1_;
		controller& controller_2_;
		u8 ram_[ram_size]{};
		bool nmi_pending_{ false };

		// Registers
		struct
		{
			u16 pc{ 0 };
			u8 sp{ 0xFD };
			u8 a{ 0 };
			u8 x{ 0 };
			u8 y{ 0 };
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

				u8 value{ 0b00100100 };
			} p{};
		} registers_{};

	public:
		explicit cpu(ppu&, cartridge&, controller& controller_1, controller& controller_2);

		cpu(cpu const&) = delete;
		cpu(cpu&&) = delete;
		auto operator=(cpu const&) -> cpu& = delete;
		auto operator=(cpu&&) -> cpu& = delete;

		auto get_cycles() const -> cycle_count { return current_cycles_; }
#ifdef NES_ENABLE_SNAPSHOTS
		auto build_snapshot(snapshot&) -> void;
#endif
		auto stall_cycles(cycle_count) -> void;
		auto trigger_nmi() -> void;
		auto step() -> status;
		auto is_nmi_pending() const -> bool { return nmi_pending_; } // XXX: Debugging

		// Memory access

		auto read8(address) -> u8;
		auto read16(address) -> u16;
		auto write8(address, u8) -> void;
		auto write16(address, u16) -> void;

	private:
		// Instructions

#define DEFINE_SIMPLE_INSTRUCTION(name) \
	auto run_##name() -> status;
#define DEFINE_OPERAND_INSTRUCTION(name) \
	template<detail::addressing_mode Mode> auto run_##name() -> status;

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

		auto advance_pc8() -> u8;
		auto advance_pc16() -> u16;
		auto push_stack8(u8) -> void;
		auto push_stack16(u16) -> void;
		auto pop_stack8() -> u8;
		auto pop_stack16() -> u16;
		auto update_zn(u8 value) -> void;
		template<detail::addressing_mode Mode>
		auto branch(bool condition) -> void;
		auto execute_interrupt(address) -> void;

		auto eval_ror(u8 arg) -> u8;
		auto eval_rol(u8 arg) -> u8;
		auto eval_asl(u8 arg) -> u8;
		auto eval_lsr(u8 arg) -> u8;
		auto eval_adc(u8 arg) -> void;
		auto eval_and(u8 arg) -> void;
		auto eval_ora(u8 arg) -> void;
		auto eval_eor(u8 arg) -> void;
		auto eval_cmp(u8 a, u8 b) -> void;
		auto eval_plp() -> void;
		auto eval_php() -> void;
	};

	namespace detail
	{
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

			auto read() -> u8 { return cpu_.read8(address_); }
			auto write(u8 const value) -> void { cpu_.write8(address_, value); }
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

			auto read() -> u8 { return cpu_.registers_.a; }
			auto write(u8 const value) -> void { cpu_.registers_.a = value; }
			auto get_cycles() -> cycle_count { return cycle_count::from_cpu(2); }
		};

		template<>
		class operand<addressing_mode::immediate>
		{
			u8 value_{ 0 };

		public:
			explicit operand(u8 const value)
				: value_{ value }
			{
			}

			auto read() -> u8 { return value_; }
			auto get_cycles() -> cycle_count { return cycle_count::from_cpu(2); }
		};

		template<addressing_mode Mode>
		auto fetch_operand(cpu&, force_page_crossing) -> operand<Mode>;
		template<>
		auto fetch_operand<addressing_mode::accumulator>(cpu&, force_page_crossing) -> operand<addressing_mode::accumulator>;
		template<>
		auto fetch_operand<addressing_mode::immediate>(cpu&, force_page_crossing) -> operand<addressing_mode::immediate>;
		
#define DEFINE_CYCLE_COUNT(name) \
	template<addressing_mode Mode> static auto name##_cycle_count() -> cycle_count;
#define DEFINE_CYCLE_COUNT_INSTANCE(name, mode, value) \
	template<> \
	[[maybe_unused]] auto name##_cycle_count<addressing_mode::mode>() -> cycle_count { return cycle_count::from_cpu(value); }

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
	} // namespace detail
} // namespace nes::sys