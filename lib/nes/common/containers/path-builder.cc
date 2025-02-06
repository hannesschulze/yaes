#include "nes/common/containers/path-builder.hh"

namespace nes
{
	path_builder::path_builder(span<char> const buffer)
		: buffer_{ buffer }
	{
		// Initialize the path to "/" if possible.
		if (buffer_.get_length() > 0) { buffer_[length_++] = '/'; }
	}

	auto path_builder::push(string_view component) -> status
	{
		// Trim leading and trailing slashes
		while (!component.is_empty() && component.get_first() == '/') { component = component.substring(1); }
		while (!component.is_empty() && component.get_last() == '/') { component = component.substring(0, component.get_length() - 1); }
		if (component.is_empty() || component.contains('/')) { return status::error_invalid_path; }

		// Check if there is enough remaining capacity for "/<component>"
		auto const add_leading_slash = length_ == 0 || buffer_[length_ - 1] != '/';
		auto const length = component.get_length() + (add_leading_slash ? 1 : 0);
		if (length_ + length > buffer_.get_length()) { return status::error_buffer_overflow; }

		// Append "/<component>"
		if (add_leading_slash) { buffer_[length_++] = '/'; }
		for (auto i = u32{ 0 }; i < component.get_length(); ++i) { buffer_[length_++] = component[i]; }

		return status::success;
	}

	auto path_builder::pop() -> status
	{
		if (length_ == 0 || buffer_[length_ - 1] == '/') { return status::error_invalid_path; }

		// Find the start of the last component.
		auto last_start = length_ - 1;
		while (last_start > 0 && buffer_[last_start] != '/') { last_start -= 1; }

		// Drop the slash if we are not at the root.
		if (last_start == 0) { last_start += 1; }

		// Drop everything including the trailing slash.
		length_ = last_start;
		return status::success;
	}
} // namespace nes