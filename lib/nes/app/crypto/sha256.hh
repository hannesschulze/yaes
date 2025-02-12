#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/types.hh"

namespace nes::app
{
	class sha256
	{
		static constexpr auto length = u32{ 256 / 8 };
		static constexpr auto block_size = u32{ 512 / 8 };

		u8 data_[length]{};

	public:
		static auto hash(span<u8 const>) -> sha256;

		auto get_data() const -> span<u8 const, length> { return data_; }

	private:
		explicit sha256();

		auto combine(span<u8 const, block_size>) -> void;

		auto get(u32 index) -> u32;
		auto set(u32 index, u32 value) -> void;
	};
} // namespace nes::app