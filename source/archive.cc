// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/archive.hh>

#include "archive/archive_ascii.hh"
#include "archive/archive_binary.hh"
#include "archive/archive_binsafe.hh"

#include <iostream>

namespace phoenix {
	constexpr std::string_view type_names[] = {
	    "unknown", // ?            = 0x00
	    "string",  // bs_string    = 0x01,
	    "int",     // bs_int       = 0x02,
	    "float",   // bs_float     = 0x03,
	    "byte",    // bs_byte      = 0x04,
	    "word",    // bs_word      = 0x05,
	    "bool",    // bs_bool      = 0x06,
	    "vec3",    // bs_vec3      = 0x07,
	    "color",   // bs_color     = 0x08,
	    "raw",     // bs_raw       = 0x09,
	    "unknown", // ?            = 0x0A
	    "unknown", // ?            = 0x0B
	    "unknown", // ?            = 0x0C
	    "unknown", // ?            = 0x0D
	    "unknown", // ?            = 0x0E
	    "unknown", // ?            = 0x0F
	    "unknown", // bs_raw_float = 0x10,
	    "enum",    // bs_enum      = 0x11,
	    "hash",    // bs_hash      = 0x12,
	};

	archive_header archive_header::parse(buffer& in) {
		try {
			archive_header header {};

			if (in.get_line() != "ZenGin Archive") {
				throw parser_error {"archive_header", "magic missing"};
			}

			std::string version = in.get_line();
			if (version.find("ver ") != 0) {
				throw parser_error {"archive_header", "ver field missing"};
			}
			header.version = std::stoi(version.substr(version.find(' ') + 1));

			header.archiver = in.get_line();

			auto format = in.get_line();
			if (format == "ASCII") {
				header.format = archive_format::ascii;
			} else if (format == "BINARY") {
				header.format = archive_format::binary;
			} else if (format == "BIN_SAFE") {
				header.format = archive_format::binsafe;
			}

			std::string save_game = in.get_line();
			if (save_game.find("saveGame ") != 0) {
				throw parser_error {"archive_header", "saveGame field missing"};
			}
			header.save = std::stoi(save_game.substr(save_game.find(' ') + 1)) != 0;

			std::string optional = in.get_line();
			if (optional.find("date ") == 0) {
				header.date = optional.substr(optional.find(' ') + 1);
				optional = in.get_line();
			}

			if (optional.find("user ") == 0) {
				header.user = optional.substr(optional.find(' ') + 1);
				optional = in.get_line();
			}

			if (optional != "END") {
				throw parser_error {"archive_header", "first END missing"};
			}

			return header;
		} catch (const buffer_error& exc) {
			throw parser_error {"archive_header", exc, "eof reached"};
		} catch (std::invalid_argument const& e) {
			throw parser_error {"archive_reader_binary", e, "reading int"};
		}
	}

	std::unique_ptr<archive_reader> archive_reader::open(buffer& in) {
		auto header = archive_header::parse(in);
		std::unique_ptr<archive_reader> reader;

		if (header.format == archive_format::ascii) {
			reader = std::make_unique<archive_reader_ascii>(in, std::move(header));
		} else if (header.format == archive_format::binary) {
			reader = std::make_unique<archive_reader_binary>(in, std::move(header));
		} else if (header.format == archive_format::binsafe) {
			reader = std::make_unique<archive_reader_binsafe>(in, std::move(header));
		} else {
			throw parser_error {"archiver_reader",
			                    "format '" + std::to_string(static_cast<uint32_t>(header.format)) +
			                        "' is not supported"};
		}

		reader->read_header();
		return reader;
	}

	void archive_reader::skip_object(bool skip_current) {
		archive_object tmp;
		int32_t level = skip_current ? 1 : 0;

		do {
			if (read_object_begin(tmp)) {
				++level;
			} else if (read_object_end()) {
				--level;
			} else {
				skip_entry();
			}
		} while (level > 0);
	}

	void archive_reader::print_structure(bool open_object) {
		this->unstable__visit_objects(
		    open_object,
		    [](const std::optional<archive_object>& obj, const std::optional<archive_entry>& ent) {
			    if (obj) {
				    std::cout << "<object class=\"" << obj->class_name << "\" name=\"" << obj->object_name
				              << "\" version=\"" << obj->version << "\" index=\"" << obj->index << "\">\n";
			    } else if (ent) {
				    std::cout << "<entry name=\"" << ent->name << "\" type=\""
				              << type_names[static_cast<uint8_t>(ent->type)] << "\" ";

				    switch (ent->type) {
				    case archive_entry_type::string:
					    std::cout << "value=\"" << std::get<std::string>(ent->value) << "\" ";
					    break;
				    case archive_entry_type::int_:
					    std::cout << "value=\"" << std::get<int>(ent->value) << "\" ";
					    break;
				    case archive_entry_type::float_:
					    std::cout << "value=\"" << std::get<float>(ent->value) << "\" ";
					    break;
				    case archive_entry_type::byte:
					    std::cout << "value=\"" << static_cast<uint16_t>(std::get<uint8_t>(ent->value)) << "\" ";
					    break;
				    case archive_entry_type::word:
					    std::cout << "value=\"" << std::get<uint16_t>(ent->value) << "\" ";
					    break;
				    case archive_entry_type::bool_:
					    std::cout << "value=\"" << std::get<bool>(ent->value) << "\" ";
					    break;
				    case archive_entry_type::vec3: {
					    auto v = std::get<glm::vec3>(ent->value);
					    std::cout << "value=\"(" << v.x << ", " << v.y << ", " << v.z << ")\" ";
					    break;
				    }
				    case archive_entry_type::color: {
					    auto v = std::get<glm::u8vec4>(ent->value);
					    std::cout << "value=\"(" << v.r << ", " << v.g << ", " << v.b << ", " << v.a << ")\" ";
					    break;
				    }
				    case archive_entry_type::raw:
				    case archive_entry_type::raw_float:
					    std::cout << "length=\"" << std::get<buffer>(ent->value).remaining() << "\" ";
					    break;
				    case archive_entry_type::enum_:
					    std::cout << "value=\"" << std::get<uint32_t>(ent->value) << "\" ";
					    break;
				    case archive_entry_type::hash:
					    std::cout << "value=\"" << std::get<uint32_t>(ent->value) << "\" ";
					    break;
				    }

				    std::cout << "/>\n";
			    } else {
				    std::cout << "</object>\n";
			    }
		    });
	}

	void archive_reader::unstable__visit_objects(bool open_object, const archive_visitor& cb) {
		std::variant<archive_object, archive_object_end, archive_entry> ent;
		int32_t level = open_object ? 1 : 0;

		do {
			ent = unstable__next();

			if (std::holds_alternative<archive_object>(ent)) {
				cb(std::get<archive_object>(ent), std::nullopt);
				++level;
			} else if (std::holds_alternative<archive_object_end>(ent)) {
				cb(std::nullopt, std::nullopt);
				--level;
			} else {
				cb(std::nullopt, std::get<archive_entry>(ent));
			}
		} while (level > 0);
	}
} // namespace phoenix
