#pragma once

#include "nes/common/types.hh"

namespace nes::app
{
	/// Keyboard modifiers.
	class modifier_mask
	{
		u8 raw_value_{ 0 };

	public:
		constexpr explicit modifier_mask() = default;

		static constexpr auto from_raw_value(u8 const raw_value) -> modifier_mask
		{
			return modifier_mask{ raw_value };
		}

		constexpr auto add(modifier_mask const other) -> void { raw_value_ |= other.raw_value_; }
		constexpr auto remove(modifier_mask const other) -> void { raw_value_ ^= other.raw_value_; }
		constexpr auto contains(modifier_mask const other) const -> bool { return (raw_value_ & other.raw_value_) == other.raw_value_; }
		constexpr auto is_empty() const -> bool { return raw_value_ == 0; }
		constexpr auto get_raw_value() const -> u8 { return raw_value_; }

	private:
		constexpr explicit modifier_mask(u8 const raw_value)
			: raw_value_{ raw_value }
		{
		}
	};

	namespace modifiers
	{
		inline constexpr auto control = modifier_mask::from_raw_value(1 << 0);
		inline constexpr auto shift = modifier_mask::from_raw_value(1 << 1);
		inline constexpr auto alt = modifier_mask::from_raw_value(1 << 2);
	} // namespace modifiers
} // namespace nes::app