#pragma once

#include <cstdint>

namespace nes
{
	/// Buttons pressed on a controller.
	class button_mask
	{
		std::uint8_t raw_value_{ 0 };

	public:
		constexpr explicit button_mask() = default;

		static constexpr auto from_raw_value(std::uint8_t const raw_value) -> button_mask
		{
			return button_mask{ raw_value };
		}

		constexpr auto add(button_mask const other) -> void { raw_value_ |= other.raw_value_; }
		constexpr auto remove(button_mask const other) -> void { raw_value_ ^= other.raw_value_; }
		constexpr auto contains(button_mask const other) const -> bool { return (raw_value_ & other.raw_value_) == other.raw_value_; }
		constexpr auto is_empty() const -> bool { return raw_value_ == 0; }
		constexpr auto get_raw_value() const -> std::uint8_t { return raw_value_; }

	private:
		constexpr explicit button_mask(std::uint8_t const raw_value)
			: raw_value_{ raw_value }
		{
		}
	};

	namespace buttons
	{
		inline constexpr auto a = button_mask::from_raw_value(1 << 0);
		inline constexpr auto b = button_mask::from_raw_value(1 << 1);
		inline constexpr auto select = button_mask::from_raw_value(1 << 2);
		inline constexpr auto start = button_mask::from_raw_value(1 << 3);
		inline constexpr auto up = button_mask::from_raw_value(1 << 4);
		inline constexpr auto down = button_mask::from_raw_value(1 << 5);
		inline constexpr auto left = button_mask::from_raw_value(1 << 6);
		inline constexpr auto right = button_mask::from_raw_value(1 << 7);
	} // namespace buttons
} // namespace nes