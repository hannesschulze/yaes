#pragma once

#include "nes/common/types.hh"
#include "nes/common/span.hh"
#include "nes/common/path-view.hh"
#include <string_view>

namespace nes
{
	/// A mutable path with a fixed-length buffer.
	class path_builder
	{
		span<char> buffer_;
		u32 length_{ 0 };

	public:
		explicit path_builder(span<char>);

		auto get_components() const -> path_view { return path_view{ get_path() }; }
		auto get_path() const -> std::string_view { return std::string_view{ buffer_.get_data(), length_ }; }

		auto push(std::string_view) -> bool;
		auto pop() -> bool;
	};

	/// A path builder with a buffer embedded into it.
	template<u32 Capacity>
	class path_buffer : public path_builder
	{
		char buffer_[Capacity];

	public:
		explicit path_buffer()
			: path_builder{ buffer_ }
		{
		}
	};
} // namespace nes