// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/texture.hh>
#include <phoenix/vdfs.hh>

#include <flags.h>
#include <fmt/format.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <iostream>

#include "config.hh"

namespace px = phoenix;

static constexpr const auto HELP_MESSAGE =
    R"(USAGE
    ztex -v
    ztex -h
    ztex [ -f FILE [-e VDF] ] [-o PATH] [-a]
    ztex [ -f FILE [-e VDF] ] [-o PATH] [-m LEVEL]

DESCRIPTION
    Converts ZenGin textures from the ZTEX to the TGA format. Optionally, dumps all
    mipmap versions of the texture.

OPTIONS
    -v, --version                  Print the version of ztex
    -h, --help                     Print this help message
    -f FILE, --input FILE          If specified, reads the given file from disk instead of
                                   stdin (unless -e is specified).
    -e VDF, --vdf VDF              Instead of reading FILE directly from disk, extract it
                                   from VDF instead.
    -o PATH, --output PATH         Write data to the given path instead of stdout (required with -a).
    -m LEVEL, --mipmap LEVEL       Instead of dumping the largest mipmap, dump the mipmap
                                   with the given LEVEL
    -a, --all-mipmaps              Dump all mipmaps of the texture to the current working directory
                                   or PATH if (-o) is specified.

VERSION
    phoenix ztex v{}
)";

static void write_tga(const std::optional<std::string>& file,
                      const std::vector<std::uint8_t>& data,
                      std::uint32_t width,
                      std::uint32_t height) {
	if (!file) {
		stbi_write_tga_to_func([](void*, void* data, int size) { std::cout.write(static_cast<char*>(data), size); },
		                       nullptr,
		                       (std::int32_t) width,
		                       (std::int32_t) height,
		                       4,
		                       data.data());
	} else {
		stbi_write_tga((*file).c_str(), (std::int32_t) width, (std::int32_t) height, 4, data.data());
	}
}

px::buffer open_buffer(const std::optional<std::string>& input, const std::optional<std::string>& vdf) {
	if (input) {
		if (vdf) {
			auto container = px::vdf_file::open(*vdf);
			if (auto* entry = container.find_entry(*input); entry != nullptr) {
				return entry->open();
			} else {
				fmt::print(stderr, "the file named {} was not found in the VDF {}", *input, *vdf);
				return phoenix::buffer::empty();
			}
		} else {
			return phoenix::buffer::mmap(*input);
		}
	} else {
		std::vector<std::byte> data {};

		while (!std::cin.eof()) {
			data.push_back(static_cast<std::byte>(std::cin.get()));
		}

		if (data.empty()) {
			fmt::print(stderr, "no data provided via stdin");
			return phoenix::buffer::empty();
		}

		// remove the EOF byte
		data.pop_back();
		return phoenix::buffer::of(std::move(data));
	}
}

int main(int argc, char** argv) {
	const flags::args args {argc, argv};

	if (args.get<bool>("v") || args.get<bool>("version")) {
		fmt::print("ztex v{}\n", ZTEX_VERSION);
	} else if (args.get<bool>("h") || args.get<bool>("help")) {
		fmt::print(HELP_MESSAGE, ZTEX_VERSION);
	} else {
		try {
			auto input = args.get<std::string>("f");
			if (!input)
				input = args.get<std::string>("file");

			auto vdf = args.get<std::string>("e");
			if (!vdf)
				vdf = args.get<std::string>("vdf");

			auto output = args.get<std::string>("o");
			if (!output)
				output = args.get<std::string>("output");

			auto level = args.get<int>("m");
			if (!level)
				level = args.get<int>("mipmap");

			auto in = open_buffer(input, vdf);
			if (in == px::buffer::empty())
				return EXIT_FAILURE;

			auto texture = phoenix::texture::parse(in);

			if (args.get<bool>("a") || args.get<bool>("all-mipmaps")) {
				if (!std::filesystem::is_directory(*output)) {
					fmt::print(stderr, "the output directory does not exist.\n");
					return EXIT_FAILURE;
				}

				for (std::uint32_t i = 0; i < texture.mipmaps(); ++i) {
					write_tga(fmt::format("{}/mip{}.tga", output.value_or("."), i),
					          texture.as_rgba8(i),
					          texture.mipmap_width(i),
					          texture.mipmap_height(i));
				}
			} else {
				if (level.value_or(0) > texture.mipmaps() - 1) {
					fmt::print(stderr,
					           "mipmap {} not available. mipmaps 0 to {} are available",
					           *level,
					           texture.mipmaps() - 1);
					return EXIT_FAILURE;
				}

				write_tga(output,
				          texture.as_rgba8(level.value_or(0)),
				          texture.mipmap_width(level.value_or(0)),
				          texture.mipmap_height(level.value_or(0)));
			}
		} catch (const std::exception& e) {
			fmt::print(stderr, "cannot convert texture: {}", e.what());
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
