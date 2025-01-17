#include "nes/cpu.hh"
#include "nes/controller.hh"
#include "nes/mapper.hh"

namespace nes
{
	cpu::cpu(ppu& ppu, mapper& mapper, controller& controller_1, controller& controller_2)
		: ppu_{ ppu }
		, mapper_{ mapper }
		, controller_1_{ controller_1 }
		, controller_2_{ controller_2 }
	{
		registers_.pc = read16(address{ 0xFFFC });
	}

	auto cpu::snapshot(test::status& snapshot) -> void
	{
		snapshot.cpu_cycle = current_cycles_;
		snapshot.ram = std::vector(std::begin(ram_), std::end(ram_));
		snapshot.registers.pc = registers_.pc;
		snapshot.registers.sp = registers_.sp;
		snapshot.registers.a = registers_.a;
		snapshot.registers.x = registers_.x;
		snapshot.registers.y = registers_.y;
		snapshot.registers.p = registers_.p;
		snapshot.registers.c = registers_.c;
		snapshot.registers.z = registers_.z;
		snapshot.registers.i = registers_.i;
		snapshot.registers.d = registers_.d;
		snapshot.registers.b = registers_.b;
		snapshot.registers.v = registers_.v;
		snapshot.registers.n = registers_.n;
	}

	auto cpu::step_to(cycle_count const target) -> void
	{
		while (current_cycles_ < target)
		{
			step();
		}
	}

