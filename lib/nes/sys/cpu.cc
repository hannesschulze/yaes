#include "nes/sys/cpu.hh"
#include "nes/sys/ppu.hh"
#include "nes/sys/controller.hh"
#include "nes/sys/cartridge.hh"
#include "nes/sys/types/snapshot.hh"

namespace nes::sys
{
	cpu::cpu(ppu& ppu, cartridge& cartridge, controller& controller_1, controller& controller_2)
		: ppu_{ ppu }
		, cartridge_{ cartridge }
		, controller_1_{ controller_1 }
		, controller_2_{ controller_2 }
	{
		registers_.pc = read16(address{ 0xFFFC });
	}

#ifdef NES_ENABLE_SNAPSHOTS
	auto cpu::build_snapshot(snapshot& snapshot) -> void
	{
		snapshot.cpu_cycle = current_cycles_;
		snapshot.ram = std::vector(std::begin(ram_), std::end(ram_));
		snapshot.registers.pc = registers_.pc;
		snapshot.registers.sp = registers_.sp;
		snapshot.registers.a = registers_.a;
		snapshot.registers.x = registers_.x;
		snapshot.registers.y = registers_.y;
		snapshot.registers.p = registers_.p.value;
		snapshot.registers.c = registers_.p.get_c();
		snapshot.registers.z = registers_.p.get_z();
		snapshot.registers.i = registers_.p.get_i();
		snapshot.registers.d = registers_.p.get_d();
		snapshot.registers.b = registers_.p.get_b();
		snapshot.registers.v = registers_.p.get_v();
		snapshot.registers.n = registers_.p.get_n();
	}
#endif

	auto cpu::stall_cycles(cycle_count const count) -> void
	{
		current_cycles_ += count;
	}

	auto cpu::trigger_nmi() -> void
	{
		nmi_pending_ = true;
	}

