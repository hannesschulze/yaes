#pragma once

#include "nes/common/types.hh"
#include <string_view>

namespace nes
{
	enum class number_format
	{
		decimal,
		hexadecimal,
		octal,
		binary,
	};

	/// Builds a string into a fixed-length buffer.
	class string_builder
	{
		char* buffer_{ nullptr };
		u32 buffer_length_{ 0 };
		u32 actual_length_{ 0 };
		bool is_good_{ true };

		enum class format_arg_type
		{
			string,
			character,
			signed_integer,
			unsigned_integer,
			boolean,
		};

		struct format_arg
		{
			format_arg_type type;
			union
			{
				std::string_view string;
				char character;
				i64 signed_integer;
				u64 unsigned_integer;
				bool boolean;
			};

			format_arg() = delete;

			explicit format_arg(char const* v) : type{ format_arg_type::string } { string = v; }
			explicit format_arg(std::string_view const v) : type{ format_arg_type::string } { string = v; }
			explicit format_arg(char const v) : type{ format_arg_type::character } { character = v; }
			explicit format_arg(unsigned char const v) : format_arg{ static_cast<char>(v) } {}
			explicit format_arg(long long const v) : type{ format_arg_type::signed_integer } { signed_integer = v; }
			explicit format_arg(unsigned long long const v) : type{ format_arg_type::unsigned_integer } { unsigned_integer = v; }
			explicit format_arg(short const v) : format_arg{ static_cast<signed long long>(v) } {}
			explicit format_arg(unsigned short const v) : format_arg{ static_cast<unsigned long long>(v) } {}
			explicit format_arg(int const v) : format_arg{ static_cast<signed long long>(v) } {}
			explicit format_arg(unsigned int const v) : format_arg{ static_cast<unsigned long long>(v) } {}
			explicit format_arg(long const v) : format_arg{ static_cast<signed long long>(v) } {}
			explicit format_arg(unsigned long const v) : format_arg{ static_cast<unsigned long long>(v) } {}
			explicit format_arg(bool const v) : type{ format_arg_type::boolean } { boolean = v; }
		};

	public:
		explicit string_builder(char* buffer, u32 buffer_length);

		/// Get the resulting string.
		auto get_result() const -> std::string_view { return std::string_view{ buffer_, actual_length_ }; }

		/// Returns false after an invalid format string or buffer overflow.
		auto is_good() const -> bool { return is_good_; }

		auto append_char(char) -> string_builder&;
		auto append_string(std::string_view) -> string_builder&;
		auto append_int(i64, number_format = number_format::decimal) -> string_builder&;
		auto append_int(u64, number_format = number_format::decimal) -> string_builder&;
		auto append_bool(bool) -> string_builder&;

		/// Print a templated string.
		///
		/// Placeholders are signified by `{<params>}` where `params` contains optional formatting parameters:
		///  - for integers either "d" (the default), "x", "o", or "b"
		///  - for any other type, no parameters are supported.
		///
		/// Curly braces can be escaped by repeating them twice like {{this}}.
		template<typename... Args>
		auto append_format(std::string_view const fmt, Args... args) -> string_builder&
		{
			constexpr u32 arg_count = sizeof...(Args);
			// Add another argument at the end so we don't have an empty array expression.
			const format_arg arg_wrappers[arg_count + 1] = { format_arg{ args }..., format_arg(0) };

			return append_format(fmt, arg_wrappers, arg_count);
		}

	private:
		auto append_format(std::string_view fmt, format_arg const* args, u32 arg_count) -> string_builder&;
		auto append_format_arg(format_arg, std::string_view params) -> string_builder&;
	};
} // namespace nes