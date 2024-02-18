// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Error.hh"
#include "zenkit/Library.hh"
#include "zenkit/Stream.hh"

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>

namespace zenkit::daedalus {
	struct CompilationSource;

	struct SourceLocation {
		std::shared_ptr<CompilationSource> source;
		std::size_t line {0}, column {0};

		[[nodiscard]] std::string format() const;
	};

	class CompilerError : public Error {
	public:
		CompilerError(std::string type, SourceLocation loc, std::string message);

		[[nodiscard]] std::string format() const;

	public:
		SourceLocation location;
		std::string type;
	};

	class SyntaxError : public CompilerError {
	public:
		SyntaxError(SourceLocation loc, std::string message);
	};

	class TypeError : public CompilerError {
	public:
		TypeError(SourceLocation loc, std::string message);
	};

	struct CompilationSource {
		explicit CompilationSource(std::filesystem::path const& path);
		explicit CompilationSource(std::vector<std::byte> source);
		explicit CompilationSource(std::unique_ptr<Read> source);

		[[nodiscard]] std::string describe() const;

		std::filesystem::path const path;
		std::unique_ptr<Read> read;
	};

	bool operator==(CompilationSource const& a, CompilationSource const& b);

	class Compiler {
	public:
		ZKAPI void add(std::filesystem::path const& source);
		ZKAPI void add_raw(std::string_view source);
		ZKAPI void add_src(std::filesystem::path const& source);

		ZKAPI void compile();

	private:
		std::vector<std::shared_ptr<CompilationSource>> _m_sources;
	};
} // namespace zenkit::daedalus