	auto cpu::step() -> status
	{
		// See https://www.nesdev.org/wiki/CPU_unofficial_opcodes

#ifdef NES_ENABLE_PPU
		if (nmi_pending_)
		{
			execute_interrupt(address{ 0xFFFA });
			nmi_pending_ = false;
		}
#endif

		auto const opcode = advance_pc8();
		switch (opcode)
		{
			case 0x00: return run_brk<addressing_mode::immediate>();
			case 0x01: return run_ora<addressing_mode::indexed_indirect>();
			case 0x02: return run_stp();
			case 0x03: return run_slo<addressing_mode::indexed_indirect>();
			case 0x04: return run_nop<addressing_mode::zero_page>();
			case 0x05: return run_ora<addressing_mode::zero_page>();
			case 0x06: return run_asl<addressing_mode::zero_page>();
			case 0x07: return run_slo<addressing_mode::zero_page>();
			case 0x08: return run_php();
			case 0x09: return run_ora<addressing_mode::immediate>();
			case 0x0A: return run_asl<addressing_mode::accumulator>();
			case 0x0B: return run_anc<addressing_mode::immediate>();
			case 0x0C: return run_nop<addressing_mode::absolute>();
			case 0x0D: return run_ora<addressing_mode::absolute>();
			case 0x0E: return run_asl<addressing_mode::absolute>();
			case 0x0F: return run_slo<addressing_mode::absolute>();
			case 0x10: return run_bpl<addressing_mode::relative>();
			case 0x11: return run_ora<addressing_mode::indirect_indexed>();
			case 0x12: return run_stp();
			case 0x13: return run_slo<addressing_mode::indirect_indexed>();
			case 0x14: return run_nop<addressing_mode::zero_page_indexed_x>();
			case 0x15: return run_ora<addressing_mode::zero_page_indexed_x>();
			case 0x16: return run_asl<addressing_mode::zero_page_indexed_x>();
			case 0x17: return run_slo<addressing_mode::zero_page_indexed_x>();
			case 0x18: return run_clc();
			case 0x19: return run_ora<addressing_mode::absolute_indexed_y>();
			case 0x1A: return run_nop();
			case 0x1B: return run_slo<addressing_mode::absolute_indexed_y>();
			case 0x1C: return run_nop<addressing_mode::absolute_indexed_x>();
			case 0x1D: return run_ora<addressing_mode::absolute_indexed_x>();
			case 0x1E: return run_asl<addressing_mode::absolute_indexed_x>();
			case 0x1F: return run_slo<addressing_mode::absolute_indexed_x>();
			case 0x20: return run_jsr<addressing_mode::absolute>();
			case 0x21: return run_and<addressing_mode::indexed_indirect>();
			case 0x22: return run_stp();
			case 0x23: return run_rla<addressing_mode::indexed_indirect>();
			case 0x24: return run_bit<addressing_mode::zero_page>();
			case 0x25: return run_and<addressing_mode::zero_page>();
			case 0x26: return run_rol<addressing_mode::zero_page>();
			case 0x27: return run_rla<addressing_mode::zero_page>();
			case 0x28: return run_plp();
			case 0x29: return run_and<addressing_mode::immediate>();
			case 0x2A: return run_rol<addressing_mode::accumulator>();
			case 0x2B: return run_anc<addressing_mode::immediate>();
			case 0x2C: return run_bit<addressing_mode::absolute>();
			case 0x2D: return run_and<addressing_mode::absolute>();
			case 0x2E: return run_rol<addressing_mode::absolute>();
			case 0x2F: return run_rla<addressing_mode::absolute>();
			case 0x30: return run_bmi<addressing_mode::relative>();
			case 0x31: return run_and<addressing_mode::indirect_indexed>();
			case 0x32: return run_stp();
			case 0x33: return run_rla<addressing_mode::indirect_indexed>();
			case 0x34: return run_nop<addressing_mode::zero_page_indexed_x>();
			case 0x35: return run_and<addressing_mode::zero_page_indexed_x>();
			case 0x36: return run_rol<addressing_mode::zero_page_indexed_x>();
			case 0x37: return run_rla<addressing_mode::zero_page_indexed_x>();
			case 0x38: return run_sec();
			case 0x39: return run_and<addressing_mode::absolute_indexed_y>();
			case 0x3A: return run_nop();
			case 0x3B: return run_rla<addressing_mode::absolute_indexed_y>();
			case 0x3C: return run_nop<addressing_mode::absolute_indexed_x>();
			case 0x3D: return run_and<addressing_mode::absolute_indexed_x>();
			case 0x3E: return run_rol<addressing_mode::absolute_indexed_x>();
			case 0x3F: return run_rla<addressing_mode::absolute_indexed_x>();
			case 0x40: return run_rti();
			case 0x41: return run_eor<addressing_mode::indexed_indirect>();
			case 0x42: return run_stp();
			case 0x43: return run_sre<addressing_mode::indexed_indirect>();
			case 0x44: return run_nop<addressing_mode::zero_page>();
			case 0x45: return run_eor<addressing_mode::zero_page>();
			case 0x46: return run_lsr<addressing_mode::zero_page>();
			case 0x47: return run_sre<addressing_mode::zero_page>();
			case 0x48: return run_pha();
			case 0x49: return run_eor<addressing_mode::immediate>();
			case 0x4A: return run_lsr<addressing_mode::accumulator>();
			case 0x4B: return run_alr<addressing_mode::immediate>();
			case 0x4C: return run_jmp<addressing_mode::absolute>();
			case 0x4D: return run_eor<addressing_mode::absolute>();
			case 0x4E: return run_lsr<addressing_mode::absolute>();
			case 0x4F: return run_sre<addressing_mode::absolute>();
			case 0x50: return run_bvc<addressing_mode::relative>();
			case 0x51: return run_eor<addressing_mode::indirect_indexed>();
			case 0x52: return run_stp();
			case 0x53: return run_sre<addressing_mode::indirect_indexed>();
			case 0x54: return run_nop<addressing_mode::zero_page_indexed_x>();
			case 0x55: return run_eor<addressing_mode::zero_page_indexed_x>();
			case 0x56: return run_lsr<addressing_mode::zero_page_indexed_x>();
			case 0x57: return run_sre<addressing_mode::zero_page_indexed_x>();
			case 0x58: return run_cli();
			case 0x59: return run_eor<addressing_mode::absolute_indexed_y>();
			case 0x5A: return run_nop();
			case 0x5B: return run_sre<addressing_mode::absolute_indexed_y>();
			case 0x5C: return run_nop<addressing_mode::absolute_indexed_x>();
			case 0x5D: return run_eor<addressing_mode::absolute_indexed_x>();
			case 0x5E: return run_lsr<addressing_mode::absolute_indexed_x>();
			case 0x5F: return run_sre<addressing_mode::absolute_indexed_x>();
			case 0x60: return run_rts();
			case 0x61: return run_adc<addressing_mode::indexed_indirect>();
			case 0x62: return run_stp();
			case 0x63: return run_rra<addressing_mode::indexed_indirect>();
			case 0x64: return run_nop<addressing_mode::zero_page>();
			case 0x65: return run_adc<addressing_mode::zero_page>();
			case 0x66: return run_ror<addressing_mode::zero_page>();
			case 0x67: return run_rra<addressing_mode::zero_page>();
			case 0x68: return run_pla();
			case 0x69: return run_adc<addressing_mode::immediate>();
			case 0x6A: return run_ror<addressing_mode::accumulator>();
			case 0x6B: return run_arr<addressing_mode::immediate>();
			case 0x6C: return run_jmp<addressing_mode::indirect>();
			case 0x6D: return run_adc<addressing_mode::absolute>();
			case 0x6E: return run_ror<addressing_mode::absolute>();
			case 0x6F: return run_rra<addressing_mode::absolute>();
			case 0x70: return run_bvs<addressing_mode::relative>();
			case 0x71: return run_adc<addressing_mode::indirect_indexed>();
			case 0x72: return run_stp();
			case 0x73: return run_rra<addressing_mode::indirect_indexed>();
			case 0x74: return run_nop<addressing_mode::zero_page_indexed_x>();
			case 0x75: return run_adc<addressing_mode::zero_page_indexed_x>();
			case 0x76: return run_ror<addressing_mode::zero_page_indexed_x>();
			case 0x77: return run_rra<addressing_mode::zero_page_indexed_x>();
			case 0x78: return run_sei();
			case 0x79: return run_adc<addressing_mode::absolute_indexed_y>();
			case 0x7A: return run_nop();
			case 0x7B: return run_rra<addressing_mode::absolute_indexed_y>();
			case 0x7C: return run_nop<addressing_mode::absolute_indexed_x>();
			case 0x7D: return run_adc<addressing_mode::absolute_indexed_x>();
			case 0x7E: return run_ror<addressing_mode::absolute_indexed_x>();
			case 0x7F: return run_rra<addressing_mode::absolute_indexed_x>();
			case 0x80: return run_nop<addressing_mode::immediate>();
			case 0x81: return run_sta<addressing_mode::indexed_indirect>();
			case 0x82: return run_nop<addressing_mode::immediate>();
			case 0x83: return run_sax<addressing_mode::indexed_indirect>();
			case 0x84: return run_sty<addressing_mode::zero_page>();
			case 0x85: return run_sta<addressing_mode::zero_page>();
			case 0x86: return run_stx<addressing_mode::zero_page>();
			case 0x87: return run_sax<addressing_mode::zero_page>();
			case 0x88: return run_dey();
			case 0x89: return run_nop<addressing_mode::immediate>();
			case 0x8A: return run_txa();
			case 0x8B: return run_xaa<addressing_mode::immediate>();
			case 0x8C: return run_sty<addressing_mode::absolute>();
			case 0x8D: return run_sta<addressing_mode::absolute>();
			case 0x8E: return run_stx<addressing_mode::absolute>();
			case 0x8F: return run_sax<addressing_mode::absolute>();
			case 0x90: return run_bcc<addressing_mode::relative>();
			case 0x91: return run_sta<addressing_mode::indirect_indexed>();
			case 0x92: return run_stp();
			case 0x93: return run_ahx<addressing_mode::indirect_indexed>();
			case 0x94: return run_sty<addressing_mode::zero_page_indexed_x>();
			case 0x95: return run_sta<addressing_mode::zero_page_indexed_x>();
			case 0x96: return run_stx<addressing_mode::zero_page_indexed_y>();
			case 0x97: return run_sax<addressing_mode::zero_page_indexed_y>();
			case 0x98: return run_tya();
			case 0x99: return run_sta<addressing_mode::absolute_indexed_y>();
			case 0x9A: return run_txs();
			case 0x9B: return run_tas<addressing_mode::absolute_indexed_y>();
			case 0x9C: return run_shy<addressing_mode::absolute_indexed_x>();
			case 0x9D: return run_sta<addressing_mode::absolute_indexed_x>();
			case 0x9E: return run_shx<addressing_mode::absolute_indexed_y>();
			case 0x9F: return run_ahx<addressing_mode::absolute_indexed_y>();
			case 0xA0: return run_ldy<addressing_mode::immediate>();
			case 0xA1: return run_lda<addressing_mode::indexed_indirect>();
			case 0xA2: return run_ldx<addressing_mode::immediate>();
			case 0xA3: return run_lax<addressing_mode::indexed_indirect>();
			case 0xA4: return run_ldy<addressing_mode::zero_page>();
			case 0xA5: return run_lda<addressing_mode::zero_page>();
			case 0xA6: return run_ldx<addressing_mode::zero_page>();
			case 0xA7: return run_lax<addressing_mode::zero_page>();
			case 0xA8: return run_tay();
			case 0xA9: return run_lda<addressing_mode::immediate>();
			case 0xAA: return run_tax();
			case 0xAB: return run_lax<addressing_mode::immediate>();
			case 0xAC: return run_ldy<addressing_mode::absolute>();
			case 0xAD: return run_lda<addressing_mode::absolute>();
			case 0xAE: return run_ldx<addressing_mode::absolute>();
			case 0xAF: return run_lax<addressing_mode::absolute>();
			case 0xB0: return run_bcs<addressing_mode::relative>();
			case 0xB1: return run_lda<addressing_mode::indirect_indexed>();
			case 0xB2: return run_stp();
			case 0xB3: return run_lax<addressing_mode::indirect_indexed>();
			case 0xB4: return run_ldy<addressing_mode::zero_page_indexed_x>();
			case 0xB5: return run_lda<addressing_mode::zero_page_indexed_x>();
			case 0xB6: return run_ldx<addressing_mode::zero_page_indexed_y>();
			case 0xB7: return run_lax<addressing_mode::zero_page_indexed_y>();
			case 0xB8: return run_clv();
			case 0xB9: return run_lda<addressing_mode::absolute_indexed_y>();
			case 0xBA: return run_tsx();
			case 0xBB: return run_las<addressing_mode::absolute_indexed_y>();
			case 0xBC: return run_ldy<addressing_mode::absolute_indexed_x>();
			case 0xBD: return run_lda<addressing_mode::absolute_indexed_x>();
			case 0xBE: return run_ldx<addressing_mode::absolute_indexed_y>();
			case 0xBF: return run_lax<addressing_mode::absolute_indexed_y>();
			case 0xC0: return run_cpy<addressing_mode::immediate>();
			case 0xC1: return run_cmp<addressing_mode::indexed_indirect>();
			case 0xC2: return run_nop<addressing_mode::immediate>();
			case 0xC3: return run_dcp<addressing_mode::indexed_indirect>();
			case 0xC4: return run_cpy<addressing_mode::zero_page>();
			case 0xC5: return run_cmp<addressing_mode::zero_page>();
			case 0xC6: return run_dec<addressing_mode::zero_page>();
			case 0xC7: return run_dcp<addressing_mode::zero_page>();
			case 0xC8: return run_iny();
			case 0xC9: return run_cmp<addressing_mode::immediate>();
			case 0xCA: return run_dex();
			case 0xCB: return run_axs<addressing_mode::immediate>();
			case 0xCC: return run_cpy<addressing_mode::absolute>();
			case 0xCD: return run_cmp<addressing_mode::absolute>();
			case 0xCE: return run_dec<addressing_mode::absolute>();
			case 0xCF: return run_dcp<addressing_mode::absolute>();
			case 0xD0: return run_bne<addressing_mode::relative>();
			case 0xD1: return run_cmp<addressing_mode::indirect_indexed>();
			case 0xD2: return run_stp();
			case 0xD3: return run_dcp<addressing_mode::indirect_indexed>();
			case 0xD4: return run_nop<addressing_mode::zero_page_indexed_x>();
			case 0xD5: return run_cmp<addressing_mode::zero_page_indexed_x>();
			case 0xD6: return run_dec<addressing_mode::zero_page_indexed_x>();
			case 0xD7: return run_dcp<addressing_mode::zero_page_indexed_x>();
			case 0xD8: return run_cld();
			case 0xD9: return run_cmp<addressing_mode::absolute_indexed_y>();
			case 0xDA: return run_nop();
			case 0xDB: return run_dcp<addressing_mode::absolute_indexed_y>();
			case 0xDC: return run_nop<addressing_mode::absolute_indexed_x>();
			case 0xDD: return run_cmp<addressing_mode::absolute_indexed_x>();
			case 0xDE: return run_dec<addressing_mode::absolute_indexed_x>();
			case 0xDF: return run_dcp<addressing_mode::absolute_indexed_x>();
			case 0xE0: return run_cpx<addressing_mode::immediate>();
			case 0xE1: return run_sbc<addressing_mode::indexed_indirect>();
			case 0xE2: return run_nop<addressing_mode::immediate>();
			case 0xE3: return run_isc<addressing_mode::indexed_indirect>();
			case 0xE4: return run_cpx<addressing_mode::zero_page>();
			case 0xE5: return run_sbc<addressing_mode::zero_page>();
			case 0xE6: return run_inc<addressing_mode::zero_page>();
			case 0xE7: return run_isc<addressing_mode::zero_page>();
			case 0xE8: return run_inx();
			case 0xE9: return run_sbc<addressing_mode::immediate>();
			case 0xEA: return run_nop();
			case 0xEB: return run_sbc<addressing_mode::immediate>();
			case 0xEC: return run_cpx<addressing_mode::absolute>();
			case 0xED: return run_sbc<addressing_mode::absolute>();
			case 0xEE: return run_inc<addressing_mode::absolute>();
			case 0xEF: return run_isc<addressing_mode::absolute>();
			case 0xF0: return run_beq<addressing_mode::relative>();
			case 0xF1: return run_sbc<addressing_mode::indirect_indexed>();
			case 0xF2: return run_stp();
			case 0xF3: return run_isc<addressing_mode::indirect_indexed>();
			case 0xF4: return run_nop<addressing_mode::zero_page_indexed_x>();
			case 0xF5: return run_sbc<addressing_mode::zero_page_indexed_x>();
			case 0xF6: return run_inc<addressing_mode::zero_page_indexed_x>();
			case 0xF7: return run_isc<addressing_mode::zero_page_indexed_x>();
			case 0xF8: return run_sed();
			case 0xF9: return run_sbc<addressing_mode::absolute_indexed_y>();
			case 0xFA: return run_nop();
			case 0xFB: return run_isc<addressing_mode::absolute_indexed_y>();
			case 0xFC: return run_nop<addressing_mode::absolute_indexed_x>();
			case 0xFD: return run_sbc<addressing_mode::absolute_indexed_x>();
			case 0xFE: return run_inc<addressing_mode::absolute_indexed_x>();
			case 0xFF: return run_isc<addressing_mode::absolute_indexed_x>();
			default: return status::error_unsupported_instruction;
		}
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Instructions
	// -----------------------------------------------------------------------------------------------------------------

	template<cpu::addressing_mode Mode>
	auto cpu::run_brk() -> status
	{
		// BRK: Break
		fetch_operand<Mode>();
		execute_interrupt(address{ 0xFFFE });
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ora() -> status
	{
		// ORA: Logical Inclusive OR
		auto operand = fetch_operand<Mode>();
		eval_ora(operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	auto cpu::run_stp() -> status
	{
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_slo() -> status
	{
		// SLO: ASL + ORA
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_asl(operand.read());
		operand.write(tmp);
		eval_ora(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_nop() -> status
	{
		// NOP: NOP
		auto operand = fetch_operand<Mode>();
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	auto cpu::run_nop() -> status
	{
		// NOP: NOP
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_php() -> status
	{
		// PHP: Push Processor Status
		eval_php();
		current_cycles_ += cycle_count::from_cpu(3);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_asl() -> status
	{
		// ASL: Arithmetic Shift Left
		auto operand = fetch_operand<Mode>();
		operand.write(eval_asl(operand.read()));
		current_cycles_ += shift_cycle_count<Mode>();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_anc() -> status
	{
		// ANC: AND #i + copy N to C
		auto operand = fetch_operand<Mode>();
		eval_and(operand.read());
		registers_.p.set_c(registers_.p.get_n());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bpl() -> status
	{
		// BPL: Break If Positive
		branch<Mode>(!registers_.p.get_n());
		return status::success;
	}

	auto cpu::run_clc() -> status
	{
		// CLC: Clear Carry Flag
		registers_.p.set_c(false);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_jsr() -> status
	{
		// JSR: Jump to Subroutine
		static_assert(Mode == addressing_mode::absolute, "JSR only supports absolute addressing.");

		push_stack16(registers_.pc + 1);

		auto operand = fetch_operand<Mode>();
		registers_.pc = operand.get_address().get_absolute();
		current_cycles_ += cycle_count::from_cpu(6);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_and() -> status
	{
		// AND: Logical AND
		auto operand = fetch_operand<Mode>();
		eval_and(operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_rla() -> status
	{
		// RLA: ROL + AND
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_rol(operand.read());
		operand.write(tmp);
		eval_and(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bit() -> status
	{
		// BIT: Bit Test
		auto operand = fetch_operand<Mode>();
		auto const arg = operand.read();
		auto const res = (registers_.a & arg) != 0;
		registers_.p.set_z(res == 0);
		registers_.p.set_v((arg & 0x40) != 0);
		registers_.p.set_n((arg & 0x80) != 0);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_rol() -> status
	{
		// ROR: Rotate Left
		auto operand = fetch_operand<Mode>();
		auto const result = eval_rol(operand.read());
		operand.write(result);
		current_cycles_ += shift_cycle_count<Mode>();
		return status::success;
	}

	auto cpu::run_plp() -> status
	{
		// PLP: Pull Processor Status
		eval_plp();
		current_cycles_ += cycle_count::from_cpu(4);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bmi() -> status
	{
		branch<Mode>(registers_.p.get_n());
		return status::success;
	}

	auto cpu::run_sec() -> status
	{
		// SEC: Set Carry Flag
		registers_.p.set_c(true);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_rti() -> status
	{
		// RTI: Return From Interrupt
		eval_plp();
		registers_.pc = pop_stack16();
		current_cycles_ += cycle_count::from_cpu(6);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_eor() -> status
	{
		// EOR: Exclusive OR
		auto operand = fetch_operand<Mode>();
		eval_eor(operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sre() -> status
	{
		// SRE: LSR + EOR
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_lsr(operand.read());
		operand.write(tmp);
		eval_eor(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_lsr() -> status
	{
		// LSR: Logical Shift Right
		auto operand = fetch_operand<Mode>();
		auto const old_val = operand.read();
		auto const new_val = static_cast<u8>(old_val >> 1);
		operand.write(new_val);
		registers_.p.set_c((old_val & 0x1) != 0);
		update_zn(new_val);
		current_cycles_ += shift_cycle_count<Mode>();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_alr() -> status
	{
		// ALR: AND #i + LSR A
		auto operand = fetch_operand<Mode>();
		eval_and(operand.read());
		registers_.a = eval_lsr(registers_.a);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	auto cpu::run_pha() -> status
	{
		// PHA: Push Accumulator
		push_stack8(registers_.a);
		current_cycles_ += cycle_count::from_cpu(3);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_jmp() -> status
	{
		// JMP: Jump
		auto operand = fetch_operand<Mode>();
		registers_.pc = operand.get_address().get_absolute();
		current_cycles_ += jump_cycle_count<Mode>();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bvc() -> status
	{
		// BVC: Branch If Overflow Clear
		branch<Mode>(!registers_.p.get_v());
		return status::success;
	}

	auto cpu::run_cli() -> status
	{
		// CLI: Clear Interrupt Disable
		registers_.p.set_i(false);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_rts() -> status
	{
		// RTS: Return from Subroutine
		auto const addr = address{ pop_stack16() } + 1;
		registers_.pc = addr.get_absolute();
		current_cycles_ += cycle_count::from_cpu(6);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_adc() -> status
	{
		// ADC: Add With Carry
		auto operand = fetch_operand<Mode>();
		eval_adc(operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_rra() -> status
	{
		// RRA: ROR + ADC
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = eval_ror(operand.read());
		operand.write(tmp);
		eval_adc(tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ror() -> status
	{
		// ROR: Rotate Right
		auto operand = fetch_operand<Mode>();
		auto const result = eval_ror(operand.read());
		operand.write(result);
		current_cycles_ += shift_cycle_count<Mode>();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_arr() -> status
	{
		// ARR: AND #i + ROR A but with different flags
		// TODO
		return status::error_unsupported_instruction;
	}

	auto cpu::run_pla() -> status
	{
		// PLA: Pull Accumulator
		registers_.a = pop_stack8();
		update_zn(registers_.a);
		current_cycles_ += cycle_count::from_cpu(4);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bvs() -> status
	{
		// BVS: Branch If Overflow Set
		branch<Mode>(registers_.p.get_v());
		return status::success;
	}

	auto cpu::run_sei() -> status
	{
		// SEI: Set Interrupt Disable
		registers_.p.set_i(true);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sta() -> status
	{
		// STA: Store Accumulator
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		operand.write(registers_.a);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ahx() -> status
	{
		// AHX: ???
		// TODO
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sax() -> status
	{
		// SAX: AND A X
		auto operand = fetch_operand<Mode>();
		auto const result = static_cast<u8>(registers_.a & registers_.x);
		operand.write(result);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sty() -> status
	{
		// STX: Store Y
		auto operand = fetch_operand<Mode>();
		operand.write(registers_.y);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_stx() -> status
	{
		// STX: Store X
		auto operand = fetch_operand<Mode>();
		operand.write(registers_.x);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	auto cpu::run_dey() -> status
	{
		// DEY: Decrement Y Register
		registers_.y -= 1;
		update_zn(registers_.y);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_txa() -> status
	{
		// TXA: Transfer X to Accumulator
		registers_.a = registers_.x;
		update_zn(registers_.a);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_xaa() -> status
	{
		// XAA: Depends on analog behavior.
		run_stp();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bcc() -> status
	{
		// BCC: Branch If Carry Clear
		branch<Mode>(!registers_.p.get_c());
		return status::success;
	}

	auto cpu::run_tya() -> status
	{
		// TYA: Transfer Y to Accumulator
		registers_.a = registers_.y;
		update_zn(registers_.a);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_txs() -> status
	{
		// TXS: Transfer X to Stack Pointer
		registers_.sp = registers_.x;
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_tas() -> status
	{
		// TAS: ???
		// TODO
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_shy() -> status
	{
		// SHY: ???
		// TODO
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_shx() -> status
	{
		// SHY: ???
		// TODO
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ldy() -> status
	{
		// LDY: Load Y Register
		auto operand = fetch_operand<Mode>();
		registers_.y = operand.read();
		update_zn(registers_.y);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_lda() -> status
	{
		// LDA: Load Accumulator
		auto operand = fetch_operand<Mode>();
		registers_.a = operand.read();
		update_zn(registers_.a);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_ldx() -> status
	{
		// LDX: Load X Register
		auto operand = fetch_operand<Mode>();
		registers_.x = operand.read();
		update_zn(registers_.x);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_lax() -> status
	{
		// LAX: LDA + TAX
		auto operand = fetch_operand<Mode>();
		registers_.x = registers_.a = operand.read();
		update_zn(registers_.a);
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	auto cpu::run_tay() -> status
	{
		// TAY: Transfer Accumulator to Y
		registers_.y = registers_.a;
		update_zn(registers_.y);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_tax() -> status
	{
		// TAX: Transfer Accumulator to X
		registers_.x = registers_.a;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bcs() -> status
	{
		// BCS: Branch If Carry Set
		branch<Mode>(registers_.p.get_c());
		return status::success;
	}

	auto cpu::run_clv() -> status
	{
		// CLV: Clear Overflow Flag
		registers_.p.set_v(false);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_tsx() -> status
	{
		// TSX: Transfer Stack Pointer to X
		registers_.x = registers_.sp;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_las() -> status
	{
		// LAS: ???
		// TODO
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_cpy() -> status
	{
		// CPY: Compare Y Register
		auto operand = fetch_operand<Mode>();
		eval_cmp(registers_.y, operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_axs() -> status
	{
		// AXS: Set X to ((A AND X) - #value without borrow), updates NZC
		// TODO
		return status::error_unsupported_instruction;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_cmp() -> status
	{
		// CMP: Compare
		auto operand = fetch_operand<Mode>();
		eval_cmp(registers_.a, operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_dcp() -> status
	{
		// DCP: DEC + CMP
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = static_cast<u8>(operand.read() - 1);
		operand.write(tmp);
		eval_cmp(registers_.a, tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_dec() -> status
	{
		// DEC: Decrement Memory
		auto operand = fetch_operand<Mode>();
		auto const old_val = read8(operand.get_address());
		auto const new_val = static_cast<u8>(old_val - 1);
		write8(operand.get_address(), new_val);
		update_zn(new_val);
		current_cycles_ += inc_dec_cycle_count<Mode>();
		return status::success;
	}

	auto cpu::run_iny() -> status
	{
		// INY: Increment Y
		registers_.y += 1;
		update_zn(registers_.y);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	auto cpu::run_dex() -> status
	{
		// DEX: Decrement X Register
		registers_.x -= 1;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_bne() -> status
	{
		// BNE: Branch If Not Equal
		branch<Mode>(!registers_.p.get_z());
		return status::success;
	}

	auto cpu::run_cld() -> status
	{
		// CLD: Clear Decimal Mode
		registers_.p.set_d(false);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_cpx() -> status
	{
		// CPX: Compare X Register
		auto operand = fetch_operand<Mode>();
		eval_cmp(registers_.x, operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_sbc() -> status
	{
		// SBC: Subtract With Carry
		auto operand = fetch_operand<Mode>();
		eval_adc(~operand.read());
		current_cycles_ += operand.get_cycles();
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_isc() -> status
	{
		// ISC: INC + SBC
		auto operand = fetch_operand<Mode>(force_page_crossing::yes);
		auto const tmp = static_cast<u8>(operand.read() + 1);
		operand.write(tmp);
		eval_adc(~tmp);
		current_cycles_ += operand.get_cycles() + cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_inc() -> status
	{
		// INC: Increment Memory
		auto operand = fetch_operand<Mode>();
		auto const old_val = read8(operand.get_address());
		auto const new_val = static_cast<u8>(old_val + 1);
		write8(operand.get_address(), new_val);
		update_zn(new_val);
		current_cycles_ += inc_dec_cycle_count<Mode>();
		return status::success;
	}

	auto cpu::run_inx() -> status
	{
		// INX: Increment X
		registers_.x += 1;
		update_zn(registers_.x);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}

	template<cpu::addressing_mode Mode>
	auto cpu::run_beq() -> status
	{
		// BEQ: Branch If Equal
		branch<Mode>(registers_.p.get_z());
		return status::success;
	}

	auto cpu::run_sed() -> status
	{
		// SED: Set Decimal Flag
		registers_.p.set_d(true);
		current_cycles_ += cycle_count::from_cpu(2);
		return status::success;
	}
	
	// -----------------------------------------------------------------------------------------------------------------
	// Helpers
	// -----------------------------------------------------------------------------------------------------------------

	auto cpu::advance_pc8() -> u8
	{
		auto const val = read8(address{ registers_.pc });
		registers_.pc += 1;
		return val;
	}

	auto cpu::advance_pc16() -> u16
	{
		auto const val = read16(address{ registers_.pc });
		registers_.pc += 2;
		return val;
	}

	auto cpu::push_stack8(u8 const value) -> void
	{
		write8(stack_offset + registers_.sp, value);
		registers_.sp -= 1;
	}

	auto cpu::push_stack16(u16 const value) -> void
	{
		auto const low = static_cast<u8>(value >> 0 & 0xFF);
		auto const high = static_cast<u8>(value >> 8 & 0xFF);
		push_stack8(high);
		push_stack8(low);
	}

	auto cpu::pop_stack8() -> u8
	{
		registers_.sp += 1;
		return read8(stack_offset + registers_.sp);
	}

	auto cpu::pop_stack16() -> u16
	{
		auto const low = pop_stack8();
		auto const high = pop_stack8();
		return static_cast<u16>((high << 8) | (low << 0));
	}

	auto cpu::update_zn(u8 const value) -> void
	{
		registers_.p.set_z(value == 0);
		registers_.p.set_n((value & 0x80) != 0);
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

	auto cpu::execute_interrupt(address const vector) -> void
	{
		push_stack16(registers_.pc);
		eval_php();
		registers_.pc = read16(vector);
		registers_.p.set_i(true);
		current_cycles_ += cycle_count::from_cpu(7);
	}

	auto cpu::eval_ror(u8 const arg) -> u8
	{
		auto const res = static_cast<u8>(((registers_.p.get_c() ? 1 :0) << 7) | (arg >> 1));
		registers_.p.set_c((arg & 0x1) != 0);
		update_zn(res);
		return res;
	}

	auto cpu::eval_rol(u8 const arg) -> u8
	{
		auto const res = static_cast<u8>((arg << 1) | (registers_.p.get_c() ? 1 : 0));
		registers_.p.set_c((arg & 0x80) != 0);
		update_zn(res);
		return res;
	}

	auto cpu::eval_asl(u8 const arg) -> u8
	{
		auto const new_val = static_cast<u8>(arg << 1);
		registers_.p.set_c((arg & 0x80) != 0);
		update_zn(new_val);
		return new_val;
	}

	auto cpu::eval_lsr(u8 const arg) -> u8
	{
		auto const new_val = static_cast<u8>(arg >> 1);
		registers_.p.set_c((arg & 0x01) != 0);
		update_zn(new_val);
		return new_val;
	}

	auto cpu::eval_adc(u8 const arg) -> void
	{
		auto const old_val = registers_.a;
		auto const tmp = old_val + arg + (registers_.p.get_c() ? 1 : 0);
		auto const new_val = static_cast<u8>(tmp);

		registers_.a = new_val;
		registers_.p.set_c(tmp > 0xFF);
		registers_.p.set_v(((old_val ^ arg) & 0x80) == 0 && ((old_val ^ new_val) & 0x80) != 0);
		update_zn(registers_.a);
	}

	auto cpu::eval_and(u8 const arg) -> void
	{
		registers_.a &= arg;
		update_zn(registers_.a);
	}

	auto cpu::eval_ora(u8 const arg) -> void
	{
		registers_.a |= arg;
		update_zn(registers_.a);
	}

	auto cpu::eval_eor(u8 const arg) -> void
	{
		registers_.a ^= arg;
		update_zn(registers_.a);
	}

	auto cpu::eval_cmp(u8 const a, u8 const b) -> void
	{
		update_zn(a - b);
		registers_.p.set_c(a >= b);
	}

	auto cpu::eval_plp() -> void
	{
		// Ignore bits 4 and 5
		registers_.p.value =
			(pop_stack8() & 0b11001111) |
			(registers_.p.value & 0b00110000);
	}

	auto cpu::eval_php() -> void
	{
		// Always set bits 4 and 5.
		push_stack8(registers_.p.value | 0b00110000);
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
			case addressing_mode::immediate: static_assert(Mode != addressing_mode::immediate);
			case addressing_mode::accumulator: static_assert(Mode != addressing_mode::accumulator);
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
				auto const offset = static_cast<i8>(advance_pc8());
				auto const base = address{ registers_.pc };
				addr = address{ static_cast<u16>(registers_.pc + offset) };
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
				addr = address{ 0x00, static_cast<u8>(advance_pc8() + registers_.x) };
				cycles = cycle_count::from_cpu(4);
				break;
			}
			case addressing_mode::zero_page_indexed_y:
			{
				// PEEK((arg + Y) % 256)
				addr = address{ 0x00, static_cast<u8>(advance_pc8() + registers_.y) };
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
				auto const page = read8(address{ 0x00, static_cast<u8>(arg + registers_.x + 1) });
				auto const offset = read8(address{ 0x00, static_cast<u8>(arg + registers_.x) });
				addr = address{ page, offset };
				cycles = cycle_count::from_cpu(6);
				break;
			}
			case addressing_mode::indirect_indexed:
			{
				// PEEK(PEEK(arg) + PEEK((arg + 1) % 256) * 256 + Y)
				auto const arg = advance_pc8();
				auto const page = read8(address{ 0x00, static_cast<u8>(arg + 1) });
				auto const offset = read8(address{ 0x00, static_cast<u8>(arg) });
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
		return operand<addressing_mode::immediate>{ advance_pc8() };
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Memory Access
	// -----------------------------------------------------------------------------------------------------------------

	//
	// See: https://www.nesdev.org/wiki/CPU_memory_map
	//

	auto cpu::read8(address const addr) -> u8
	{
		if (addr <= address{ 0x1FFF }) { return ram_[addr.get_absolute() % ram_size]; }
		if (addr <= address{ 0x3FFF })
		{
#ifdef NES_ENABLE_PPU
			switch (addr.get_absolute() % 8)
			{
				case 0: return ppu_.read_latch();
				case 1: return ppu_.read_latch();
				case 2: return ppu_.read_ppustatus();
				case 3: return ppu_.read_latch();
				case 4: return ppu_.read_oamdata();
				case 5: return ppu_.read_latch();
				case 6: return ppu_.read_latch();
				case 7: return ppu_.read_ppudata();
				default: return 0x0;
			}
#else
			return 0x0;
#endif
		}
		if (addr <= address{ 0x4013 }) { return 0x0; } // TODO: APU registers
#ifdef NES_ENABLE_PPU
		if (addr == address{ 0x4014 }) { return ppu_.read_latch(); }
#else
		if (addr == address{ 0x4014 }) { return 0x0; }
#endif
		if (addr == address{ 0x4015 }) { return 0x0; } // TODO: APU registers
		if (addr == address{ 0x4016 }) { return controller_1_.read(); }
		if (addr == address{ 0x4017 }) { return controller_2_.read(); }
		if (addr <= address{ 0x401F }) { return 0x0; }
		return cartridge_.get_mapper().read_cpu(addr, cartridge_);
	}

	auto cpu::read16(address const addr) -> u16
	{
		auto const low = read8(addr + 0);
		auto const high = read8(addr + 1);
		return static_cast<u16>((high << 8) | (low << 0));
	}

	auto cpu::write8(address const addr, u8 const value) -> void
	{
		if (addr <= address{ 0x1FFF }) { ram_[addr.get_absolute() % ram_size] = value; return; }
		if (addr <= address{ 0x3FFF })
		{
#ifdef NES_ENABLE_PPU
			switch (addr.get_absolute() % 8)
			{
				case 0: ppu_.write_ppuctrl(value); return;
				case 1: ppu_.write_ppumask(value); return;
				case 2: ppu_.write_latch(value); return;
				case 3: ppu_.write_oamaddr(value); return;
				case 4: ppu_.write_oamdata(value); return;
				case 5: ppu_.write_ppuscroll(value); return;
				case 6: ppu_.write_ppuaddr(value); return;
				case 7: ppu_.write_ppudata(value); return;
				default: return;
			}
#else
			return;
#endif
		}
		if (addr <= address{ 0x4013 }) { return; } // TODO: APU registers
#ifdef NES_ENABLE_PPU
		if (addr == address{ 0x4014 }) { ppu_.write_oamdma(value); return; }
#else
		if (addr == address{ 0x4014 }) { return; }
#endif
		if (addr == address{ 0x4015 }) { return; } // TODO: APU registers
		if (addr == address{ 0x4016 }) { controller_1_.write(value); return; }
		if (addr == address{ 0x4017 }) { controller_2_.write(value); return; }
		if (addr <= address{ 0x401F }) { return; }
		cartridge_.get_mapper().write_cpu(addr, value, cartridge_);
	}

	auto cpu::write16(address const addr, u16 const value) -> void
	{
		auto const low = static_cast<u8>(value >> 0 & 0xFF);
		auto const high = static_cast<u8>(value >> 8 & 0xFF);
		write8(addr + 0, low);
		write8(addr + 1, high);
	}
} // namespace nes::sys