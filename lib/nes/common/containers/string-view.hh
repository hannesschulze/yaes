#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/types.hh"

namespace nes
{
	/// Non-owning view into a (not necessarily null-terminated) string.
	class string_view
	{
		span<char const> data_;

	public:
		explicit string_view() = default;
		explicit string_view(span<char const>);
		explicit string_view(char const* data, u32 length);
		string_view(char const* c_string);

		using iterator = span<char const>::iterator;

		auto operator[](u32 const index) const -> char { return data_[index]; }
		auto get_span() const -> span<char const> { return data_; }
		auto get_data() const -> char const* { return data_.get_data(); }
		auto get_length() const -> u32 { return data_.get_length(); }
		auto is_empty() const -> bool { return data_.is_empty(); }
		auto begin() const -> iterator { return data_.begin(); }
		auto end() const -> iterator { return data_.end(); }

		// String helpers

		auto get_first() const -> char;
		auto get_last() const -> char;
		auto contains(char) const -> bool;
		auto substring(u32 first) const -> string_view;
		auto substring(u32 first, u32 length) const -> string_view;
	};

	auto operator==(string_view, string_view) -> bool;
	auto operator!=(string_view, string_view) -> bool;
} // namespace nes