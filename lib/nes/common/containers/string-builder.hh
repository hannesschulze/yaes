#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/containers/string-view.hh"
#include "nes/common/types.hh"
#include "nes/common/status.hh"
#include "nes/common/utils.hh"

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
		template<u32 Capacity>
		friend class string_buffer;

		span<char> buffer_;
		u32 length_{ 0 };

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
				string_view string;
				char character;
				i64 signed_integer;
				u64 unsigned_integer;
				bool boolean;
			};

			format_arg() = delete;

			explicit format_arg(char const* v) : type{ format_arg_type::string } { string = v; }
			explicit format_arg(string_view const v) : type{ format_arg_type::string } { string = v; }
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
		explicit string_builder(span<char>);

		/// Get the resulting string.
		auto get_result() const -> string_view { return string_view{ buffer_.get_data(), length_ }; }

		auto append_char(char) -> status;
		auto append_string(string_view) -> status;
		auto append_int(i64, number_format = number_format::decimal) -> status;
		auto append_int(u64, number_format = number_format::decimal) -> status;
		auto append_bool(bool) -> status;

		/// Print a templated string.
		///
		/// Placeholders are signified by `{<params>}` where `params` contains optional formatting parameters:
		///  - for integers either "d" (the default), "x", "o", or "b"
		///  - for any other type, no parameters are supported.
		///
		/// Curly braces can be escaped by repeating them twice like {{this}}.
		template<typename... Args>
		auto append_format(string_view const fmt, Args... args) -> status
		{
			constexpr u32 arg_count = sizeof...(Args);
			// Add another argument at the end so we don't have an empty array expression.
			const format_arg arg_wrappers[arg_count + 1] = { format_arg{ args }..., format_arg(0) };

			return append_format(fmt, arg_wrappers, arg_count);
		}

		auto remove_last() -> status;
		auto clear() -> void;

	private:
		auto append_format(string_view fmt, format_arg const* args, u32 arg_count) -> status;
		auto append_format_arg(format_arg, string_view params) -> status;
	};

	/// A string builder with a buffer embedded into it.
	template<u32 Capacity>
	class string_buffer : public string_builder
	{
		char buffer_[Capacity];

	public:
		explicit string_buffer()
			: string_builder{ buffer_ }
		{
		}

		string_buffer(string_buffer const& other)
			: string_builder{ buffer_ }
		{
			copy(other.buffer_, buffer_, Capacity);
			length_ = other.length_;
		}

		auto operator=(string_buffer const& other)
		{
			copy(other.buffer_, buffer_, Capacity);
			length_ = other.length_;
		}
	};
} // namespace nes