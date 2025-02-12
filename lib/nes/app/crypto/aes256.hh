#pragma once

#include "nes/common/containers/span.hh"
#include "nes/common/types.hh"

namespace nes::app::aes256
{
	using key = span<u8 const, 256 / 8>;

	auto encrypt(span<u8>, key) -> void;
	auto decrypt(span<u8>, key) -> void;
} // namespace nes::app::aes256