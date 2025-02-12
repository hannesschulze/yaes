#include "nes/app/crypto/aes256.hh"
#include "nes/app/crypto/sha256.hh"
#include "nes/common/utils.hh"
#include "nes/common/containers/string-view.hh"
#include <iostream>
#include <vector>
#include <unistd.h>

auto main(int const argc, char** argv) -> int
{
	if (argc != 2)
	{
		std::cerr << "Usage:\n";
		std::cerr << "  " << argv[0] << " <key> < input > output" << std::endl;
		return EXIT_FAILURE;
	}

	auto const key_str = nes::string_view{ argv[1] };
	auto const key = nes::span{ reinterpret_cast<nes::u8 const*>(key_str.get_data()), key_str.get_length() };
	auto const key_hash = nes::app::sha256::hash(key);

	auto data = std::vector<nes::u8>{};
	nes::u8 buffer[1024]{};
	while (true)
	{
		auto const bytes = read(STDIN_FILENO, buffer, sizeof(buffer));
		if (bytes < 0)
		{
			perror("read");
			return EXIT_FAILURE;
		}

		if (bytes == 0)
		{
			break;
		}

		auto offset = data.size();
		data.resize(offset + bytes);
		nes::copy(buffer, &data[offset], bytes);
	}

	nes::app::aes256::encrypt(nes::span{ data.data(), static_cast<nes::u32>(data.size()) }, key_hash.get_data());

	if (write(STDOUT_FILENO, data.data(), data.size()) < 0)
	{
		perror("write");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}