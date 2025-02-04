#include "nes/common/string-builder.hh"

namespace nes
{
	string_builder::string_builder(span<char> const buffer)
		: buffer_{ buffer }
	{
	}

	auto string_builder::append_char(char const value) -> status
	{
		if (length_ == buffer_.get_length()) { return status::error_buffer_overflow; }

		buffer_[length_] = value;
		length_ += 1;
		return status::success;
	}

	auto string_builder::append_string(std::string_view const value) -> status
	{
		auto copy = *this;
		for (auto const c : value)
		{
			if (auto const s = copy.append_char(c); s != status::success) { return s; }
		}

		*this = copy;
		return status::success;
	}

	auto string_builder::append_int(i64 value, number_format const format) -> status
	{
		auto copy = *this;
		if (value < 0)
		{
			if (auto const s = copy.append_char('-'); s != status::success) { return s; }
			value += 1; // to prevent lossy conversion to unsigned
			auto abs_value = static_cast<u64>(-value);
			abs_value += 1; // to correct the previous offset
			if (auto const s = copy.append_int(abs_value, format); s != status::success) { return s; }
		}
		else
		{
			if (auto const s = copy.append_int(static_cast<u64>(value), format); s != status::success) { return s; }
		}

		*this = copy;
		return status::success;
	}

	auto string_builder::append_int(u64 value, number_format const format) -> status
	{
		constexpr auto max_string_length = u32{ 64 }; // worst case is a 64-bit integer in binary representation
		constexpr auto digits = "0123456789abcdef";

		auto copy = *this;
		auto base = u64{ 10 };
		auto prefix = "";
		switch (format)
		{
			case number_format::binary:
				prefix = "0b";
				base = 2;
				break;
			case number_format::octal:
				prefix = "0";
				base = 8;
				break;
			case number_format::decimal:
				break;
			case number_format::hexadecimal:
				prefix = "0x";
				base = 16;
				break;
		}
		if (auto const s = copy.append_string(prefix); s != status::success) { return s; }

		char buf[max_string_length + 1] = {}; // null-terminated
		auto pos = max_string_length;
		// Go through each digit in reverse order by continuously dividing the integer by the base.
		while (value)
		{
			pos--;

			buf[pos] = digits[value % base]; // Convert digit to char representation
			value /= base;
		}
		if (pos == max_string_length)
		{
			// Special case 0 would not print any character otherwise.
			pos--;
			buf[pos] = '0';
		}

		if (const auto s = copy.append_string(&buf[pos]); s != status::success) { return s; }

		*this = copy;
		return status::success;
	}

	auto string_builder::append_bool(bool const value) -> status
	{
		return append_string(value ? "true" : "false");
	}

	auto string_builder::append_format(std::string_view const fmt, format_arg const* args, u32 const arg_count) -> status
	{
		auto copy = *this;

		// Go through the format string, inserting the arguments for the templates as necessary.
		auto current_arg = u32{ 0 };
		for (auto i = u32{ 0 }; i < fmt.length(); ++i)
		{
			if (fmt[i] == '{')
			{
				i += 1;

				if (fmt[i] == '{') {
					// escaped '{'
					if (auto const s = copy.append_char('{'); s != status::success) { return s; }
				}
				else
				{
					// start of template
					auto const params = &fmt[i]; // remember start position
					auto params_length = u32{ 0 };
					// skip to the end of the template
					while (fmt[i] && fmt[i] != '}')
					{
						params_length++;
						i++;
					}
					if (fmt[i] != '}')
					{
						// unmatched '{'
						return status::error_invalid_format_string;
					}

					// end of template
					auto const p = std::string_view{ params, params_length };
					if (current_arg < arg_count)
					{
						if (auto const s = copy.append_format_arg(args[current_arg], p); s != status::success) { return s; }
						current_arg += 1;
					}
					else
					{
						// missing argument in parameter pack
						return status::error_invalid_format_string;
					}
				}
			}
			else if (fmt[i] == '}')
			{
				i += 1;

				if (fmt[i] == '}')
				{
					// escaped '}'
					if (auto const s = copy.append_char('}'); s != status::success) { return s; }
				}
				else
				{
					// unmatched '}'
					return status::error_invalid_format_string;
				}
			}
			else
			{
				// normal character in the template string
				if (auto const s = copy.append_char(fmt[i]); s != status::success) { return s; }
			}
		}

		if (current_arg < arg_count)
		{
			// not all supplied arguments were used
			return status::error_invalid_format_string;
		}

		*this = copy;
		return status::success;
	}

	auto string_builder::append_format_arg(format_arg const arg, std::string_view const params) -> status
	{
		auto const parse_params = [&]
		{
			if (!params.empty())
			{
				return status::error_invalid_format_string;
			}

			return status::success;
		};

		auto const parse_params_integer = [&](number_format& out_format)
		{
			if (params.empty() || params == "d")
			{
				out_format = number_format::decimal;
			}
			else if (params == "x")
			{
				out_format = number_format::hexadecimal;
			}
			else if (params == "o")
			{
				out_format = number_format::octal;
			}
			else if (params == "b")
			{
				out_format = number_format::binary;
			}
			else
			{
				return status::error_invalid_format_string;
			}

			return status::success;
		};

		switch (arg.type)
		{
			case format_arg_type::string:
			{
				if (auto const s = parse_params(); s != status::success) { return s; }
				return append_string(arg.string);
			}
			case format_arg_type::character:
			{
				if (auto const s = parse_params(); s != status::success) { return s; }
				return append_char(arg.character);
			}
			case format_arg_type::signed_integer:
			{
				auto format = number_format{};
				if (auto const s = parse_params_integer(format); s != status::success) { return s; }
				return append_int(arg.signed_integer, format);
			}
			case format_arg_type::unsigned_integer:
			{
				auto format = number_format{};
				if (auto const s = parse_params_integer(format); s != status::success) { return s; }
				return append_int(arg.unsigned_integer, format);
			}
			case format_arg_type::boolean:
			{
				if (auto const s = parse_params(); s != status::success) { return s; }
				return append_bool(arg.character);
			}
		}

		return status::error_invalid_format_string;
	}
} // namespace nes