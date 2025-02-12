#include "nes/app/crypto/sha256.hh"
#include "nes/common/utils.hh"

namespace nes::app
{
	namespace
	{
		constexpr u32 round_constants[64] =
			{
			0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
			0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
			0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
			0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
			0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
			0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
			0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
			0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
		};

		auto rightrotate(u32 input, u32 index) -> u32
		{
			return (input >> index) | (input << (32 - index));
		}
	} // namespace

	auto sha256::get(u32 const index) -> u32
	{
		auto const offset = index * 4;
		return static_cast<u32>(
			(data_[offset + 0] << 24) |
			(data_[offset + 1] << 16) |
			(data_[offset + 2] << 8) |
			(data_[offset + 3] << 0));
	}

	auto sha256::set(u32 const index, u32 const value) -> void
	{
		auto const offset = index * 4;
		data_[offset + 0] = static_cast<u8>((value >> 24) & 0xFF);
		data_[offset + 1] = static_cast<u8>((value >> 16) & 0xFF);
		data_[offset + 2] = static_cast<u8>((value >> 8) & 0xFF);
		data_[offset + 3] = static_cast<u8>((value >> 0) & 0xFF);
	}

	auto sha256::hash(span<u8 const> const message) -> sha256
	{
		// https://en.wikipedia.org/wiki/SHA-2#Pseudocode

		auto res = sha256{};

		auto const blocks = message.get_length() / block_size;
		auto const remainder = message.get_length() - blocks * block_size;
		for (auto i = u32{ 0 }; i < blocks; ++i)
		{
			res.combine(message.subspan<block_size>(i * block_size));
		}

		u8 last_blocks[block_size * 2]{};
		auto last_block_length = remainder;
		copy(&message[blocks * block_size], last_blocks, remainder);
		// Add a single 1 bit.
		last_blocks[last_block_length++] = 0b10000000;
		// Fill with 0 bits s.t. there's 8 bytes remaining in the last block.
		auto zeroes = sizeof(last_blocks) - last_block_length - 8;
		while (zeroes >= block_size) { zeroes -= block_size; }
		last_block_length += zeroes;
		// Add the length as big-endian.
		auto const length = static_cast<u64>(message.get_length()) * 8;
		last_blocks[last_block_length++] = static_cast<u8>((length >> 56) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 48) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 40) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 32) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 24) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 16) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 8) & 0xFF);
		last_blocks[last_block_length++] = static_cast<u8>((length >> 0) & 0xFF);
		auto const last_block_count = last_block_length / block_size;
		for (auto i = u32{ 0 }; i < last_block_count; ++i)
		{
			res.combine(span<u8 const>(last_blocks).subspan<block_size>(i * block_size));
		}

		return res;
	}

	sha256::sha256()
	{
		set(0, 0x6a09e667);
		set(1, 0xbb67ae85);
		set(2, 0x3c6ef372);
		set(3, 0xa54ff53a);
		set(4, 0x510e527f);
		set(5, 0x9b05688c);
		set(6, 0x1f83d9ab);
		set(7, 0x5be0cd19);
	}

	auto sha256::combine(span<u8 const, block_size> const chunk) -> void
	{
		u32 w[64]{};

		// copy chunk into first 16 words w[0..15] of the message schedule array
		for (auto i = u32{ 0 }; i < 16; ++i)
		{
			auto const offset = i * 4;
			w[i] = static_cast<u32>(
				(chunk[offset + 0] << 24) |
				(chunk[offset + 1] << 16) |
				(chunk[offset + 2] << 8) |
				(chunk[offset + 3] << 0));
		}

		for (auto i = u32{ 16 }; i < 64; i++)
		{
			auto s0 = rightrotate(w[i-15], 7) ^ rightrotate(w[i-15], 18) ^ (w[i-15] >> 3);
			auto s1 = rightrotate(w[i-2], 17) ^ rightrotate(w[i-2], 19) ^ (w[i-2] >> 10);
			w[i] = w[i-16] + s0 + w[i-7] + s1;
		}
		auto a = get(0);
		auto b = get(1);
		auto c = get(2);
		auto d = get(3);
		auto e = get(4);
		auto f = get(5);
		auto g = get(6);
		auto h = get(7);

		for (auto i = u32{ 0 }; i < 64; i++)
		{
			auto s1 = rightrotate(e, 6) ^ rightrotate(e, 11) ^ rightrotate(e, 25);
			auto ch = (e & f) ^ ((~e) & g);
			auto tmp1 = h + s1 + ch + round_constants[i] + w[i];
			auto s0 = rightrotate(a, 2) ^ rightrotate(a, 13) ^ rightrotate(a, 22);
			auto maj = (a & b) ^ (a & c) ^ (b & c);
			auto tmp2 = s0 + maj;

			h = g;
			g = f;
			f = e;
			e = d + tmp1;
			d = c;
			c = b;
			b = a;
			a = tmp1 + tmp2;
		}

		set(0, get(0) + a);
		set(1, get(1) + b);
		set(2, get(2) + c);
		set(3, get(3) + d);
		set(4, get(4) + e);
		set(5, get(5) + f);
		set(6, get(6) + g);
		set(7, get(7) + h);
	}
} // namespace nes::app