#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/containers/string-view.hh"
#include "nes/common/containers/path-view.hh"
#include "nes/common/types.hh"
#include "nes/common/status.hh"
#include "nes/common/utils.hh"

namespace nes
{
	/// A mutable path with a fixed-length buffer.
	class path_builder
	{
		template<u32 Capacity>
		friend class path_buffer;

		span<char> buffer_;
		u32 length_{ 0 };

	public:
		explicit path_builder(span<char>);

		auto get_components() const -> path_view { return path_view{ get_path() }; }
		auto get_path() const -> string_view { return string_view{ buffer_.get_data(), length_ }; }

		auto push(string_view) -> status;
		auto pop() -> status;
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

		path_buffer(path_buffer const& other)
			: path_builder{ buffer_ }
		{
			copy(other.buffer_, buffer_, Capacity);
			length_ = other.length_;
		}

		auto operator=(path_buffer const& other)
		{
			copy(other.buffer_, buffer_, Capacity);
			length_ = other.length_;
		}
	};
} // namespace nes