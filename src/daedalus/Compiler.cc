// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT

#include "zenkit/daedalus/Compiler.hh"
#include "zenkit/Misc.hh"
#include "zenkit/daedalus/SyntaxTree.hh"

#include "../Internal.hh"

#include "TypeCheck.hh"
#include "TypeStore.hh"

#include <utility>

namespace zenkit::daedalus {
	static std::vector<std::filesystem::path> find_children(std::filesystem::path const& parent,
	                                                        std::string const& child) {
		if (!std::filesystem::is_directory(parent)) {
			return {};
		}

		std::string prefix = child.substr(child.find_first_not_of(' '), child.find_last_not_of(' ') + 1), suffix;
		std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);

		auto wildcard = prefix.find('*');
		if (wildcard != std::string::npos) {
			suffix = prefix.substr(wildcard + 1);
			prefix = prefix.substr(0, wildcard);
		}

		std::vector<std::filesystem::path> paths;
		for (auto const& node : std::filesystem::directory_iterator(parent)) {
			auto name = node.path().filename().string();
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			auto suffix_loc = wildcard == std::string::npos ? prefix.size() : name.rfind(suffix);

			if (name.find(prefix) == 0 && suffix_loc == (name.size() - suffix.size())) {
				paths.push_back(node);
			}
		}

		return paths;
	}

	CompilerError::CompilerError(std::string type, SourceLocation location, std::string message)
	    : Error(std::move(message)), location(std::move(location)), type(std::move(type)) {}

	SyntaxError::SyntaxError(SourceLocation location, std::string message)
	    : CompilerError("SyntaxError", std::move(location), std::move(message)) {}

	TypeError::TypeError(SourceLocation location, std::string message)
	    : CompilerError("TypeError", std::move(location), std::move(message)) {}

	std::string CompilerError::format() const {
		std::string repr = this->type + ": " + this->message + "\n";

		auto filename = this->location.source->describe();
		repr += " --> " + filename + ":" + std::to_string(this->location.line) + ":" +
		    std::to_string(this->location.column);
		repr += "\n";

		return repr + "\n" + this->location.format();
	}

	std::string SourceLocation::format() const {
		auto filename = this->source->describe();

		auto line_start = this->line - 5, line_end = this->line;
		long line_chars = (long) fmax(::floor(::log10((double) line_end)) + 1, 3);
		long line_length = 0;

		if (line_start > line_end) {
			line_start = 0;
		}

		std::string last_line;
		std::string code_lines;

		auto off = this->source->read->tell();
		auto* r = this->source->read.get();
		r->seek(0, Whence::BEG);

		for (auto i = 0u; i < line_end; ++i) {
			last_line = r->read_line(false);
			if (i < line_start) continue;

			line_length = (long) ::fmax((double) line_length, (double) last_line.length());

			auto lineno_string = std::to_string(i + 1);
			code_lines += std::string(line_chars - lineno_string.length(), ' ');
			code_lines += lineno_string;
			code_lines += " | ";
			code_lines += last_line;
			code_lines += "\n";
		}

		r->seek(static_cast<ssize_t>(off), Whence::BEG);

		code_lines += std::string(line_chars, ' ') + " | ";
		long col = (long) fmin((double) this->column - 2, (double) last_line.size());
		for (auto i = 0; i < col; ++i) {
			code_lines += std::isspace(last_line[i]) ? last_line[i] : ' ';
		}
		code_lines += "^~ Here.";

		long separator_length;
		if ((long) filename.length() > line_length) {
			separator_length = 10;
		} else {
			separator_length = line_length - (long) filename.length();
		}

		std::string repr = std::string(line_chars - 3, ' ');
		repr += "----|--(";
		repr += filename;
		repr += ")";
		repr += std::string((long) ::fmax(10., (double) separator_length), '-');
		return repr + "\n" + code_lines;
	}

	CompilationSource::CompilationSource(std::filesystem::path const& path) : path(path), read(Read::from(path)) {}

	CompilationSource::CompilationSource(std::vector<std::byte> source)
	    : path(""), read(Read::from(std::move(source))) {}

	CompilationSource::CompilationSource(std::unique_ptr<Read> source) : path(""), read(std::move(source)) {}

	std::string CompilationSource::describe() const {
		return this->path.empty() ? "<memory>" : this->path.filename().string();
	}

	bool operator==(CompilationSource const& a, CompilationSource const& b) {
		return !a.path.empty() && a.path == b.path;
	}

	void Compiler::add(std::filesystem::path const& source) {
		for (auto& src : _m_sources) {
			if (src->path == source) {
				return;
			}
		}

		_m_sources.push_back(std::make_shared<CompilationSource>(source));
	}

	void Compiler::add_raw(std::string_view source) {
		_m_sources.push_back(std::make_shared<CompilationSource>(
		    std::vector<std::byte>(reinterpret_cast<std::byte const*>(source.data()),
		                           reinterpret_cast<std::byte const*>(source.data() + source.size()))));
	}

	void Compiler::add_src(std::filesystem::path const& source) {
		auto dir = source.parent_path();
		auto rd = Read::from(source);

		while (!rd->eof()) {
			std::string line = rd->read_line(true);
			std::replace(line.begin(), line.end(), '\\', '/');

			std::filesystem::path path = line;
			path.make_preferred();

			bool found = true;
			std::filesystem::path root = dir;
			std::vector<std::filesystem::path> files;

			for (auto& node : path) {
				auto res = find_children(root, node.string());

				// No matching child was found.
				if (res.empty()) {
					found = false;
					break;
				}

				if (res.size() == 1 && std::filesystem::is_directory(res[0])) {
					root = res[0];
					continue;
				}

				files.insert(files.end(), res.begin(), res.end());
			}

			if (found) {
				for (auto& file : files) {
					this->add(file);
				}
			} else {
				ZKLOGW("Daedalus.Compiler", ".src reference \"%s\" not found", line.c_str());
			}
		}
	}

	void Compiler::compile() {
		ZKLOGI("Daedalus.Compiler", "Compiling ...");

		try {
			AstScript root {};
			for (auto& source : _m_sources) {
				ZKLOGD("Daedalus.Compiler", "Parsing module %s ...", source->describe().c_str());
				parse_script(&root, source);
			}

			ZKLOGD("Daedalus.Compiler", "Checking types ...");
			TypeStore types;
			types.add_script(std::move(root));
			check_types(&types);

		} catch (CompilerError& err) {
			ZKLOGE("Daedalus.Compiler", "CompilerError:\n\n%s\n", err.format().c_str());
			throw err;
		}
	}
} // namespace zenkit::daedalus
