#pragma once

namespace nes
{
	/// A status code.
	enum class status
	{
		success,
		error_system_error,
		error_invalid_ines_data,
		error_unsupported_mapper,
		error_unsupported_instruction,
		error_buffer_overflow,
		error_invalid_path,
		error_invalid_format_string,
		error_unknown_file_type,
	};

	constexpr auto to_string(status const status) -> char const*
	{
		switch (status)
		{
			case status::success:
				return "Success";
			case status::error_system_error:
				return "System error";
			case status::error_invalid_ines_data:
				return "Invalid iNES data";
			case status::error_unsupported_mapper:
				return "Unsupported mapper";
			case status::error_unsupported_instruction:
				return "Unsupported instruction";
			case status::error_buffer_overflow:
				return "Buffer overflow";
			case status::error_invalid_path:
				return "Invalid path";
			case status::error_invalid_format_string:
				return "Invalid format string";
			case status::error_unknown_file_type:
				return "Unknown file type";
		}

		return "(invalid)";
	}
} // namespace nes
