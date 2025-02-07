#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <filesystem>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace
{
	enum class type
	{
		mask,
		image,
	};

	enum class status
	{
		ok,
		invalid,
	};

	struct rgba
	{
		explicit rgba() = default;

		explicit rgba(std::uint8_t const r, std::uint8_t const g, std::uint8_t const b, std::uint8_t const a)
			: r{ r }
			, g{ g }
			, b{ b }
			, a{ a }
		{
		}

		static auto from_hex(std::uint32_t const hex) -> rgba
		{
			auto res = rgba{};
			res.r = static_cast<std::uint8_t>((hex & 0xFF000000) >> 24);
			res.g = static_cast<std::uint8_t>((hex & 0x00FF0000) >> 16);
			res.b = static_cast<std::uint8_t>((hex & 0x0000FF00) >> 8);
			res.a = static_cast<std::uint8_t>((hex & 0x000000FF) >> 0);
			return res;
		}

		std::uint8_t r{ 0 };
		std::uint8_t g{ 0 };
		std::uint8_t b{ 0 };
		std::uint8_t a{ 0 };
	};

	auto operator==(rgba const lhs, rgba const rhs) -> bool
	{
		return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
	}

	/// A decoded image.
	class image
	{
		std::string name_;
		std::unique_ptr<unsigned char[], void(*)(void*)> data_;
		unsigned width_{ 0 };
		unsigned height_{ 0 };

		explicit image(std::string name, unsigned char* data, unsigned const width, unsigned const height)
			: name_{ std::move(name) }
			, data_{ data, stbi_image_free }
			, width_{ width }
			, height_{ height }
		{
		}

	public:
		static auto load(char const* path) -> std::optional<image>
		{
			int x, y;
			auto const data = stbi_load(path, &x, &y, nullptr, 4);
			if (!data) { return std::nullopt; }
			assert(x >= 0);
			assert(y >= 0);

			auto name = std::string{ std::filesystem::path(path).stem() };
			for (auto& c : name)
			{
				if (c >= 'a' && c <= 'z') { continue; }
				if (c >= '0' && c <= '9') { continue; }
				if (c >= 'A' && c <= 'Z')
				{
					c = (c - 'A') + 'a';
					continue;
				}
				c = '_';
			}

			return image{ std::move(name), data, static_cast<unsigned>(x), static_cast<unsigned>(y) };
		}

		auto get_name() const -> std::string_view { return name_; }
		auto get_width() const -> unsigned { return width_; }
		auto get_height() const -> unsigned { return height_; }
		auto get(unsigned const x, unsigned const y) const -> rgba
		{
			assert(x < width_);
			assert(y < height_);

			auto const offset = y * width_ + x;
			return rgba{ data_[offset * 4 + 0], data_[offset * 4 + 1], data_[offset * 4 + 2], data_[offset * 4 + 3] };
		}
	};

	auto decode_type(std::string_view const t) -> std::optional<type>
	{
		if (t == "mask") { return type::mask; }
		if (t == "image") { return type::image; }
		return std::nullopt;
	}

	auto encode_mask(std::stringstream& header, std::stringstream& source, image const& img) -> status
	{
		if (img.get_width() != 8) { return status::invalid; }
		if (img.get_height() != 8) { return status::invalid; }

		header << "\textern mask_tile const " << img.get_name() << ";\n";

		source << "\textern auto const " << img.get_name() << " = mask_tile\n";
		source << "\t{\n";
		for (auto y = unsigned{ 0 }; y < img.get_height(); ++y)
		{
			source << "\t\t0b";
			for (auto x = unsigned{ 0 }; x < img.get_width(); ++x)
			{
				if (img.get(x, y).a > 0)
				{
					source << '1';
				}
				else
				{
					source << '0';
				}
			}
			source << ",\n";
		}
		source << "\t};\n\n";

		return status::ok;
	}

	/// Generate the constructor call for a single image tile.
	auto generate_image_tile(std::stringstream& str, image const& img, unsigned const x, unsigned const y) -> status
	{
		str << "\t{\n";
		for (auto y_px = unsigned{ 0 }; y_px < 8; ++y_px)
		{
			str << "\t\t";
			for (auto x_px = unsigned{ 0 }; x_px < 8; ++x_px)
			{
				auto const color = img.get(x * 8 + x_px, y * 8 + y_px);
				str << "color{ ";
				if (color.a == 0) { str << "0x0"; }
				else if (color == rgba::from_hex(0x004058FF)) { str << "0x1"; }
				else if (color == rgba::from_hex(0x002B3BFF)) { str << "0x2"; }
				else if (color == rgba::from_hex(0xFCFCFCFF)) { str << "0x3"; }
				else if (color == rgba::from_hex(0x008888FF)) { str << "0x4"; }
				else if (color == rgba::from_hex(0xE45C10FF)) { str << "0x5"; }
				else if (color == rgba::from_hex(0x881400FF)) { str << "0x6"; }
				else if (color == rgba::from_hex(0xFCFCFCFF)) { str << "0x7"; }
				else if (color == rgba::from_hex(0xBCBCBCFF)) { str << "0x8"; }
				else if (color == rgba::from_hex(0x000000FF)) { str << "0x9"; }
				else { return status::invalid; }
				str << " }, ";
			}
			str << "\n";
		}
		str << "\t}";

		return status::ok;
	}

	auto encode_image(std::stringstream& header, std::stringstream& source, image const& img) -> status
	{
		if (img.get_width() % 8 != 0) { return status::invalid; }
		if (img.get_height() % 8 != 0) { return status::invalid; }

		if (img.get_width() == 8 && img.get_height() == 8)
		{
			header << "\textern image_tile const " << img.get_name() << ";\n";

			source << "\textern auto const " << img.get_name() << " = image_tile\n";
			if (generate_image_tile(source, img, 0, 0) != status::ok) { return status::invalid; }
			source << ";\n\n";
		}
		else
		{
			auto const width_tiles = img.get_width() / 8;
			auto const height_tiles = img.get_height() / 8;

			header << "\textern image_view const " << img.get_name() << ";\n";

			source << "\tstatic image_tile const " << img.get_name() << "_DATA[] = \n";
			source << "\t{\n";
			for (auto y = unsigned{ 0 }; y < height_tiles; ++y)
			{
				for (auto x = unsigned{ 0 }; x < width_tiles; ++x)
				{
					if (generate_image_tile(source, img, x, y) != status::ok) { return status::invalid; }
					source << ",\n";
				}
			}
			source << "\t};\n";
			source << "\textern auto const " << img.get_name()
				<< " = image_view{ " << img.get_name() << "_DATA, " << width_tiles << ", " << height_tiles << " };\n\n";
		}

		return status::ok;
	}

	auto write_file(std::string_view const filename, std::string_view const content) -> status
	{
		auto const filename_str = std::string{ filename };

		auto const fd = open(filename_str.c_str(), O_TRUNC | O_WRONLY | O_CREAT, 0644);
		if (fd == -1)
		{
			perror("open");
			return status::invalid;
		}

		if (write(fd, content.data(), content.length()) == -1)
		{
			perror("write");
			return status::invalid;
		}

		close(fd);
		return status::ok;
	}
} // namespace

