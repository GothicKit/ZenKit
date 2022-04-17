// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/model_hierarchy.hh>
#include <phoenix/texture.hh>
#include <phoenix/vdfs.hh>

#include <argh.h>
#include <fmt/format.h>
#include <glm/gtx/matrix_decompose.hpp>

#include "config.hh"

static constexpr const auto HELP_MESSAGE = "Usage: pxinfo [--version]\n"
                                           "       pxinfo [-h|--help]\n"
                                           "       pxinfo <FILE>\n"
                                           "       pxinfo <FILE> [-v|--vdf <VDF>]\n"
                                           "\n"
                                           "phoenix pxinfo v{}\n"
                                           "Display information about files found in Gothic installations.\n";

void print_hierarchy(const phoenix::model_hierachy& hierachy);
int main(int argc, const char** argv) {
	argh::parser args {argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION};

	if (args["--version"]) {
		fmt::print("pxinfo v{}\n", PXINFO_VERSION);
		return EXIT_SUCCESS;
	}

	if (args[{"-h", "--help"}]) {
		fmt::print(HELP_MESSAGE, PXINFO_VERSION);
		return EXIT_SUCCESS;
	}

	std::string file;
	if (!(args(1) >> file)) {
		fmt::print(stderr, "please provide an input file.");
		return EXIT_FAILURE;
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

		if (file.ends_with(".MDH")) {
			print_hierarchy(phoenix::model_hierachy::parse(in));
		} else {
			fmt::print(stderr, "format not supported: {}", file.substr(file.rfind('.') + 1));
			return EXIT_FAILURE;
		}
	} catch (const std::exception& e) {
		fmt::print(stderr, "cannot parse file: {}", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void print_hierarchy(const phoenix::model_hierachy& hierachy) {
	fmt::print("Type: Model Hierarchy File\n");
	fmt::print("Root Translation: vec3(x={}, y={}, z={})\n",
	           hierachy.root_translation().x,
	           hierachy.root_translation().y,
	           hierachy.root_translation().z);

	auto [bbox_min, bbox_max] = hierachy.bbox();
	fmt::print("Bounding Box: min=vec3(x={}, y={}, z={}), max=vec3(x={}, y={}, z={})\n",
	           bbox_min.x,
	           bbox_min.y,
	           bbox_min.z,
	           bbox_max.x,
	           bbox_max.y,
	           bbox_max.z);

	auto [coll_min, coll_max] = hierachy.collision_bbox();
	fmt::print("Collision Box: min=vec3(x={}, y={}, z={}), max=vec3(x={}, y={}, z={})\n",
	           coll_min.x,
	           coll_min.y,
	           coll_min.z,
	           coll_max.x,
	           coll_max.y,
	           coll_max.z);

	auto& nodes = hierachy.nodes();
	fmt::print("Node Count: {}\n", nodes.size());
	fmt::print("Nodes:\n");

	auto& root = nodes[0];
	fmt::print("  Name: {}\n", root.name);

	std::function<void(unsigned short, unsigned)> print_tree = [&nodes, &print_tree](unsigned short parent,
	                                                                                 unsigned indent) {
		for (unsigned i = 0; i < nodes.size(); i++) {
			auto& node = nodes[i];
			if (node.parent_index == parent) {
				fmt::print("{:>{}}Name: {}\n", "", indent, node.name);
				glm::vec3 scale;
				glm::quat rotation;
				glm::vec3 translation;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(node.transform, scale, rotation, translation, skew, perspective);

				fmt::print("{:>{}}Translation: vec3(x={}, y={}, z={})\n",
				           "",
				           indent,
				           translation.x,
				           translation.y,
				           translation.z);

				fmt::print("{:>{}}Rotation: vec3(x={}, y={}, z={})\n", "", indent, rotation.x, rotation.y, rotation.z);
				fmt::print("{:>{}}Scale: vec3(x={}, y={}, z={})\n", "", indent, scale.x, scale.y, scale.z);
				print_tree(i, indent + 2);
			}
		}
	};

	print_tree(0, 4);
}
