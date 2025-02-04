#include "nes/common/path-view.hh"

namespace nes
{
	auto path_view::iterator::operator*() const -> std::string_view
	{
		return current_ ? std::string_view{ current_, component_length_ } : "";
	}

	auto path_view::iterator::operator++() -> iterator&
	{
		// Move forward to next component
		current_ += component_length_;
		remaining_ -= component_length_;

		// Skip over '/'.
		while (remaining_ > 0 && *current_ == '/')
		{
			current_ += 1;
			remaining_ -= 1;
		}

		// Scan until end of component to find its length.
		component_length_ = 0;
		while (component_length_ < remaining_ && current_[component_length_] != '/')
		{
			component_length_ += 1;
		}

		return *this;
	}

	auto path_view::iterator::operator++(int) -> iterator
	{
		auto const res = *this;
		++*this;
		return res;
	}

	auto path_view::begin() const -> iterator
	{
		return ++iterator{ path_.data(), 0, static_cast<u32>(path_.size()) };
	}

	auto path_view::end() const -> iterator
	{
		return iterator{ path_.data() + path_.size(), 0, 0 };
	}
} // namespace nes