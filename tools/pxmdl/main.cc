// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/compat.hh>
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
		if (!mat.texture().empty()) {
			mtl << "newmtl " << mat.name() << "\n";
			mtl << "Kd 1.00 1.00 1.00\n";
			mtl << "map_Kd " << mat.texture() << "\n";
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

	// Not sure what these normals are for
	for (const auto& item : mesh.normals()) {
		out << "vn " << item.x << " " << item.y << " " << item.z << "\n";
	}

	int i = 0;
	for (const auto& msh : mesh.submeshes()) {
		out << "g sub" << ++i << "\n"
			<< "\tusemtl " << msh.mat.name() << "\n";

		for (const auto& item : msh.wedges) {
			out << "\tvn " << item.normal.x << " " << item.normal.y << " " << item.normal.z << "\n";
		}
		for (const auto& item : msh.wedges) {
			out << "\tvt " << item.texture.x << " " << item.texture.y << "\n";
		}

		for (const auto& item : msh.triangles) {

			auto wedge0 = msh.wedges[item.wedges[0]];
			auto wedge1 = msh.wedges[item.wedges[1]];
			auto wedge2 = msh.wedges[item.wedges[2]];

			out << "\tf "
				<< wedge0.index + 1 << "/" << item.wedges[0] + 1 << "/" << item.wedges[0] + 1 << " "
				<< wedge1.index + 1 << "/" << item.wedges[1] + 1 << "/" << item.wedges[1] + 1 << " "
				<< wedge2.index + 1 << "/" << item.wedges[2] + 1 << "/" << item.wedges[2] + 1 << "\n";
		}
	}

	dump_material(name, mesh.materials());
}

static void dump_wavefront(std::ostream& out, const std::string& name, const phoenix::mesh& mesh) {
	out << "# pxmdl exported mesh\n"
		<< "mtllib " << name << ".mtl\n\n"
		<< "# vertices\n";

	for (const auto& item : mesh.vertices()) {
		out << "v " << item.x << " " << item.y << " " << item.z << "\n";
	}

	auto& mats = mesh.materials();
	auto& feats = mesh.features();

	out << "\n# normals\n";
	for (const auto& feat : feats) {
		out << "vn " << feat.normal[0] << " " << feat.normal[1] << " " << feat.normal[2] << "\n";
	}

	out << "\n# textures\n";
	for (const auto& feat : feats) {
		out << "vt " << feat.texture[0] << " " << feat.texture[1] << "\n";
	}

	int i = 0;
	for (const auto& poly : mesh.polygons()) {
		if (poly.vertex_count != 0) {
			auto& mat = mats[poly.material_index];
			out << "usemtl " << mat.name() << "\n";

			if (poly.vertex_count == 3) {
				out << "f " << poly.indices[0].vertex + 1 << "/" << poly.indices[0].feature + 1 << "/" << poly.indices[0].feature + 1 << " "
					<< poly.indices[1].vertex + 1 << "/" << poly.indices[1].feature + 1 << "/" << poly.indices[1].feature + 1 << " "
					<< poly.indices[2].vertex + 1 << "/" << poly.indices[2].feature + 1 << "/" << poly.indices[2].feature + 1 << "\n";
			} else {
				// Calculate triangle fans
				for (int j = 1; j < poly.vertex_count - 1; ++j) {
					out << "f " << poly.indices[0].vertex + 1 << "/" << poly.indices[0].feature + 1 << "/" << poly.indices[0].feature + 1 << " "
						<< poly.indices[i].vertex + 1 << "/" << poly.indices[i].feature + 1 << "/" << poly.indices[i].feature + 1 << " "
						<< poly.indices[i + 1].vertex + 1 << "/" << poly.indices[i + 1].feature + 1 << "/" << poly.indices[i + 1].feature + 1 << "\n";
				}
			}
		}
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

		output = output.empty() ? (file.substr(file.rfind('/') + 1, file.rfind('.') - file.rfind('/') - 1) + ".obj") : output;

		auto extension = file.substr(file.find('.') + 1);
		std::ofstream out {output};

		if (phoenix::iequals(extension, "MRM")) {
			auto mesh = phoenix::proto_mesh::parse(in);

			dump_wavefront(out, output, mesh);
			out.close();

		} else if (phoenix::iequals(extension, "ZEN")) {
			auto wld = phoenix::world::parse(in, phoenix::game_version::gothic_2);// FIXME: implement algorithm of detecting the actual version of Gothic!

			dump_wavefront(out, output, wld.world_mesh());
			out.close();
		} else if (phoenix::iequals(extension, "MSH")) {
			auto msh = phoenix::mesh::parse(in);
			dump_wavefront(out, output, msh);
			out.close();
		} else if (phoenix::iequals(extension, "MMB")) {
			auto msh = phoenix::morph_mesh::parse(in);
			dump_wavefront(out, output, msh.mesh());
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