int main(int const argc, char** const argv)
{
	if (argc < 5)
	{
		std::cerr << "Usage:\n";
		std::cerr << "  " << argv[0] << " <mask|image> <output-header> <output-source> <files...>" << std::endl;
		return EXIT_FAILURE;
	}

	auto const t = decode_type(argv[1]);
	if (!t)
	{
		std::cerr << "Invalid type: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}
	auto const output_header = std::string_view{ argv[2] };
	auto const output_source = std::string_view{ argv[3] };

	auto header = std::stringstream{};
	auto source = std::stringstream{};

	header << "#pragma once\n";
	header << "\n";
	header << "#include \"nes/app/graphics/mask-tile.hh\"\n";
	header << "#include \"nes/app/graphics/image-tile.hh\"\n";
	header << "#include \"nes/app/graphics/image-view.hh\"\n";
	header << "\n";
	header << "namespace nes::app::tiles\n";
	header << "{\n";

	source << "#include \"nes/app/graphics/mask-tile.hh\"\n";
	source << "#include \"nes/app/graphics/image-tile.hh\"\n";
	source << "#include \"nes/app/graphics/image-view.hh\"\n";
	source << "\n";
	source << "namespace nes::app::tiles\n";
	source << "{\n\n";

	for (auto i = 4; i < argc; ++i)
	{
		auto const img = image::load(argv[i]);
		if (!img)
		{
			std::cerr << "Unable to load file: " << argv[i] << std::endl;
			return EXIT_FAILURE;
		}

		auto res = status::invalid;
		switch (*t)
		{
			case type::mask:
				res = encode_mask(header, source, *img);
				break;
			case type::image:
				res = encode_image(header, source, *img);
				break;
		}

		if (res != status::ok)
		{
			std::cerr << "Input has invalid dimensions: " << argv[i] << std::endl;
			return EXIT_FAILURE;
		}
	}

	header << "} // namespace nes::app::tiles\n";

	source << "} // namespace nes::app::tiles\n";

	if (write_file(output_header, header.str()) != status::ok) { return EXIT_FAILURE; }
	if (write_file(output_source, source.str()) != status::ok) { return EXIT_FAILURE; }

	return EXIT_SUCCESS;
}
