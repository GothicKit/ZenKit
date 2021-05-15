// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/compat.hh>
#include <phoenix/mesh.hh>
#include <phoenix/vdfs.hh>

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
			<< "\tusemtl " << msh.material.name() << "\n";

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

	std::ofstream mtl {name + ".mtl"};

	for (const auto& mat : mesh.materials()) {
		mtl << "newmtl " << mat.name() << "\n";
		mtl << "Kd 1.00 1.00 1.00"
			<< "\n";
		mtl << "map_Kd " << mat.texture() << "\n";
	}

	mtl.close();
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
		phoenix::reader in {};

		if (input_vdf) {
			auto vdf = phoenix::vdf_file::open(vdf_in);

			if (auto* entry = vdf.find_entry(file); entry != nullptr) {
				in = entry->open();
			} else {
				fmt::print(stderr, "the file named {} was not found in the VDF {}", file, vdf_in);
				return EXIT_FAILURE;
			}
		} else {
			in = phoenix::reader::from(file);
		}


		auto extension = file.substr(file.find('.') + 1);


		if (phoenix::iequals(extension, "MRM")) {
			auto mesh = phoenix::proto_mesh::read(in);

			std::ofstream out {output.empty() ? (file.substr(file.rfind('/') + 1, file.rfind('.'))) : output};
			dump_wavefront(out, output, mesh);
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