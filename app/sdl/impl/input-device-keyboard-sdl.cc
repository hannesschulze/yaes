#include "impl/input-device-keyboard-sdl.hh"
#include <SDL3/SDL_keycode.h>

namespace nes::app::sdl
{
	auto input_device_keyboard_sdl::handle_key_down(SDL_Event* event) -> void
	{
		if (auto key = convert_key(event->key.scancode)) { buffer_.key_down(*key); }
	}

	auto input_device_keyboard_sdl::handle_key_up(SDL_Event* event) -> void
	{
		if (auto key = convert_key(event->key.scancode)) { buffer_.key_up(*key); }
	}

	auto input_device_keyboard_sdl::convert_key(SDL_Scancode code) const -> std::optional<key>
	{
		switch (static_cast<int>(code))
		{
			case SDL_SCANCODE_0: return key::digit_0;
			case SDL_SCANCODE_1: return key::digit_1;
			case SDL_SCANCODE_2: return key::digit_2;
			case SDL_SCANCODE_3: return key::digit_3;
			case SDL_SCANCODE_4: return key::digit_4;
			case SDL_SCANCODE_5: return key::digit_5;
			case SDL_SCANCODE_6: return key::digit_6;
			case SDL_SCANCODE_7: return key::digit_7;
			case SDL_SCANCODE_8: return key::digit_8;
			case SDL_SCANCODE_9: return key::digit_9;
			case SDL_SCANCODE_A: return key::letter_a;
			case SDL_SCANCODE_B: return key::letter_b;
			case SDL_SCANCODE_C: return key::letter_c;
			case SDL_SCANCODE_D: return key::letter_d;
			case SDL_SCANCODE_E: return key::letter_e;
			case SDL_SCANCODE_F: return key::letter_f;
			case SDL_SCANCODE_G: return key::letter_g;
			case SDL_SCANCODE_H: return key::letter_h;
			case SDL_SCANCODE_I: return key::letter_i;
			case SDL_SCANCODE_J: return key::letter_j;
			case SDL_SCANCODE_K: return key::letter_k;
			case SDL_SCANCODE_L: return key::letter_l;
			case SDL_SCANCODE_M: return key::letter_m;
			case SDL_SCANCODE_N: return key::letter_n;
			case SDL_SCANCODE_O: return key::letter_o;
			case SDL_SCANCODE_P: return key::letter_p;
			case SDL_SCANCODE_Q: return key::letter_q;
			case SDL_SCANCODE_R: return key::letter_r;
			case SDL_SCANCODE_S: return key::letter_s;
			case SDL_SCANCODE_T: return key::letter_t;
			case SDL_SCANCODE_U: return key::letter_u;
			case SDL_SCANCODE_V: return key::letter_v;
			case SDL_SCANCODE_W: return key::letter_w;
			case SDL_SCANCODE_X: return key::letter_x;
			case SDL_SCANCODE_Y: return key::letter_y;
			case SDL_SCANCODE_Z: return key::letter_z;
			case SDL_SCANCODE_ESCAPE: return key::escape;
			case SDL_SCANCODE_MINUS: return key::dash;
			case SDL_SCANCODE_EQUALS: return key::equal;
			case SDL_SCANCODE_BACKSPACE: return key::backspace;
			case SDL_SCANCODE_TAB: return key::tab;
			case SDL_SCANCODE_RETURN: return key::enter;
			case SDL_SCANCODE_SEMICOLON: return key::semicolon;
			case SDL_SCANCODE_APOSTROPHE: return key::apostrophe;
			case SDL_SCANCODE_BACKSLASH: return key::backslash;
			case SDL_SCANCODE_COMMA: return key::comma;
			case SDL_SCANCODE_PERIOD: return key::period;
			case SDL_SCANCODE_SLASH: return key::slash;
			case SDL_SCANCODE_SPACE: return key::space;
			case SDL_SCANCODE_CAPSLOCK: return key::caps_lock;
			case SDL_SCANCODE_LEFTBRACKET: return key::bracket_open;
			case SDL_SCANCODE_RIGHTBRACKET: return key::bracket_close;
			case SDL_SCANCODE_LCTRL: return key::control_left;
			case SDL_SCANCODE_RCTRL: return key::control_right;
			case SDL_SCANCODE_LSHIFT: return key::shift_left;
			case SDL_SCANCODE_RSHIFT: return key::shift_right;
			case SDL_SCANCODE_LALT: return key::alt_left;
			case SDL_SCANCODE_RALT: return key::alt_right;
			case SDL_SCANCODE_LEFT: return key::arrow_left;
			case SDL_SCANCODE_RIGHT: return key::arrow_right;
			case SDL_SCANCODE_UP: return key::arrow_up;
			case SDL_SCANCODE_DOWN: return key::arrow_down;
			case SDL_SCANCODE_F1: return key::f1;
			case SDL_SCANCODE_F2: return key::f2;
			case SDL_SCANCODE_F3: return key::f3;
			case SDL_SCANCODE_F4: return key::f4;
			case SDL_SCANCODE_F5: return key::f5;
			case SDL_SCANCODE_F6: return key::f6;
			case SDL_SCANCODE_F7: return key::f7;
			case SDL_SCANCODE_F8: return key::f8;
			case SDL_SCANCODE_F9: return key::f9;
			case SDL_SCANCODE_F10: return key::f10;
			case SDL_SCANCODE_F11: return key::f11;
			case SDL_SCANCODE_F12: return key::f12;
			default: return std::nullopt;
		}
	}
} // namespace nes::app::sdl
