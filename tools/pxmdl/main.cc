// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/detail/compat.hh>
#include <phoenix/model.hh>
#include <phoenix/morph_mesh.hh>
#include <phoenix/proto_mesh.hh>
#include <phoenix/vdfs.hh>
#include <phoenix/world.hh>

#include <argh.h>
#include <fmt/format.h>

#include <fstream>
#include <iostream>

#include "config.hh"

static constexpr const auto HELP_MESSAGE = "Usage: pxmdl [--version]\n"
                                           "       pxmdl [-h|--help]\n"
                                           "       pxmdl <FILE> [-o|--output <OUT>] [-v|--vdf <VDF>]"
                                           "       "
                                           "\n"
                                           "phoenix pxmdl v{}\n"
                                           "Convert ZenGin models to Wavefront (.obj) models.\n";

static void dump_material(const std::string& name, const std::vector<phoenix::material>& materials) {
	std::ofstream mtl {name + ".mtl"};

	for (const auto& mat : materials) {
		if (!mat.texture.empty()) {
			mtl << "newmtl " << mat.name << "\n";
			mtl << "Kd 1.00 1.00 1.00\n";
			mtl << "map_Kd " << mat.texture << "\n";
		}
	}

	mtl.close();
}

static void dump_wavefront(std::ostream& out, const std::string& name, const phoenix::proto_mesh& mesh) {
	out << "# pxmdl exported mesh\n"
	    << "mtllib " << name << ".mtl\n\n"
	    << "# vertices\n";

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

	dump_material(name, mesh.materials());
}

static void dump_wavefront(std::ostream& out, const std::string& name, const phoenix::mesh& mesh) {
	out << "# pxmdl exported mesh\n"
	    << "mtllib " << name << ".mtl\n\n"
	    << "o " << name << "\n"
	    << "# vertices\n";

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

	dump_material(name, mats);
}

int main(int argc, const char** argv) {
	argh::parser args {argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION};

	if (args["--version"]) {
		fmt::print("pxmdl v{}\n", PXMDL_VERSION);
		return EXIT_SUCCESS;
	}

	if (args[{"-h", "--help"}]) {
		fmt::print(HELP_MESSAGE, PXMDL_VERSION);
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

		output = output.empty() ? (file.substr(file.rfind('/') + 1, file.rfind('.') - file.rfind('/') - 1) + ".obj")
		                        : output;

		auto extension = file.substr(file.find('.') + 1);
		std::ofstream out {output};

		if (phoenix::iequals(extension, "MRM")) {
			auto mesh = phoenix::proto_mesh::parse(in);

			dump_wavefront(out, output, mesh);
			out.close();

		} else if (phoenix::iequals(extension, "ZEN")) {
			auto wld =
			    phoenix::world::parse(in, phoenix::game_version::gothic_2); // FIXME: implement algorithm of detecting
			                                                                // the actual version of Gothic!

			dump_wavefront(out, output, wld.world_mesh);
			out.close();
		} else if (phoenix::iequals(extension, "MSH")) {
			auto msh = phoenix::mesh::parse(in, {});
			dump_wavefront(out, output, msh);
			out.close();
		} else if (phoenix::iequals(extension, "MMB")) {
			auto msh = phoenix::morph_mesh::parse(in);
			dump_wavefront(out, output, msh.mesh());
			out.close();
		} else if (phoenix::iequals(extension, "MDL")) {
			auto msh = phoenix::model::parse(in);
			dump_wavefront(out, output, msh.mesh().meshes()[0].mesh()); // FIXME: support dumping multiple meshes
			out.close();
		} else if (phoenix::iequals(extension, "MDM")) {
			auto msh = phoenix::model_mesh::parse(in);
			dump_wavefront(out, output, msh.meshes()[0].mesh()); // FIXME: support dumping multiple meshes
			out.close();
		} else {
			fmt::print(stderr, "format not supported: {}", extension);
			return EXIT_FAILURE;
		}
	} catch (const std::exception& e) {
		fmt::print(stderr, "cannot convert model: {}", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}
