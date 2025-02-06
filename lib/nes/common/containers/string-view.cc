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

	auto operator==(string_view const lhs, string_view const rhs) -> bool
	{
		if (lhs.get_length() != rhs.get_length()) { return false; }

		for (auto i = u32{ 0 }; i < lhs.get_length(); ++i)
		{
			if (lhs[i] != rhs[i]) { return false; }
		}

		return true;
	}
	auto operator!=(string_view const lhs, string_view const rhs) -> bool
	{
		return !(lhs == rhs);
	}
} // namespace nes