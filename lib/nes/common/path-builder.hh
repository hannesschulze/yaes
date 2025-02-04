#pragma once

#include "nes/common/types.hh"
#include "nes/common/path-view.hh"
#include <string_view>

namespace nes
{
	/// A mutable path with a fixed-length buffer.
	class path_builder
	{
		char* buffer_{ nullptr };
		u32 buffer_length_{ 0 };
		u32 actual_length_{ 0 };

	public:
		explicit path_builder(char* buffer, u32 buffer_length);

		auto get_components() const -> path_view { return path_view{ get_path() }; }
		auto get_path() const -> std::string_view { return std::string_view{ buffer_, actual_length_ }; }

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
			: path_builder{ buffer_, Capacity }
		{
		}
	};
} // namespace nes