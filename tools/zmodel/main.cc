// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/detail/compat.hh>
#include <phoenix/model.hh>
#include <phoenix/morph_mesh.hh>
#include <phoenix/proto_mesh.hh>
#include <phoenix/vdfs.hh>
#include <phoenix/world.hh>

#include <flags.h>
#include <fmt/format.h>

#include <fstream>
#include <iostream>

#include "config.hh"

namespace px = phoenix;

static constexpr const auto HELP_MESSAGE =
    R"(USAGE
    zmodel -v
    zmodel -h
    zmodel -f FILE [-e VDF] [-o PATH] [-g1] [-g2] [-m PATH]

DESCRIPTION
    Converts ZenGin models to Wavefront (.obj) models

OPTIONS
    -v, --version              Print the version of zmodel
    -h, --help                 Print this help message
    -f FILE, --input FILE      The file to convert.
    -o PATH, --output PATH     Write data to the given path instead of stdout.
    -e VDF, --vdf VDF          Instead of reading FILE directly from disk, extract it from VDF instead.
    -m PATH, --material PATH   Also write a material file to PATH
    -g1                        Parse a Gothic 1 world file
    -g2                        Parse a Gothic 2 world file (default)

VERSION
    phoenix zdump v{}
)";

static void dump_material(std::ostream& mtl, std::string_view name, const std::vector<phoenix::material>& materials) {
	for (const auto& mat : materials) {
		if (!mat.texture.empty()) {
			mtl << "newmtl " << mat.name << "\n";
			mtl << "Kd 1.00 1.00 1.00\n";
			mtl << "map_Kd " << mat.texture << "\n";
		}
	}
}

static void dump_wavefront(std::ostream& out,
                           std::ostream* material_out,
                           std::string_view mtllib_name,
                           const phoenix::proto_mesh& mesh) {
	out << "# zmodel exported mesh\n";
	if (material_out != nullptr)
		out << "mtllib " << mtllib_name << ".mtl\n\n";

	out << "# vertices\n";

	for (const auto& item : mesh.positions()) {
		out << "v " << item.x << " " << item.y << " " << item.z << "\n";
	}

	unsigned wedge_offset = 0;
	int i = 0;
	for (const auto& msh : mesh.submeshes()) {
		out << "g sub" << ++i << "\n"
		    << "usemtl " << msh.mat.name << "\n";

		for (const auto& item : msh.wedges) {
			out << "vn " << item.normal.x << " " << item.normal.y << " " << item.normal.z << "\n";
			out << "vt " << item.texture.x << " " << item.texture.y << "\n";
		}

		for (const auto& item : msh.triangles) {
			auto wedge0 = msh.wedges[wedge_offset + item.wedges[0]];
			auto wedge1 = msh.wedges[wedge_offset + item.wedges[1]];
			auto wedge2 = msh.wedges[wedge_offset + item.wedges[2]];

			out << "f " << wedge0.index + 1 << "/" << wedge_offset + item.wedges[0] + 1 << "/"
			    << wedge_offset + item.wedges[0] + 1 << " " << wedge1.index + 1 << "/"
			    << wedge_offset + item.wedges[1] + 1 << "/" << wedge_offset + item.wedges[1] + 1 << " "
			    << wedge2.index + 1 << "/" << wedge_offset + item.wedges[2] + 1 << "/"
			    << wedge_offset + item.wedges[2] + 1 << "\n";
		}

		wedge_offset += msh.wedges.size();
	}

	if (material_out != nullptr) {
		dump_material(*material_out, mtllib_name, mesh.materials());
	}
}

