// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/animation.hh>
#include <phoenix/font.hh>
#include <phoenix/mesh.hh>
#include <phoenix/messages.hh>
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

static constexpr const char* TEXTURE_FORMAT_NAMES[] = {
    "B8G8R8A8",
    "R8G8B8A8",
    "A8B8G8R8",
    "A8R8G8B8",
    "B8G8R8",
    "R8G8B8",
    "A4R4G4B4",
    "A1R5G5B5",
    "R5G6B5",
    "PAL8",
    "DXT1",
    "DXT2",
    "DXT3",
    "DXT4",
    "DXT5",
};

void print_hierarchy(const phoenix::model_hierachy& hierachy);
void print_animation(const phoenix::animation& animation);
void print_messages(const phoenix::messages& messages);
void print_font(const phoenix::font& fnt);
void print_texture(const phoenix::texture& tex);
void print_mesh(const phoenix::mesh& msh);

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
		}
		if (file.ends_with(".MAN")) {
			print_animation(phoenix::animation::parse(in));
		} else if (file.ends_with(".BIN") || file.ends_with(".CSL") || file.ends_with(".DAT") ||
		           file.ends_with(".LSC")) {
			print_messages(phoenix::messages::parse(in));
		} else if (file.ends_with(".FNT")) {
			print_font(phoenix::font::parse(in));
		} else if (file.ends_with(".MSH")) {
			print_mesh(phoenix::mesh::parse(in, {}));
		} else if (file.ends_with(".TEX")) {
			print_texture(phoenix::texture::parse(in));
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

void print_texture(const phoenix::texture& tex) {
	fmt::print("Type: Texture\n");
	fmt::print("Format: {}\n", TEXTURE_FORMAT_NAMES[tex.format()]);
	fmt::print("Width: {}\n", tex.width());
	fmt::print("Height: {}\n", tex.height());
	fmt::print("Reference Width: {}\n", tex.ref_width());
	fmt::print("Reference Height: {}\n", tex.ref_height());
	fmt::print("Mipmap Count: {}\n", tex.mipmaps());
	fmt::print("Average Color (ARGB): #{:0>6x}\n", tex.average_color());
}

void print_mesh(const phoenix::mesh& msh) {
	fmt::print("Type: Mesh\n");
	fmt::print("Source File: {}\n", msh.name());

	auto date = msh.date();
	fmt::print("Date: {}-{}-{}T{}:{}:{}\n", date.year, date.month, date.day, date.hour, date.minute, date.second);

	auto bbox = msh.bbox();
	fmt::print("Bounding Box: min=vec3(x={}, y={}, z={}), max=vec3(x={}, y={}, z={})\n",
	           bbox.min.x,
	           bbox.min.y,
	           bbox.min.z,
	           bbox.max.x,
	           bbox.max.y,
	           bbox.max.z);

	fmt::print("Vertex Count: {}\n", msh.vertices().size());
	fmt::print("Polygon Count: {}\n", msh.polygons().feature_indices.size());
	fmt::print("Feature Count: {}\n", msh.features().size());
	fmt::print("Lightmaps:\n");

	for (auto& lightmap : msh.lightmaps()) {
		fmt::print("  - Name: {}\n", TEXTURE_FORMAT_NAMES[lightmap.image->format()]);
		fmt::print("    Origin: vec3(x={}, y={}, z={})\n", lightmap.origin.x, lightmap.origin.y, lightmap.origin.z);
		fmt::print("    Normals: a=vec3(x={}, y={}, z={}) b=vec3(x={}, y={}, z={})\n",
		           lightmap.normals[0].x,
		           lightmap.normals[0].y,
		           lightmap.normals[0].z,
		           lightmap.normals[1].x,
		           lightmap.normals[1].y,
		           lightmap.normals[1].z);
	}

	fmt::print("Materials:\n");
	for (auto& material : msh.materials()) {
		fmt::print("  - Name: {}\n", material.name());
		fmt::print("    Texture: {}\n", material.texture());
	}
}

void print_font(const phoenix::font& fnt) {
	fmt::print("Type: Font\n");
	fmt::print("Name: {}\n", fnt.name());
	fmt::print("Glyph Count: {}\n", fnt.glyphs().size());
	fmt::print("Glyph Height: {}\n", fnt.height());
	fmt::print("Glyphs:\n");

	for (unsigned i = 0; i < fnt.glyphs().size(); ++i) {
		auto& glyph = fnt.glyphs()[i];
		fmt::print("  {:0>2x}: u=({}, {}), v=({}, {}), w={}\n",
		           i,
		           glyph.uv[0].x,
		           glyph.uv[0].y,
		           glyph.uv[1].x,
		           glyph.uv[1].y,
		           glyph.width);
	}
}

void print_messages(const phoenix::messages& messages) {
	fmt::print("Type: Message Database\n");
	fmt::print("Block Count: {}\n", messages.blocks().size());
	fmt::print("Blocks:\n");

	for (const auto& block : messages.blocks()) {
		fmt::print("- Name: {}\n", block.name);
		fmt::print("  Message: type={}, name={}, text={}\n",
		           block.message.type,
		           block.message.name,
		           block.message.text);
	}
}

void print_animation(const phoenix::animation& animation) {
	fmt::print("Type: Animation\n");
	fmt::print("Name: {}\n", animation.name());
	fmt::print("Next: {}\n", animation.next());
	fmt::print("Layer: {}\n", animation.layer());
	fmt::print("Frames: {}\n", animation.frames());
	fmt::print("FPS: {}\n", animation.frames_per_second());
	fmt::print("FPS alt: {}\n", animation.frames_per_second_alt());
	fmt::print("Source Path: {}\n", animation.source_path());
	fmt::print("Source Script:\n  {}\n", animation.source_script());
	fmt::print("Checksum: {}\n", animation.checksum());

	auto bbox = animation.bbox();
	fmt::print("Bounding Box: min=vec3(x={}, y={}, z={}), max=vec3(x={}, y={}, z={})\n",
	           bbox.min.x,
	           bbox.min.y,
	           bbox.min.z,
	           bbox.max.x,
	           bbox.max.y,
	           bbox.max.z);

	fmt::print("Events:");
	for (const auto& event : animation.events()) {
		fmt::print("\n- Tag: {}\n", event.tag);
		fmt::print("  Type: {}\n", (unsigned) event.type);
		fmt::print("  Number: {}\n", event.no);
		fmt::print("  Probability: {}\n", event.probability);

		fmt::print("  Content: ");
		for (auto& v : event.content)
			fmt::print("{} ", v);

		fmt::print("\n  Values:");
		for (auto v : event.values)
			fmt::print("{} ", v);
	}

	fmt::print("\nSamples: {}\n", animation.samples().size());
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
