#include "nes/app/input/key.hh"
#include "nes/app/input/modifier-mask.hh"

namespace nes::app
{
	auto to_ascii(key const k, modifier_mask const m) -> char
	{
		switch (k)
		{
			case key::digit_0: return m.contains(modifiers::shift) ? ')' : '0';
			case key::digit_1: return m.contains(modifiers::shift) ? '!' : '1';
			case key::digit_2: return m.contains(modifiers::shift) ? '@' : '2';
			case key::digit_3: return m.contains(modifiers::shift) ? '#' : '3';
			case key::digit_4: return m.contains(modifiers::shift) ? '$' : '4';
			case key::digit_5: return m.contains(modifiers::shift) ? '%' : '5';
			case key::digit_6: return m.contains(modifiers::shift) ? '^' : '6';
			case key::digit_7: return m.contains(modifiers::shift) ? '&' : '7';
			case key::digit_8: return m.contains(modifiers::shift) ? '*' : '8';
			case key::digit_9: return m.contains(modifiers::shift) ? '(' : '9';
			case key::letter_a: return m.contains(modifiers::shift) ? 'A' : 'a';
			case key::letter_b: return m.contains(modifiers::shift) ? 'B' : 'b';
			case key::letter_c: return m.contains(modifiers::shift) ? 'C' : 'c';
			case key::letter_d: return m.contains(modifiers::shift) ? 'D' : 'd';
			case key::letter_e: return m.contains(modifiers::shift) ? 'E' : 'e';
			case key::letter_f: return m.contains(modifiers::shift) ? 'F' : 'f';
			case key::letter_g: return m.contains(modifiers::shift) ? 'G' : 'g';
			case key::letter_h: return m.contains(modifiers::shift) ? 'H' : 'h';
			case key::letter_i: return m.contains(modifiers::shift) ? 'I' : 'i';
			case key::letter_j: return m.contains(modifiers::shift) ? 'J' : 'j';
			case key::letter_k: return m.contains(modifiers::shift) ? 'K' : 'k';
			case key::letter_l: return m.contains(modifiers::shift) ? 'L' : 'l';
			case key::letter_m: return m.contains(modifiers::shift) ? 'M' : 'm';
			case key::letter_n: return m.contains(modifiers::shift) ? 'N' : 'n';
			case key::letter_o: return m.contains(modifiers::shift) ? 'O' : 'o';
			case key::letter_p: return m.contains(modifiers::shift) ? 'P' : 'p';
			case key::letter_q: return m.contains(modifiers::shift) ? 'Q' : 'q';
			case key::letter_r: return m.contains(modifiers::shift) ? 'R' : 'r';
			case key::letter_s: return m.contains(modifiers::shift) ? 'S' : 's';
			case key::letter_t: return m.contains(modifiers::shift) ? 'T' : 't';
			case key::letter_u: return m.contains(modifiers::shift) ? 'U' : 'u';
			case key::letter_v: return m.contains(modifiers::shift) ? 'V' : 'v';
			case key::letter_w: return m.contains(modifiers::shift) ? 'W' : 'w';
			case key::letter_x: return m.contains(modifiers::shift) ? 'X' : 'x';
			case key::letter_y: return m.contains(modifiers::shift) ? 'Y' : 'y';
			case key::letter_z: return m.contains(modifiers::shift) ? 'Z' : 'z';
			case key::dash: return m.contains(modifiers::shift) ? '_' : '-';
			case key::equal: return m.contains(modifiers::shift) ? '+' : '=';
			case key::semicolon: return m.contains(modifiers::shift) ? ':' : ';';
			case key::apostrophe: return m.contains(modifiers::shift) ? '"' : '\'';
			case key::backslash: return m.contains(modifiers::shift) ? '|' : '\\';
			case key::comma: return m.contains(modifiers::shift) ? '<' : ',';
			case key::period: return m.contains(modifiers::shift) ? '>' : '.';
			case key::slash: return m.contains(modifiers::shift) ? '?' : '/';
			case key::bracket_open: return m.contains(modifiers::shift) ? '{' : '[';
			case key::bracket_close: return m.contains(modifiers::shift) ? '}' : ']';
			case key::tab: return '\t';
			case key::enter: return '\n';
			case key::space: return ' ';
			case key::escape: return 0;
			case key::backspace: return 0;
			case key::caps_lock: return 0;
			case key::control_left: return 0;
			case key::control_right: return 0;
			case key::shift_left: return 0;
			case key::shift_right: return 0;
			case key::alt_left: return 0;
			case key::alt_right: return 0;
			case key::arrow_left: return 0;
			case key::arrow_right: return 0;
			case key::arrow_up: return 0;
			case key::arrow_down: return 0;
			case key::f1: return 0;
			case key::f2: return 0;
			case key::f3: return 0;
			case key::f4: return 0;
			case key::f5: return 0;
			case key::f6: return 0;
			case key::f7: return 0;
			case key::f8: return 0;
			case key::f9: return 0;
			case key::f10: return 0;
			case key::f11: return 0;
			case key::f12: return 0;
		}

		return 0;
	}
} // namespace nes::app