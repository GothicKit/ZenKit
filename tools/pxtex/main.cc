// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/texture.hh>
#include <phoenix/vdfs.hh>

#include <argh.h>
#include <fmt/format.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <fstream>

#include "config.hh"

static constexpr const auto HELP_MESSAGE = "Usage: pxtex [--version]\n"
										   "       pxtex [-h|--help]\n"
										   "       pxtex <FILE> [-v|--vdf <VDF>] [-o|--output <OUT>] [-a|--all-mipmaps]\n"
										   "       pxtex <FILE> [-v|--vdf <VDF>] [-o|--output <OUT>] [-m|--mipmap <NUM>]\n"
										   "\n"
										   "phoenix pxtex v{}\n"
										   "Convert ZenGin textures from the ZTEX to TGA format.\n";

static void write_tga(const std::string& file, const std::vector<std::uint8_t>& data, std::uint32_t width, std::uint32_t height) {
	if (file.empty()) {
		stbi_write_tga("pxtex.tga", (std::int32_t) width, (std::int32_t) height, 4, data.data());
	} else {
		stbi_write_tga(file.c_str(), (std::int32_t) width, (std::int32_t) height, 4, data.data());
	}
}

int main(int argc, const char** argv) {
	argh::parser args {argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION};

	if (args["--version"]) {
		fmt::print("pxtex v{}\n", PXTEX_VERSION);
		return EXIT_SUCCESS;
	}

	if (args[{"-h", "--help"}]) {
		fmt::print(HELP_MESSAGE, PXTEX_VERSION);
		return EXIT_SUCCESS;
	}

	std::string file;
	if (!(args(1) >> file)) {
		fmt::print(stderr, "please provide an input file.");
		return EXIT_FAILURE;
	}

	std::string output;
	if (!(args({"-o", "--output"}) >> output)) {
		output = "";
	}

	std::uint32_t level = 0;
	if (!(args({"-m", "--mipmap"}) >> level)) {
		level = 0;
	}

	bool all_levels = args[{"-a", "--all-mipmaps"}];

	std::string vdf_in;
	bool input_vdf = false;

	if (args({"-v", "--vdf"}) >> vdf_in) {
		input_vdf = true;
	}

	try {
		phoenix::buffer in = phoenix::buffer::empty();

		if (input_vdf) {
			auto vdf = phoenix::vdf_file::open(vdf_in);

			if (auto* entry = vdf.find_entry(file); entry != nullptr) {
				in = entry->open();
			} else {
				fmt::print(stderr, "the file named {} was not found in the VDF {}", file, vdf_in);
				return EXIT_FAILURE;
			}
		} else {
			in = phoenix::buffer::open(file);
		}

		auto texture = phoenix::texture::parse(in);

		if (all_levels) {
			if (output.empty()) {
				output = fmt::format("{}.tga", file);
			}

			for (std::uint32_t i = 0; i < texture.mipmaps(); ++i) {
				write_tga(fmt::format("mip{}_{}", i, output), texture.as_rgba8(i), texture.mipmap_width(i), texture.mipmap_height(i));
			}
		} else {
			if (level > texture.mipmaps() - 1) {
				fmt::print(stderr, "mipmap {} not available. mipmaps 0 to {} are available", level, texture.mipmaps() - 1);
				return EXIT_FAILURE;
			}

			write_tga(output, texture.as_rgba8(level), texture.mipmap_width(level), texture.mipmap_height(level));
		}
	} catch (const std::exception& e) {
		fmt::print(stderr, "cannot convert texture: {}", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}