static void
dump_wavefront(std::ostream& out, std::ostream* material_out, std::string_view mtllib_name, const phoenix::mesh& mesh) {
	out << "# zmodel exported mesh\n";
	if (material_out != nullptr)
		out << "mtllib " << mtllib_name << ".mtl\n\n";

	out << "# vertices\n";

	for (const auto& item : mesh.vertices()) {
		out << "v " << item.x << " " << item.y << " " << item.z << "\n";
	}

	auto& mats = mesh.materials();
	auto& feats = mesh.features();

	out << "\n# normals\n";
	for (const auto& feat : feats) {
		out << "vn " << feat.normal.x << " " << feat.normal.y << " " << feat.normal.z << "\n";
	}

	out << "\n# textures\n";
	for (const auto& feat : feats) {
		out << "vt " << feat.texture.x << " " << feat.texture.y << "\n";
	}

	long old_material = -1;
	auto& polys = mesh.polygons();
	for (unsigned i = 0; i < polys.vertex_indices.size() / 3; ++i) {
		auto material = polys.material_indices[i];

		if (old_material != material) {
			auto& mat = mats[material];
			out << "usemtl " << mat.name << "\n";
			out << "g " << mat.name << "\n";
			old_material = material;
		}

		out << "f ";
		for (unsigned v = 0; v < 3; ++v) {
			auto feature = polys.feature_indices[i * 3 + v] + 1;
			out << polys.vertex_indices[i * 3 + v] + 1 << "/" << feature << "/" << feature;

			if (v != 2) {
				out << " ";
			}
		}

		out << "\n";
	}

	if (material_out != nullptr) {
		dump_material(*material_out, mtllib_name, mesh.materials());
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
				return px::buffer::empty();
			}
		} else {
			return px::buffer::mmap(*input);
		}
	} else {
		std::vector<std::byte> data {};

		while (!std::cin.eof()) {
			data.push_back(static_cast<std::byte>(std::cin.get()));
		}

		if (data.empty()) {
			fmt::print(stderr, "no data provided via stdin");
			return px::buffer::empty();
		}

		// remove the EOF byte
		data.pop_back();
		return px::buffer::of(std::move(data));
	}
}

int main(int argc, char** argv) {
	const flags::args args {argc, argv};

	if (args.get<bool>("v") || args.get<bool>("version")) {
		fmt::print("zmodel v{}\n", ZMODEL_VERSION);
	} else if (args.get<bool>("h") || args.get<bool>("help")) {
		fmt::print(HELP_MESSAGE, ZMODEL_VERSION);
	} else {
		try {
			auto input = args.get<std::string>("f");
			if (!input && !(input = args.get<std::string>("file"))) {
				fmt::print(stderr, "no input file given\n");
				return EXIT_FAILURE;
			}

			auto vdf = args.get<std::string>("e");
			if (!vdf)
				vdf = args.get<std::string>("vdf");

			auto output = args.get<std::string>("o");
			if (!output)
				output = args.get<std::string>("output");

			auto material = args.get<std::string>("m");
			if (!material)
				material = args.get<std::string>("material");

			auto g1 = args.get<bool>("g1").value_or(!args.get<bool>("g2").value_or(true));
			auto in = open_buffer(input, vdf);
			auto extension = input->substr(input->find('.') + 1);

			std::ostream* model_out = &std::cout;
			if (output)
				model_out = new std::ofstream {*output};

			std::ostream* material_out = nullptr;
			if (material)
				material_out = new std::ofstream {*output};

			if (phoenix::iequals(extension, "MRM")) {
				auto mesh = phoenix::proto_mesh::parse(in);
				dump_wavefront(*model_out, material_out, material.value_or(""), mesh);
			} else if (phoenix::iequals(extension, "ZEN")) {
				auto wld =
				    phoenix::world::parse(in, g1 ? phoenix::game_version::gothic_1 : phoenix::game_version::gothic_2);

				dump_wavefront(*model_out, material_out, material.value_or(""), wld.world_mesh);
			} else if (phoenix::iequals(extension, "MSH")) {
				auto msh = phoenix::mesh::parse(in, {});
				dump_wavefront(*model_out, material_out, material.value_or(""), msh);
			} else if (phoenix::iequals(extension, "MMB")) {
				auto msh = phoenix::morph_mesh::parse(in);
				dump_wavefront(*model_out, material_out, material.value_or(""), msh.mesh());
			} else if (phoenix::iequals(extension, "MDL")) {
				auto msh = phoenix::model::parse(in);
				dump_wavefront(*model_out,
				               material_out,
				               material.value_or(""),
				               msh.mesh().meshes()[0].mesh()); // FIXME: support dumping multiple meshes
			} else if (phoenix::iequals(extension, "MDM")) {
				auto msh = phoenix::model_mesh::parse(in);
				dump_wavefront(*model_out,
				               material_out,
				               material.value_or(""),
				               msh.meshes()[0].mesh()); // FIXME: support dumping multiple meshes
			} else {
				fmt::print(stderr, "format not supported: {}", extension);
				return EXIT_FAILURE;
			}
		} catch (const std::exception& e) {
			fmt::print(stderr, "cannot convert model: {}", e.what());
			return EXIT_FAILURE;
		}
	}

	return 0;
}
