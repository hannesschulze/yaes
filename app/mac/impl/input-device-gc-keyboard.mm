#import "impl/input-device-gc-keyboard.hh"

namespace nes::app::mac
{
	input_device_gc_keyboard::input_device_gc_keyboard(GCKeyboardInput* profile)
	{
		set_profile(profile);
	}

	input_device_gc_keyboard::~input_device_gc_keyboard()
	{
		set_profile(nil);
	}

	auto input_device_gc_keyboard::set_profile(GCKeyboardInput* profile) -> void
	{
		[profile_ setKeyChangedHandler:nil];
		profile_ = profile;
		[profile_ setKeyChangedHandler:^(GCKeyboardInput*, GCControllerButtonInput*, GCKeyCode key_code, BOOL pressed) {
		    handle_key_changed(key_code, pressed);
		}];
	}

	auto input_device_gc_keyboard::handle_key_changed(GCKeyCode const key_code, BOOL const pressed) -> void
	{
		auto const key = convert_key_code(key_code);
		if (key)
		{
			if (pressed)
			{
				buffer_.key_down(*key);
			}
			else
			{
				buffer_.key_up(*key);
			}
		}
	}

	auto input_device_gc_keyboard::convert_key_code(GCKeyCode const key_code) -> std::optional<key>
	{
		if (key_code == GCKeyCodeZero) { return key::digit_0; }
		if (key_code == GCKeyCodeOne) { return key::digit_1; }
		if (key_code == GCKeyCodeTwo) { return key::digit_2; }
		if (key_code == GCKeyCodeThree) { return key::digit_3; }
		if (key_code == GCKeyCodeFour) { return key::digit_4; }
		if (key_code == GCKeyCodeFive) { return key::digit_5; }
		if (key_code == GCKeyCodeSix) { return key::digit_6; }
		if (key_code == GCKeyCodeSeven) { return key::digit_7; }
		if (key_code == GCKeyCodeEight) { return key::digit_8; }
		if (key_code == GCKeyCodeNine) { return key::digit_9; }
		if (key_code == GCKeyCodeKeyA) { return key::letter_a; }
		if (key_code == GCKeyCodeKeyB) { return key::letter_b; }
		if (key_code == GCKeyCodeKeyC) { return key::letter_c; }
		if (key_code == GCKeyCodeKeyD) { return key::letter_d; }
		if (key_code == GCKeyCodeKeyE) { return key::letter_e; }
		if (key_code == GCKeyCodeKeyF) { return key::letter_f; }
		if (key_code == GCKeyCodeKeyG) { return key::letter_g; }
		if (key_code == GCKeyCodeKeyH) { return key::letter_h; }
		if (key_code == GCKeyCodeKeyI) { return key::letter_i; }
		if (key_code == GCKeyCodeKeyJ) { return key::letter_j; }
		if (key_code == GCKeyCodeKeyK) { return key::letter_k; }
		if (key_code == GCKeyCodeKeyL) { return key::letter_l; }
		if (key_code == GCKeyCodeKeyM) { return key::letter_m; }
		if (key_code == GCKeyCodeKeyN) { return key::letter_n; }
		if (key_code == GCKeyCodeKeyO) { return key::letter_o; }
		if (key_code == GCKeyCodeKeyP) { return key::letter_p; }
		if (key_code == GCKeyCodeKeyQ) { return key::letter_q; }
		if (key_code == GCKeyCodeKeyR) { return key::letter_r; }
		if (key_code == GCKeyCodeKeyS) { return key::letter_s; }
		if (key_code == GCKeyCodeKeyT) { return key::letter_t; }
		if (key_code == GCKeyCodeKeyU) { return key::letter_u; }
		if (key_code == GCKeyCodeKeyV) { return key::letter_v; }
		if (key_code == GCKeyCodeKeyW) { return key::letter_w; }
		if (key_code == GCKeyCodeKeyX) { return key::letter_x; }
		if (key_code == GCKeyCodeKeyY) { return key::letter_y; }
		if (key_code == GCKeyCodeKeyZ) { return key::letter_z; }
		if (key_code == GCKeyCodeEscape) { return key::escape; }
		if (key_code == GCKeyCodeHyphen) { return key::dash; }
		if (key_code == GCKeyCodeEqualSign) { return key::equal; }
		if (key_code == GCKeyCodeDeleteOrBackspace) { return key::backspace; }
		if (key_code == GCKeyCodeTab) { return key::tab; }
		if (key_code == GCKeyCodeReturnOrEnter) { return key::enter; }
		if (key_code == GCKeyCodeSemicolon) { return key::semicolon; }
		if (key_code == GCKeyCodeQuote) { return key::apostrophe; }
		if (key_code == GCKeyCodeBackslash) { return key::backslash; }
		if (key_code == GCKeyCodeComma) { return key::comma; }
		if (key_code == GCKeyCodePeriod) { return key::period; }
		if (key_code == GCKeyCodeSlash) { return key::slash; }
		if (key_code == GCKeyCodeSpacebar) { return key::space; }
		if (key_code == GCKeyCodeCapsLock) { return key::caps_lock; }
		if (key_code == GCKeyCodeOpenBracket) { return key::bracket_open; }
		if (key_code == GCKeyCodeCloseBracket) { return key::bracket_close; }
		if (key_code == GCKeyCodeLeftControl) { return key::control_left; }
		if (key_code == GCKeyCodeRightControl) { return key::control_right; }
		if (key_code == GCKeyCodeLeftShift) { return key::shift_left; }
		if (key_code == GCKeyCodeRightShift) { return key::shift_right; }
		if (key_code == GCKeyCodeLeftAlt) { return key::alt_left; }
		if (key_code == GCKeyCodeRightAlt) { return key::alt_right; }
		if (key_code == GCKeyCodeLeftArrow) { return key::arrow_left; }
		if (key_code == GCKeyCodeRightArrow) { return key::arrow_right; }
		if (key_code == GCKeyCodeUpArrow) { return key::arrow_up; }
		if (key_code == GCKeyCodeDownArrow) { return key::arrow_down; }
		if (key_code == GCKeyCodeF1) { return key::f1; }
		if (key_code == GCKeyCodeF2) { return key::f2; }
		if (key_code == GCKeyCodeF3) { return key::f3; }
		if (key_code == GCKeyCodeF4) { return key::f4; }
		if (key_code == GCKeyCodeF5) { return key::f5; }
		if (key_code == GCKeyCodeF6) { return key::f6; }
		if (key_code == GCKeyCodeF7) { return key::f7; }
		if (key_code == GCKeyCodeF8) { return key::f8; }
		if (key_code == GCKeyCodeF9) { return key::f9; }
		if (key_code == GCKeyCodeF10) { return key::f10; }
		if (key_code == GCKeyCodeF11) { return key::f11; }
		if (key_code == GCKeyCodeF12) { return key::f12; }
		
		return std::nullopt;
	}
} // namespace nes::app::mac