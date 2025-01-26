#pragma once

namespace nes
{
	/// A status code.
	enum class status
	{
		success,
		error_uninitialized,
		error_system_error,
		error_invalid_ines_data,
		error_unsupported_mapper,
		error_unsupported_instruction,
	};

	constexpr auto to_string(status const status) -> char const*
	{
		switch (status)
		{
			case status::success:
				return "Success";
			case status::error_uninitialized:
				return "Uninitialized";
			case status::error_system_error:
				return "System error";
			case status::error_invalid_ines_data:
				return "Invalid iNES data";
			case status::error_unsupported_mapper:
				return "Unsupported mapper";
			case status::error_unsupported_instruction:
				return "Unsupported instruction";
		}

		return "(invalid)";
	}
} // namespace nes