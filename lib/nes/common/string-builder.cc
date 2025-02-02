#include "nes/common/string-builder.hh"

namespace nes
{
	string_builder::string_builder(char* buffer, u32 buffer_length)
		: buffer_{ buffer }
		, buffer_length_{ buffer_length }
	{
	}

	auto string_builder::append_char(char const value) -> string_builder&
	{
		if (actual_length_ == buffer_length_)
		{
			is_good_ = false;
			return *this;
		}

		buffer_[actual_length_] = value;
		actual_length_ += 1;
		return *this;
	}

	auto string_builder::append_string(std::string_view const value) -> string_builder&
	{
		for (auto const c : value)
		{
			append_char(c);
		}

		return *this;
	}

	auto string_builder::append_int(i64 value, number_format const format) -> string_builder&
	{
		if (value < 0)
		{
			append_char('-');
			value += 1; // to prevent lossy conversion to unsigned
			auto abs_value = static_cast<u64>(-value);
			abs_value += 1; // to correct the previous offset
			append_int(abs_value, format);
		}
		else
		{
			append_int(static_cast<u64>(value), format);
		}

		return *this;
	}

	auto string_builder::append_int(u64 value, number_format const format) -> string_builder&
	{
		constexpr auto max_string_length = u32{ 64 }; // worst case is a 64-bit integer in binary representation
		constexpr auto digits = "0123456789abcdef";

		auto base = u64{ 10 };
		switch (format)
		{
			case number_format::binary:
				append_string("0b");
				base = 2;
				break;
			case number_format::octal:
				append_string("0");
				base = 8;
				break;
			case number_format::decimal:
				break;
			case number_format::hexadecimal:
				append_string("0x");
				base = 16;
				break;
		}

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

		append_string(&buf[pos]);
		return *this;
	}

	auto string_builder::append_bool(bool const value) -> string_builder&
	{
		append_string(value ? "true" : "false");
		return *this;
	}

	auto string_builder::append_format(
		std::string_view const fmt, format_arg const* args, u32 const arg_count) -> string_builder&
	{
		// Go through the format string, inserting the arguments for the templates as necessary.
		auto current_arg = u32{ 0 };
		for (auto i = u32{ 0 }; i < fmt.length(); ++i)
		{
			if (fmt[i] == '{')
			{
				i += 1;

				if (fmt[i] == '{') {
					// escaped '{'
					append_char('{');
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
						is_good_ = false;
						break;
					}

					// end of template
					auto const p = std::string_view{ params, params_length };
					if (current_arg < arg_count)
					{
						append_format_arg(args[current_arg], p);
						current_arg += 1;
					}
					else
					{
						// missing argument in parameter pack
						is_good_ = false;
					}
				}
			}
			else if (fmt[i] == '}')
			{
				i += 1;

				if (fmt[i] == '}')
				{
					// escaped '}'
					append_char('}');
				}
				else
				{
					// unmatched '}'
					is_good_ = false;
					break;
				}
			}
			else
			{
				// normal character in the template string
				append_char(fmt[i]);
			}
		}

		if (current_arg < arg_count)
		{
			// not all supplied arguments were used
			is_good_ = false;
		}

		return *this;
	}

	auto string_builder::append_format_arg(format_arg const arg, std::string_view const params) -> string_builder&
	{
		auto const parse_params = [&]
		{
			if (!params.empty())
			{
				is_good_ = false;
			}
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
				out_format = number_format::decimal;
				is_good_ = false;
			}
		};

		switch (arg.type)
		{
			case format_arg_type::string:
			{
				parse_params();
				append_string(arg.string);
				break;
			}
			case format_arg_type::character:
			{
				parse_params();
				append_char(arg.character);
				break;
			}
			case format_arg_type::signed_integer:
			{
				auto format = number_format{};
				parse_params_integer(format);
				append_int(arg.signed_integer, format);
				break;
			}
			case format_arg_type::unsigned_integer:
			{
				auto format = number_format{};
				parse_params_integer(format);
				append_int(arg.unsigned_integer, format);
				break;
			}
			case format_arg_type::boolean:
			{
				parse_params();
				append_bool(arg.character);
				break;
			}
		}

		return *this;
	}
} // namespace nes