	auto cpu::step() -> void
	{
		// See https://www.nesdev.org/wiki/CPU_unofficial_opcodes

		auto const opcode = advance_pc8();
		switch (opcode)
		{
			case 0x00: run_brk(); break;
			case 0x01: run_ora<addressing_mode::indexed_indirect>(); break;
			case 0x02: run_stp(); break;
			case 0x03: run_slo<addressing_mode::indexed_indirect>(); break;
			case 0x04: run_nop<addressing_mode::zero_page>(); break;
			case 0x05: run_ora<addressing_mode::zero_page>(); break;
			case 0x06: run_asl<addressing_mode::zero_page>(); break;
			case 0x07: run_slo<addressing_mode::zero_page>(); break;
			case 0x08: run_php(); break;
			case 0x09: run_ora<addressing_mode::immediate>(); break;
			case 0x0A: run_asl<addressing_mode::accumulator>(); break;
			case 0x0B: run_anc<addressing_mode::immediate>(); break;
			case 0x0C: run_nop<addressing_mode::absolute>(); break;
			case 0x0D: run_ora<addressing_mode::absolute>(); break;
			case 0x0E: run_asl<addressing_mode::absolute>(); break;
			case 0x0F: run_slo<addressing_mode::absolute>(); break;
			case 0x10: run_bpl<addressing_mode::relative>(); break;
			case 0x11: run_ora<addressing_mode::indirect_indexed>(); break;
			case 0x12: run_stp(); break;
			case 0x13: run_slo<addressing_mode::indirect_indexed>(); break;
			case 0x14: run_nop<addressing_mode::zero_page_indexed_x>(); break;
			case 0x15: run_ora<addressing_mode::zero_page_indexed_x>(); break;
			case 0x16: run_asl<addressing_mode::zero_page_indexed_x>(); break;
			case 0x17: run_slo<addressing_mode::zero_page_indexed_x>(); break;
			case 0x18: run_clc(); break;
			case 0x19: run_ora<addressing_mode::absolute_indexed_y>(); break;
			case 0x1A: run_nop(); break;
			case 0x1B: run_slo<addressing_mode::absolute_indexed_y>(); break;
			case 0x1C: run_nop<addressing_mode::absolute_indexed_x>(); break;
			case 0x1D: run_ora<addressing_mode::absolute_indexed_x>(); break;
			case 0x1E: run_asl<addressing_mode::absolute_indexed_x>(); break;
			case 0x1F: run_slo<addressing_mode::absolute_indexed_x>(); break;
			case 0x20: run_jsr<addressing_mode::absolute>(); break;
			case 0x21: run_and<addressing_mode::indexed_indirect>(); break;
			case 0x22: run_stp(); break;
			case 0x23: run_rla<addressing_mode::indexed_indirect>(); break;
			case 0x24: run_bit<addressing_mode::zero_page>(); break;
			case 0x25: run_and<addressing_mode::zero_page>(); break;
			case 0x26: run_rol<addressing_mode::accumulator>(); break;
			case 0x27: run_rla<addressing_mode::zero_page>(); break;
			case 0x28: run_plp(); break;
			case 0x29: run_and<addressing_mode::immediate>(); break;
			case 0x2A: run_rol<addressing_mode::accumulator>(); break;
			case 0x2B: run_anc<addressing_mode::immediate>(); break;
			case 0x2C: run_bit<addressing_mode::absolute>(); break;
			case 0x2D: run_and<addressing_mode::absolute>(); break;
			case 0x2E: run_rol<addressing_mode::absolute>(); break;
			case 0x2F: run_rla<addressing_mode::absolute>(); break;
			case 0x30: run_bmi<addressing_mode::relative>(); break;
			case 0x31: run_and<addressing_mode::indirect_indexed>(); break;
			case 0x32: run_stp(); break;
			case 0x33: run_rla<addressing_mode::indirect_indexed>(); break;
			case 0x34: run_nop<addressing_mode::zero_page_indexed_x>(); break;
			case 0x35: run_and<addressing_mode::zero_page_indexed_x>(); break;
			case 0x36: run_rol<addressing_mode::zero_page_indexed_x>(); break;
			case 0x37: run_rla<addressing_mode::zero_page_indexed_x>(); break;
			case 0x38: run_sec(); break;
			case 0x39: run_and<addressing_mode::absolute_indexed_y>(); break;
			case 0x3A: run_nop(); break;
			case 0x3B: run_rla<addressing_mode::absolute_indexed_y>(); break;
			case 0x3C: run_nop<addressing_mode::absolute_indexed_x>(); break;
			case 0x3D: run_and<addressing_mode::absolute_indexed_x>(); break;
			case 0x3E: run_rol<addressing_mode::absolute_indexed_x>(); break;
			case 0x3F: run_rla<addressing_mode::absolute_indexed_x>(); break;
			case 0x40: run_rti(); break;
			case 0x41: run_eor<addressing_mode::indexed_indirect>(); break;
			case 0x42: run_stp(); break;
			case 0x43: run_sre<addressing_mode::indexed_indirect>(); break;
			case 0x44: run_nop<addressing_mode::zero_page>(); break;
			case 0x45: run_eor<addressing_mode::zero_page>(); break;
			case 0x46: run_lsr<addressing_mode::zero_page>(); break;
			case 0x47: run_sre<addressing_mode::zero_page>(); break;
			case 0x48: run_pha(); break;
			case 0x49: run_eor<addressing_mode::immediate>(); break;
			case 0x4A: run_lsr<addressing_mode::accumulator>(); break;
			case 0x4B: run_alr<addressing_mode::immediate>(); break;
			case 0x4C: run_jmp<addressing_mode::absolute>(); break;
			case 0x4D: run_eor<addressing_mode::absolute>(); break;
			case 0x4E: run_lsr<addressing_mode::absolute>(); break;
			case 0x4F: run_sre<addressing_mode::absolute>(); break;
			case 0x50: run_bvc<addressing_mode::relative>(); break;
			case 0x51: run_eor<addressing_mode::indirect_indexed>(); break;
			case 0x52: run_stp(); break;
			case 0x53: run_sre<addressing_mode::indirect_indexed>(); break;
			case 0x54: run_nop<addressing_mode::zero_page_indexed_x>(); break;
			case 0x55: run_eor<addressing_mode::zero_page_indexed_x>(); break;
			case 0x56: run_lsr<addressing_mode::zero_page_indexed_x>(); break;
			case 0x57: run_sre<addressing_mode::zero_page_indexed_x>(); break;
			case 0x58: run_cli(); break;
			case 0x59: run_eor<addressing_mode::absolute_indexed_y>(); break;
			case 0x5A: run_nop(); break;
			case 0x5B: run_sre<addressing_mode::absolute_indexed_y>(); break;
			case 0x5C: run_nop<addressing_mode::absolute_indexed_x>(); break;
			case 0x5D: run_eor<addressing_mode::absolute_indexed_x>(); break;
			case 0x5E: run_lsr<addressing_mode::absolute_indexed_x>(); break;
			case 0x5F: run_sre<addressing_mode::absolute_indexed_x>(); break;
			case 0x60: run_rts(); break;
			case 0x61: run_adc<addressing_mode::indexed_indirect>(); break;
			case 0x62: run_stp(); break;
			case 0x63: run_rra<addressing_mode::indexed_indirect>(); break;
			case 0x64: run_nop<addressing_mode::zero_page>(); break;
			case 0x65: run_adc<addressing_mode::zero_page>(); break;
			case 0x66: run_ror<addressing_mode::zero_page>(); break;
			case 0x67: run_rra<addressing_mode::zero_page>(); break;
			case 0x68: run_pla(); break;
			case 0x69: run_adc<addressing_mode::immediate>(); break;
			case 0x6A: run_ror<addressing_mode::accumulator>(); break;
			case 0x6B: run_arr<addressing_mode::immediate>(); break;
			case 0x6C: run_jmp<addressing_mode::indirect>(); break;
			case 0x6D: run_adc<addressing_mode::absolute>(); break;
			case 0x6E: run_ror<addressing_mode::absolute>(); break;
			case 0x6F: run_rra<addressing_mode::absolute>(); break;
			case 0x70: run_bvs<addressing_mode::relative>(); break;
			case 0x71: run_adc<addressing_mode::indirect_indexed>(); break;
			case 0x72: run_stp(); break;
			case 0x73: run_rra<addressing_mode::indirect_indexed>(); break;
			case 0x74: run_nop<addressing_mode::zero_page_indexed_x>(); break;
			case 0x75: run_adc<addressing_mode::zero_page_indexed_x>(); break;
			case 0x76: run_ror<addressing_mode::zero_page_indexed_x>(); break;
			case 0x77: run_rra<addressing_mode::zero_page_indexed_x>(); break;
			case 0x78: run_sei(); break;
			case 0x79: run_adc<addressing_mode::absolute_indexed_y>(); break;
			case 0x7A: run_nop(); break;
			case 0x7B: run_rra<addressing_mode::absolute_indexed_y>(); break;
			case 0x7C: run_nop<addressing_mode::absolute_indexed_x>(); break;
			case 0x7D: run_adc<addressing_mode::absolute_indexed_x>(); break;
			case 0x7E: run_ror<addressing_mode::absolute_indexed_x>(); break;
			case 0x7F: run_rra<addressing_mode::absolute_indexed_x>(); break;
			case 0x80: run_nop<addressing_mode::immediate>(); break;
			case 0x81: run_sta<addressing_mode::indexed_indirect>(); break;
			case 0x82: run_nop<addressing_mode::immediate>(); break;
			case 0x83: run_sax<addressing_mode::indexed_indirect>(); break;
			case 0x84: run_sty<addressing_mode::zero_page>(); break;
			case 0x85: run_sta<addressing_mode::zero_page>(); break;
			case 0x86: run_stx<addressing_mode::zero_page>(); break;
			case 0x87: run_sax<addressing_mode::zero_page>(); break;
			case 0x88: run_dey(); break;
			case 0x89: run_nop<addressing_mode::immediate>(); break;
			case 0x8A: run_txa(); break;
			case 0x8B: run_xaa<addressing_mode::immediate>(); break;
			case 0x8C: run_sty<addressing_mode::absolute>(); break;
			case 0x8D: run_sta<addressing_mode::absolute>(); break;
			case 0x8E: run_stx<addressing_mode::absolute>(); break;
			case 0x8F: run_sax<addressing_mode::absolute>(); break;
			case 0x90: run_bcc<addressing_mode::relative>(); break;
			case 0x91: run_sta<addressing_mode::indirect_indexed>(); break;
			case 0x92: run_stp(); break;
			case 0x93: run_ahx<addressing_mode::indirect_indexed>(); break;
			case 0x94: run_sty<addressing_mode::zero_page_indexed_x>(); break;
			case 0x95: run_sta<addressing_mode::zero_page_indexed_x>(); break;
			case 0x96: run_stx<addressing_mode::zero_page_indexed_y>(); break;
			case 0x97: run_sax<addressing_mode::zero_page_indexed_y>(); break;
			case 0x98: run_tya(); break;
			case 0x99: run_sta<addressing_mode::absolute_indexed_y>(); break;
			case 0x9A: run_txs(); break;
			case 0x9B: run_tas<addressing_mode::absolute_indexed_y>(); break;
			case 0x9C: run_shy<addressing_mode::absolute_indexed_x>(); break;
			case 0x9D: run_sta<addressing_mode::absolute_indexed_x>(); break;
			case 0x9E: run_shx<addressing_mode::absolute_indexed_y>(); break;
			case 0x9F: run_ahx<addressing_mode::absolute_indexed_y>(); break;
			case 0xA0: run_ldy<addressing_mode::immediate>(); break;
			case 0xA1: run_lda<addressing_mode::indexed_indirect>(); break;
			case 0xA2: run_ldx<addressing_mode::immediate>(); break;
			case 0xA3: run_lax<addressing_mode::indexed_indirect>(); break;
			case 0xA4: run_ldy<addressing_mode::zero_page>(); break;
			case 0xA5: run_lda<addressing_mode::zero_page>(); break;
			case 0xA6: run_ldx<addressing_mode::zero_page>(); break;
			case 0xA7: run_lax<addressing_mode::zero_page>(); break;
			case 0xA8: run_tay(); break;
			case 0xA9: run_lda<addressing_mode::immediate>(); break;
			case 0xAA: run_tax(); break;
			case 0xAB: run_lax<addressing_mode::immediate>(); break;
			case 0xAC: run_ldy<addressing_mode::absolute>(); break;
			case 0xAD: run_lda<addressing_mode::absolute>(); break;
			case 0xAE: run_ldx<addressing_mode::absolute>(); break;
			case 0xAF: run_lax<addressing_mode::absolute>(); break;
			case 0xB0: run_bcs<addressing_mode::relative>(); break;
			case 0xB1: run_lda<addressing_mode::indirect_indexed>(); break;
			case 0xB2: run_stp(); break;
			case 0xB3: run_lax<addressing_mode::indirect_indexed>(); break;
			case 0xB4: run_ldy<addressing_mode::zero_page_indexed_x>(); break;
			case 0xB5: run_lda<addressing_mode::zero_page_indexed_x>(); break;
			case 0xB6: run_ldx<addressing_mode::zero_page_indexed_y>(); break;
			case 0xB7: run_lax<addressing_mode::zero_page_indexed_y>(); break;
			case 0xB8: run_clv(); break;
			case 0xB9: run_lda<addressing_mode::absolute_indexed_y>(); break;
			case 0xBA: run_tsx(); break;
			case 0xBB: run_las<addressing_mode::absolute_indexed_y>(); break;
			case 0xBC: run_ldy<addressing_mode::absolute_indexed_x>(); break;
			case 0xBD: run_lda<addressing_mode::absolute_indexed_x>(); break;
			case 0xBE: run_ldx<addressing_mode::absolute_indexed_y>(); break;
			case 0xBF: run_lax<addressing_mode::absolute_indexed_y>(); break;
			case 0xC0: run_cpy<addressing_mode::immediate>(); break;
			case 0xC1: run_cmp<addressing_mode::indexed_indirect>(); break;
			case 0xC2: run_nop<addressing_mode::immediate>(); break;
			case 0xC3: run_dcp<addressing_mode::indexed_indirect>(); break;
			case 0xC4: run_cpy<addressing_mode::zero_page>(); break;
			case 0xC5: run_cmp<addressing_mode::zero_page>(); break;
			case 0xC6: run_dec<addressing_mode::zero_page>(); break;
			case 0xC7: run_dcp<addressing_mode::zero_page>(); break;
			case 0xC8: run_iny(); break;
			case 0xC9: run_cmp<addressing_mode::immediate>(); break;
			case 0xCA: run_dex(); break;
			case 0xCB: run_axs<addressing_mode::immediate>(); break;
			case 0xCC: run_cpy<addressing_mode::absolute>(); break;
			case 0xCD: run_cmp<addressing_mode::absolute>(); break;
			case 0xCE: run_dec<addressing_mode::absolute>(); break;
			case 0xCF: run_dcp<addressing_mode::absolute>(); break;
			case 0xD0: run_bne<addressing_mode::relative>(); break;
			case 0xD1: run_cmp<addressing_mode::indirect_indexed>(); break;
			case 0xD2: run_stp(); break;
			case 0xD3: run_dcp<addressing_mode::indirect_indexed>(); break;
			case 0xD4: run_nop<addressing_mode::zero_page_indexed_x>(); break;
			case 0xD5: run_cmp<addressing_mode::zero_page_indexed_x>(); break;
			case 0xD6: run_dec<addressing_mode::zero_page_indexed_x>(); break;
			case 0xD7: run_dcp<addressing_mode::zero_page_indexed_x>(); break;
			case 0xD8: run_cld(); break;
			case 0xD9: run_cmp<addressing_mode::absolute_indexed_y>(); break;
			case 0xDA: run_nop(); break;
			case 0xDB: run_dcp<addressing_mode::absolute_indexed_y>(); break;
			case 0xDC: run_nop<addressing_mode::absolute_indexed_x>(); break;
			case 0xDD: run_cmp<addressing_mode::absolute_indexed_x>(); break;
			case 0xDE: run_dec<addressing_mode::absolute_indexed_x>(); break;
			case 0xDF: run_dcp<addressing_mode::absolute_indexed_x>(); break;
			case 0xE0: run_cpx<addressing_mode::immediate>(); break;
			case 0xE1: run_sbc<addressing_mode::indexed_indirect>(); break;
			case 0xE2: run_nop<addressing_mode::immediate>(); break;
			case 0xE3: run_isc<addressing_mode::indexed_indirect>(); break;
			case 0xE4: run_cpx<addressing_mode::zero_page>(); break;
			case 0xE5: run_sbc<addressing_mode::zero_page>(); break;
			case 0xE6: run_inc<addressing_mode::zero_page>(); break;
			case 0xE7: run_isc<addressing_mode::zero_page>(); break;
			case 0xE8: run_inx(); break;
			case 0xE9: run_sbc<addressing_mode::immediate>(); break;
			case 0xEA: run_nop(); break;
			case 0xEB: run_sbc<addressing_mode::immediate>(); break;
			case 0xEC: run_cpx<addressing_mode::absolute>(); break;
			case 0xED: run_sbc<addressing_mode::absolute>(); break;
			case 0xEE: run_inc<addressing_mode::absolute>(); break;
			case 0xEF: run_isc<addressing_mode::absolute>(); break;
			case 0xF0: run_beq<addressing_mode::relative>(); break;
			case 0xF1: run_sbc<addressing_mode::indirect_indexed>(); break;
			case 0xF2: run_stp(); break;
			case 0xF3: run_isc<addressing_mode::indirect_indexed>(); break;
			case 0xF4: run_nop<addressing_mode::zero_page_indexed_x>(); break;
			case 0xF5: run_sbc<addressing_mode::zero_page_indexed_x>(); break;
			case 0xF6: run_inc<addressing_mode::zero_page_indexed_x>(); break;
			case 0xF7: run_isc<addressing_mode::zero_page_indexed_x>(); break;
			case 0xF8: run_sed(); break;
			case 0xF9: run_sbc<addressing_mode::absolute_indexed_y>(); break;
			case 0xFA: run_nop(); break;
			case 0xFB: run_isc<addressing_mode::absolute_indexed_y>(); break;
			case 0xFC: run_nop<addressing_mode::absolute_indexed_x>(); break;
			case 0xFD: run_sbc<addressing_mode::absolute_indexed_x>(); break;
			case 0xFE: run_inc<addressing_mode::absolute_indexed_x>(); break;
			case 0xFF: run_isc<addressing_mode::absolute_indexed_x>(); break;
			default: std::abort();
		}
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Instructions
	// -----------------------------------------------------------------------------------------------------------------

	auto cpu::run_brk() -> void
	{
		// TODO: Proper BRK handling
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ora() -> void
	{
		// ORA: Logical Inclusive OR
		auto operand = fetch_operand<Mode>();
		eval_ora(operand.read());
		current_cycles_ += operand.get_cycles();
	}

	auto cpu::run_stp() -> void
	{
		// TODO: Proper STP handling
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_slo() -> void
	{
		// SLO: ASL + ORA
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_asl(operand.read());
		operand.write(tmp);
		eval_ora(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_nop() -> void
	{
		// NOP: NOP
		auto operand = fetch_operand<Mode>();
		current_cycles_ += operand.get_cycles();
	}

	auto cpu::run_nop() -> void
	{
		// NOP: NOP
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_php() -> void
	{
		// PHP: Push Processor Status
		// Always set bits 4 and 5.
		push_stack8(registers_.p | 0b00110000);
		current_cycles_ += cycle_count::from_cpu(3);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_asl() -> void
	{
		// ASL: Arithmetic Shift Left
		auto operand = fetch_operand<Mode>();
		operand.write(eval_asl(operand.read()));
		current_cycles_ += shift_cycle_count<Mode>();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_anc() -> void
	{
		// ANC: AND #i + copy N to C
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bpl() -> void
	{
		// BPL: Break If Positive
		branch<Mode>(!registers_.n);
	}

	auto cpu::run_clc() -> void
	{
		// CLC: Clear Carry Flag
		registers_.c = false;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_jsr() -> void
	{
		// JSR: Jump to Subroutine
		static_assert(Mode == addressing_mode::absolute, "JSR only supports absolute addressing.");

		push_stack16(registers_.pc + 1);

		auto operand = fetch_operand<Mode>();
		registers_.pc = operand.get_address().get_absolute();
		current_cycles_ += cycle_count::from_cpu(6);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_and() -> void
	{
		// AND: Logical AND
		auto operand = fetch_operand<Mode>();
		eval_and(operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_rla() -> void
	{
		// RLA: ROL + AND
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_rol(operand.read());
		operand.write(tmp);
		eval_and(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bit() -> void
	{
		// BIT: Bit Test
		auto operand = fetch_operand<Mode>();
		auto const arg = operand.read();
		auto const res = (registers_.a & arg) != 0;
		registers_.z = !res;
		registers_.v = (arg & 0x40) != 0;
		registers_.n = (arg & 0x80) != 0;
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_rol() -> void
	{
		// ROR: Rotate Left
		auto operand = fetch_operand<Mode>();
		auto const result = eval_rol(operand.read());
		operand.write(result);
		current_cycles_ += shift_cycle_count<Mode>();
	}

	auto cpu::run_plp() -> void
	{
		// PLP: Pull Processor Status
		eval_plp();
		current_cycles_ += cycle_count::from_cpu(4);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bmi() -> void
	{
		branch<Mode>(registers_.n);
	}

	auto cpu::run_sec() -> void
	{
		// SEC: Set Carry Flag
		registers_.c = true;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_rti() -> void
	{
		// RTI: Return From Interrupt
		eval_plp();
		registers_.pc = pop_stack16();
		current_cycles_ += cycle_count::from_cpu(6);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_eor() -> void
	{
		// EOR: Exclusive OR
		auto operand = fetch_operand<Mode>();
		eval_eor(operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sre() -> void
	{
		// SRE: LSR + EOR
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_lsr(operand.read());
		operand.write(tmp);
		eval_eor(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_lsr() -> void
	{
		// LSR: Logical Shift Right
		auto operand = fetch_operand<Mode>();
		auto const old_val = operand.read();
		auto const new_val = static_cast<std::uint8_t>(old_val >> 1);
		operand.write(new_val);
		registers_.c = (old_val & 0x1) != 0;
		update_zn(new_val);
		current_cycles_ += shift_cycle_count<Mode>();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_alr() -> void
	{
		// ALR: AND #i + LSR A
		// TODO
		std::abort();
	}

	auto cpu::run_pha() -> void
	{
		// PHA: Push Accumulator
		push_stack8(registers_.a);
		current_cycles_ += cycle_count::from_cpu(3);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_jmp() -> void
	{
		// JMP: Jump
		auto operand = fetch_operand<Mode>();
		registers_.pc = operand.get_address().get_absolute();
		current_cycles_ += jump_cycle_count<Mode>();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bvc() -> void
	{
		// BVC: Branch If Overflow Clear
		branch<Mode>(!registers_.v);
	}

	auto cpu::run_cli() -> void
	{
		// CLI: Clear Interrupt Disable
		registers_.i = false;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_rts() -> void
	{
		// RTS: Return from Subroutine
		auto const addr = address{ pop_stack16() } + 1;
		registers_.pc = addr.get_absolute();
		current_cycles_ += cycle_count::from_cpu(6);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_adc() -> void
	{
		// ADC: Add With Carry
		auto operand = fetch_operand<Mode>();
		eval_adc(operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_rra() -> void
	{
		// RRA: ROR + ADC
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_ror(operand.read());
		operand.write(tmp);
		eval_adc(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ror() -> void
	{
		// ROR: Rotate Right
		auto operand = fetch_operand<Mode>();
		auto const result = eval_ror(operand.read());
		operand.write(result);
		current_cycles_ += shift_cycle_count<Mode>();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_arr() -> void
	{
		// ARR: AND #i + ROR A but with different flags
		// TODO
		std::abort();
	}

	auto cpu::run_pla() -> void
	{
		// PLA: Pull Accumulator
		registers_.a = pop_stack8();
		update_zn(registers_.a);
		current_cycles_ += cycle_count::from_cpu(4);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bvs() -> void
	{
		// BVS: Branch If Overflow Set
		branch<Mode>(registers_.v);
	}

	auto cpu::run_sei() -> void
	{
		// SEI: Set Interrupt Disable
		registers_.i = true;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sta() -> void
	{
		// STA: Store Accumulator
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		operand.write(registers_.a);
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ahx() -> void
	{
		// AHX: ???
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sax() -> void
	{
		// SAX: AND A X
		auto operand = fetch_operand<Mode>();
		auto const result = static_cast<std::uint8_t>(registers_.a & registers_.x);
		operand.write(result);
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sty() -> void
	{
		// STX: Store Y
		auto operand = fetch_operand<Mode>();
		operand.write(registers_.y);
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_stx() -> void
	{
		// STX: Store X
		auto operand = fetch_operand<Mode>();
		operand.write(registers_.x);
		current_cycles_ += operand.get_cycles();
	}

	auto cpu::run_dey() -> void
	{
		// DEY: Decrement Y Register
		registers_.y -= 1;
		update_zn(registers_.y);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_txa() -> void
	{
		// TXA: Transfer X to Accumulator
		registers_.a = registers_.x;
		update_zn(registers_.a);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_xaa() -> void
	{
		// XAA: Depends on analog behavior.
		run_brk();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bcc() -> void
	{
		// BCC: Branch If Carry Clear
		branch<Mode>(!registers_.c);
	}

	auto cpu::run_tya() -> void
	{
		// TYA: Transfer Y to Accumulator
		registers_.a = registers_.y;
		update_zn(registers_.a);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_txs() -> void
	{
		// TXS: Transfer X to Stack Pointer
		registers_.sp = registers_.x;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_tas() -> void
	{
		// TAS: ???
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_shy() -> void
	{
		// SHY: ???
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_shx() -> void
	{
		// SHY: ???
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ldy() -> void
	{
		// LDY: Load Y Register
		auto operand = fetch_operand<Mode>();
		registers_.y = operand.read();
		update_zn(registers_.y);
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_lda() -> void
	{
		// LDA: Load Accumulator
		auto operand = fetch_operand<Mode>();
		registers_.a = operand.read();
		update_zn(registers_.a);
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ldx() -> void
	{
		// LDX: Load X Register
		auto operand = fetch_operand<Mode>();
		registers_.x = operand.read();
		update_zn(registers_.x);
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_lax() -> void
	{
		// LAX: LDA + TAX
		auto operand = fetch_operand<Mode>();
		registers_.x = registers_.a = operand.read();
		update_zn(registers_.a);
		current_cycles_ += operand.get_cycles();
	}

	auto cpu::run_tay() -> void
	{
		// TAY: Transfer Accumulator to Y
		registers_.y = registers_.a;
		update_zn(registers_.y);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_tax() -> void
	{
		// TAX: Transfer Accumulator to X
		registers_.x = registers_.a;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bcs() -> void
	{
		// BCS: Branch If Carry Set
		branch<Mode>(registers_.c);
	}

	auto cpu::run_clv() -> void
	{
		// CLV: Clear Overflow Flag
		registers_.v = false;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_tsx() -> void
	{
		// TSX: Transfer Stack Pointer to X
		registers_.x = registers_.sp;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_las() -> void
	{
		// LAS: ???
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_cpy() -> void
	{
		// CPY: Compare Y Register
		auto operand = fetch_operand<Mode>();
		eval_cmp(registers_.y, operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_axs() -> void
	{
		// AXS: Set X to ((A AND X) - #value without borrow), updates NZC
		// TODO
		std::abort();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_cmp() -> void
	{
		// CMP: Compare
		auto operand = fetch_operand<Mode>();
		eval_cmp(registers_.a, operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_dcp() -> void
	{
		// DCP: DEC + CMP
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = static_cast<std::uint8_t>(operand.read() - 1);
		operand.write(tmp);
		eval_cmp(registers_.a, tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_dec() -> void
	{
		// DEC: Decrement Memory
		auto operand = fetch_operand<Mode>();
		auto const old_val = read8(operand.get_address());
		auto const new_val = static_cast<std::uint8_t>(old_val - 1);
		write8(operand.get_address(), new_val);
		update_zn(new_val);
		current_cycles_ += inc_dec_cycle_count<Mode>();
	}

	auto cpu::run_iny() -> void
	{
		// INY: Increment Y
		registers_.y += 1;
		update_zn(registers_.y);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	auto cpu::run_dex() -> void
	{
		// DEX: Decrement X Register
		registers_.x -= 1;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bne() -> void
	{
		// BNE: Branch If Not Equal
		branch<Mode>(!registers_.z);
	}

	auto cpu::run_cld() -> void
	{
		// CLD: Clear Decimal Mode
		registers_.d = false;
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_cpx() -> void
	{
		// CPX: Compare X Register
		auto operand = fetch_operand<Mode>();
		eval_cmp(registers_.x, operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sbc() -> void
	{
		// SBC: Subtract With Carry
		auto operand = fetch_operand<Mode>();
		eval_adc(~operand.read());
		current_cycles_ += operand.get_cycles();
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_isc() -> void
	{
		// ISC: INC + SBC
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = static_cast<std::uint8_t>(operand.read() + 1);
		operand.write(tmp);
		eval_adc(~tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_inc() -> void
	{
		// INC: Increment Memory
		auto operand = fetch_operand<Mode>();
		auto const old_val = read8(operand.get_address());
		auto const new_val = static_cast<std::uint8_t>(old_val + 1);
		write8(operand.get_address(), new_val);
		update_zn(new_val);
		current_cycles_ += inc_dec_cycle_count<Mode>();
	}

	auto cpu::run_inx() -> void
	{
		// INX: Increment X
		registers_.x += 1;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_beq() -> void
	{
		// BEQ: Branch If Equal
		branch<Mode>(registers_.z);
	}

	auto cpu::run_sed() -> void
	{
		// SED: Set Decimal Flag
		registers_.d = true;
		current_cycles_ += cycle_count::from_cpu(2);
	}
	
	// -----------------------------------------------------------------------------------------------------------------
	// Helpers
	// -----------------------------------------------------------------------------------------------------------------

	auto cpu::advance_pc8() -> std::uint8_t
	{
		auto const val = read8(address{ registers_.pc });
		registers_.pc += 1;
		return val;
	}

	auto cpu::advance_pc16() -> std::uint16_t
	{
		auto const val = read16(address{ registers_.pc });
		registers_.pc += 2;
		return val;
	}

	auto cpu::push_stack8(std::uint8_t const value) -> void
	{
		write8(stack_offset + registers_.sp, value);
		registers_.sp -= 1;
	}

	auto cpu::push_stack16(std::uint16_t const value) -> void
	{
		auto const low = static_cast<std::uint8_t>(value >> 0 & 0xFF);
		auto const high = static_cast<std::uint8_t>(value >> 8 & 0xFF);
		push_stack8(high);
		push_stack8(low);
	}

	auto cpu::pop_stack8() -> std::uint8_t
	{
		registers_.sp += 1;
		return read8(stack_offset + registers_.sp);
	}

	auto cpu::pop_stack16() -> std::uint16_t
	{
		auto const low = pop_stack8();
		auto const high = pop_stack8();
		return static_cast<std::uint16_t>((high << 8) | (low << 0));
	}

	auto cpu::update_zn(std::uint8_t const value) -> void
	{
		registers_.z = value == 0;
		registers_.n = (value & 0x80) != 0;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::branch(bool const condition) -> void
	{
		static_assert(Mode == addressing_mode::relative, "Branch should only be used with relative operands.");

		auto operand = fetch_operand<Mode>();
		if (condition)
		{
			registers_.pc = operand.get_address().get_absolute();
			current_cycles_ += operand.get_cycles();
		}
		else
		{
			current_cycles_ += cycle_count::from_cpu(2);
		}
	}

	auto cpu::eval_ror(std::uint8_t const arg) -> std::uint8_t
	{
		auto const res = static_cast<std::uint8_t>((registers_.c << 7) | (arg >> 1));
		registers_.c = (arg & 0x1) != 0;
		update_zn(res);
		return res;
	}

	auto cpu::eval_rol(std::uint8_t const arg) -> std::uint8_t
	{
		auto const res = static_cast<std::uint8_t>((arg << 1) | (registers_.c ? 1 : 0));
		registers_.c = (arg & 0x80) != 0;
		update_zn(res);
		return res;
	}

	auto cpu::eval_asl(std::uint8_t const arg) -> std::uint8_t
	{
		auto const new_val = static_cast<std::uint8_t>(arg << 1);
		registers_.c = (arg & 0x80) != 0;
		update_zn(new_val);
		return new_val;
	}

	auto cpu::eval_lsr(std::uint8_t const arg) -> std::uint8_t
	{
		auto const new_val = static_cast<std::uint8_t>(arg >> 1);
		registers_.c = (arg & 0x01) != 0;
		update_zn(new_val);
		return new_val;
	}

	auto cpu::eval_adc(std::uint8_t const arg) -> void
	{
		auto const old_val = registers_.a;
		auto const tmp = old_val + arg + (registers_.c ? 1 : 0);
		auto const new_val = static_cast<std::uint8_t>(tmp);

		registers_.a = new_val;
		registers_.c = tmp > 0xFF;
		registers_.v = ((old_val ^ arg) & 0x80) == 0 && ((old_val ^ new_val) & 0x80) != 0;
		update_zn(registers_.a);
	}

	auto cpu::eval_and(std::uint8_t const arg) -> void
	{
		registers_.a &= arg;
		update_zn(registers_.a);
	}

	auto cpu::eval_ora(std::uint8_t const arg) -> void
	{
		registers_.a |= arg;
		update_zn(registers_.a);
	}

	auto cpu::eval_eor(std::uint8_t const arg) -> void
	{
		registers_.a ^= arg;
		update_zn(registers_.a);
	}

	auto cpu::eval_cmp(std::uint8_t const a, std::uint8_t const b) -> void
	{
		update_zn(a - b);
		registers_.c = a >= b;
	}

	auto cpu::eval_plp() -> void
	{
		// Ignore bits 4 and 5
		registers_.p =
			(pop_stack8() & 0b11001111) |
			(registers_.p & 0b00110000);
	}


	// -----------------------------------------------------------------------------------------------------------------
	// Operands
	// -----------------------------------------------------------------------------------------------------------------

	//
	// See: https://www.nesdev.org/wiki/CPU_addressing_modes
	//

	template<cpu::addressing_mode Mode>
	auto cpu::fetch_operand(force_page_crossing const force_page_crossing) -> operand<Mode>
	{
		auto addr = address{};
		auto cycles = cycle_count{};

		switch (Mode)
		{
			case addressing_mode::zero_page:
			{
				addr = address{ advance_pc8() };
				cycles = cycle_count::from_cpu(3);
				break;
			}
			case addressing_mode::absolute:
			{
				addr = address{ advance_pc16() };
				cycles = cycle_count::from_cpu(4);
				break;
			}
			case addressing_mode::relative:
			{
				auto const offset = static_cast<std::int8_t>(advance_pc8());
				auto const base = address{ registers_.pc };
				addr = address{ static_cast<std::uint16_t>(registers_.pc + offset) };
				auto const page_crossing =
					base.get_page() != addr.get_page() || force_page_crossing == force_page_crossing::yes;
				cycles = cycle_count::from_cpu(page_crossing ? 4 : 3);
				break;
			}
			case addressing_mode::indirect:
			{
				auto const arg = address{ advance_pc16() };
				// Account for buggy indirect read: https://www.nesdev.org/wiki/Errata
				addr = arg.get_offset() == 0xFF
					? address{ read8(address{ arg.get_page(), 0x00 }), read8(arg) }
					: address{ read16(arg) };
				cycles = cycle_count::from_cpu(6);
				break;
			}
			case addressing_mode::zero_page_indexed_x:
			{
				// PEEK((arg + X) % 256)
				addr = address{ 0x00, static_cast<std::uint8_t>(advance_pc8() + registers_.x) };
				cycles = cycle_count::from_cpu(4);
				break;
			}
			case addressing_mode::zero_page_indexed_y:
			{
				// PEEK((arg + Y) % 256)
				addr = address{ 0x00, static_cast<std::uint8_t>(advance_pc8() + registers_.y) };
				cycles = cycle_count::from_cpu(4);
				break;
			}
			case addressing_mode::absolute_indexed_x:
			{
				// PEEK(arg + X)
				auto const arg = address{ advance_pc16() };
				addr = arg + registers_.x;
				auto const page_crossing =
					addr.get_page() != arg.get_page() || force_page_crossing == force_page_crossing::yes;
				cycles = cycle_count::from_cpu(page_crossing ? 5 : 4);
				break;
			}
			case addressing_mode::absolute_indexed_y:
			{
				// PEEK(arg + Y)
				auto const arg = address{ advance_pc16() };
				addr = arg + registers_.y;
				auto const page_crossing =
					addr.get_page() != arg.get_page() || force_page_crossing == force_page_crossing::yes;
				cycles = cycle_count::from_cpu(page_crossing ? 5 : 4);
				break;
			}
			case addressing_mode::indexed_indirect:
			{
				// PEEK(PEEK((arg + X) % 256) + PEEK((arg + X + 1) % 256) * 256)
				auto const arg = advance_pc8();
				auto const page = read8(address{ 0x00, static_cast<std::uint8_t>(arg + registers_.x + 1) });
				auto const offset = read8(address{ 0x00, static_cast<std::uint8_t>(arg + registers_.x) });
				addr = address{ page, offset };
				cycles = cycle_count::from_cpu(6);
				break;
			}
			case addressing_mode::indirect_indexed:
			{
				// PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
				auto const arg = advance_pc8();
				auto const page = read8(address{ 0x00, static_cast<std::uint8_t>(arg + 1) });
				auto const offset = read8(address{ 0x00, static_cast<std::uint8_t>(arg) });
				auto const base = address{ page, offset };
				addr = base + registers_.y;
				auto const page_crossing =
					addr.get_page() != base.get_page() || force_page_crossing == force_page_crossing::yes;
				cycles = cycle_count::from_cpu(page_crossing ? 6 : 5);
				break;
			}
		}

		return operand<Mode>{ *this, addr, cycles };
	}

	template<>
	auto cpu::fetch_operand<cpu::addressing_mode::accumulator>(force_page_crossing)
		-> operand<addressing_mode::accumulator>
	{
		return operand<addressing_mode::accumulator>{ *this };
	}

	template<>
	auto cpu::fetch_operand<cpu::addressing_mode::immediate>(force_page_crossing)
		-> operand<addressing_mode::immediate>
	{
		return operand<addressing_mode::immediate>{ *this, advance_pc8() };
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Memory Access
	// -----------------------------------------------------------------------------------------------------------------

	//
	// See: https://www.nesdev.org/wiki/CPU_memory_map
	//

	auto cpu::read8(address const addr) -> std::uint8_t
	{
		if (addr <= address{ 0x1FFF }) { return ram_[addr.get_absolute() % ram_size]; }
		if (addr <= address{ 0x3FFF }) { return 0x0; } // TODO: PPU registers
		if (addr <= address{ 0x4013 }) { return 0x0; } // TODO: APU registers
		if (addr == address{ 0x4014 }) { return 0x0; } // TODO: PPU registers
		if (addr == address{ 0x4015 }) { return 0x0; } // TODO: APU registers
		if (addr == address{ 0x4016 }) { return controller_1_.read(); }
		if (addr == address{ 0x4017 }) { return controller_2_.read(); }
		if (addr <= address{ 0x401F }) { return 0x0; }
		return mapper_.read(addr);
	}

	auto cpu::read16(address const addr) -> std::uint16_t
	{
		auto const low = read8(addr + 0);
		auto const high = read8(addr + 1);
		return static_cast<std::uint16_t>((high << 8) | (low << 0));
	}

	auto cpu::write8(address const addr, std::uint8_t const value) -> void
	{
		if (addr <= address{ 0x1FFF }) { ram_[addr.get_absolute() % ram_size] = value; return; }
		if (addr <= address{ 0x3FFF }) { return; } // TODO: PPU registers
		if (addr <= address{ 0x4013 }) { return; } // TODO: APU registers
		if (addr == address{ 0x4014 }) { return; } // TODO: PPU registers
		if (addr == address{ 0x4015 }) { return; } // TODO: APU registers
		if (addr == address{ 0x4016 }) { controller_1_.write(value); return; }
		if (addr == address{ 0x4017 }) { controller_2_.write(value); return; }
		if (addr <= address{ 0x401F }) { return; }
		mapper_.write(addr, value);
	}

	auto cpu::write16(address const addr, std::uint16_t const value) -> void
	{
		auto const low = static_cast<std::uint8_t>(value >> 0 & 0xFF);
		auto const high = static_cast<std::uint8_t>(value >> 8 & 0xFF);
		write8(addr + 0, low);
		write8(addr + 1, high);
	}

} // namespace nes