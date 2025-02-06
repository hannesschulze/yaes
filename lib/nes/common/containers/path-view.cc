#include "nes/common/containers/path-view.hh"
#include "nes/common/debug.hh"

namespace nes
{
	auto path_view::iterator::operator*() const -> string_view
	{
		NES_ASSERT(current_ != nullptr && component_length_ > 0 && "invalid iterator");
		return string_view{ current_, component_length_ };
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
		return ++iterator{ path_.get_data(), 0, path_.get_length() };
	}

	auto path_view::end() const -> iterator
	{
		return iterator{ path_.get_data() + path_.get_length(), 0, 0 };
	}
} // namespace nes