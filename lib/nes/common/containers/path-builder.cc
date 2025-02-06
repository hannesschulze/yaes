#include "nes/common/containers/path-builder.hh"

namespace nes
{
	path_builder::path_builder(span<char> const buffer)
		: buffer_{ buffer }
	{
		// Initialize the path to "/" if possible.
		if (buffer_.get_length() > 0) { buffer_[length_++] = '/'; }
	}

	auto path_builder::push(std::string_view component) -> status
	{
		// Trim leading and trailing slashes
		while (!component.empty() && component[0] == '/') { component = component.substr(1); }
		while (!component.empty() && component[component.length() - 1] == '/') { component = component.substr(0, component.length() - 1); }
		if (component.empty() || std::find(component.begin(), component.end(), '/') != component.end()) { return status::error_invalid_path; }

		// Check if there is enough remaining capacity for "/<component>"
		auto const add_leading_slash = length_ == 0 || buffer_[length_ - 1] != '/';
		auto const length = component.length() + (add_leading_slash ? 1 : 0);
		if (length_ + length > buffer_.get_length()) { return status::error_buffer_overflow; }

		// Append "/<component>"
		if (add_leading_slash) { buffer_[length_++] = '/'; }
		for (auto i = u32{ 0 }; i < component.length(); ++i) { buffer_[length_++] = component[i]; }

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