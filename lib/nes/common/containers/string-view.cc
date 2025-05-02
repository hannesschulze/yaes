#include "nes/common/containers/string-view.hh"
#include "nes/common/debug.hh"

namespace nes
{
	namespace
	{
		auto compute_length(char const* c_string) -> u32
		{
			auto res = u32{ 0 };
			while (*c_string)
			{
				res += 1;
				c_string++;
			}
			return res;
		}

		auto to_uppercase(char const c) -> char
		{
			if (c >= 'a' && c <= 'z')
			{
				return static_cast<char>(c - 'a' + 'A');
			}

			return c;
		}
	} // namespace

	string_view::string_view(span<char const> const data)
		: data_{ data }
	{
	}

	string_view::string_view(char const* data, u32 length)
		: data_{ data, length }
	{
	}

	string_view::string_view(char const* c_string)
		: string_view{ c_string, compute_length(c_string) }
	{
	}

	auto string_view::get_first() const -> char
	{
		NES_ASSERT(!is_empty() && "unable to get character of empty string");
		return data_[0];
	}

	auto string_view::get_last() const -> char
	{
		NES_ASSERT(!is_empty() && "unable to get character of empty string");
		return data_[data_.get_length() - 1];
	}

	auto string_view::compare(string_view const other, case_sensitive const sensitive) const -> bool
	{
		if (get_length() != other.get_length()) { return false; }

		for (auto i = u32{ 0 }; i < get_length(); ++i)
		{
			auto const a = (*this)[i];
			auto const b = other[i];

			if (sensitive == case_sensitive::yes)
			{
				if (a != b) { return false; }
			}
			else
			{
				if (to_uppercase(a) != to_uppercase(b)) { return false; }
			}
		}

		return true;
	}

	auto string_view::contains(char const needle) const -> bool
	{
		for (const auto c : *this)
		{
			if (c == needle) { return true; }
		}
		return false;
	}

	auto string_view::substring(u32 const first) const -> string_view
	{
		return string_view{ data_.subspan(first) };
	}

	auto string_view::substring(u32 const first, u32 const length) const -> string_view
	{
		return string_view{ data_.subspan(first, length) };
	}

	auto string_view::has_prefix(string_view const prefix, case_sensitive const case_sensitive) const -> bool
	{
		if (prefix.get_length() > get_length()) { return false; }
		return substring(0, prefix.get_length()).compare(prefix, case_sensitive);
	}

	auto string_view::has_suffix(string_view const suffix, case_sensitive const case_sensitive) const -> bool
	{
		if (suffix.get_length() > get_length()) { return false; }
		return substring(get_length() - suffix.get_length()).compare(suffix, case_sensitive);
	}

	auto operator==(string_view const lhs, string_view const rhs) -> bool
	{
		return lhs.compare(rhs);
	}

	auto operator!=(string_view const lhs, string_view const rhs) -> bool
	{
		return !lhs.compare(rhs);
	}
} // namespace nes
