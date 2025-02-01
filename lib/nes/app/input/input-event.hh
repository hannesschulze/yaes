#pragma once

#include "nes/app/input/key.hh"

namespace nes::app
{
	class input_event
	{
	public:
		enum class type
		{
			none,
			key_down,
			key_up,
		};

	private:
		type type_{ type::none };
		key key_{};

	public:
		constexpr explicit input_event() = default;

		static constexpr auto key_down(key const key) -> input_event { return input_event{ type::key_down, key }; }
		static constexpr auto key_up(key const key) -> input_event { return input_event{ type::key_up, key }; }

		constexpr operator bool() const { return type_ != type::none; }

		constexpr auto get_type() const -> type { return type_; }
		constexpr auto get_key() const -> key { return key_; }

	private:
		constexpr explicit input_event(type const type, key const key)
			: type_{ type }
			, key_{ key }
		{
		}
	};

	constexpr auto operator==(input_event const lhs, input_event const rhs)
	{
		return lhs.get_type() == rhs.get_type() && lhs.get_key() == rhs.get_key();
	}

	constexpr auto operator!=(input_event const lhs, input_event const rhs) { return !(lhs == rhs); }
} // namespace nes::app