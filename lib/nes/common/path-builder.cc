#include "nes/common/path-builder.hh"

namespace nes
{
	path_builder::path_builder(char* buffer, u32 const buffer_length)
		: buffer_{ buffer }
		, buffer_length_{ buffer_length }
	{
		// Initialize the path to "/" if possible.
		if (buffer_length > 0) { buffer_[actual_length_++] = '/'; }
	}

	auto path_builder::push(std::string_view component) -> bool
	{
		// Trim leading and trailing slashes
		while (!component.empty() && component[0] == '/') { component = component.substr(1); }
		while (!component.empty() && component[component.length() - 1] == '/') { component = component.substr(0, component.length() - 1); }
		if (component.empty() || std::find(component.begin(), component.end(), '/') != component.end()) { return false; }

		// Check if there is enough remaining capacity for "/<component>"
		auto const add_leading_slash = actual_length_ == 0 || buffer_[actual_length_ - 1] != '/';
		auto const length = component.length() + (add_leading_slash ? 1 : 0);
		if (actual_length_ + length > buffer_length_) { return false; }

		// Append "/<component>"
		if (add_leading_slash) { buffer_[actual_length_++] = '/'; }
		for (auto i = u32{ 0 }; i < component.length(); ++i) { buffer_[actual_length_++] = component[i]; }

		return true;
	}

	auto path_builder::pop() -> bool
	{
		if (actual_length_ == 0 || buffer_[actual_length_ - 1] == '/') { return false; }

		// Find the start of the last component.
		auto last_start = actual_length_ - 1;
		while (last_start > 0 && buffer_[last_start] != '/') { last_start -= 1; }

		// Drop the slash if we are not at the root.
		if (last_start == 0) { last_start += 1; }

		// Drop everything including the trailing slash.
		actual_length_ = last_start;
		return true;
	}
} // namespace nes