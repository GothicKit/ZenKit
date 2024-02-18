// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/daedalus/Module.hh"
#include "zenkit/daedalus/SyntaxTree.hh"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace zenkit::daedalus {
	class TypeStore {
	public:
		TypeStore() = default;

		void each(std::function<void(Symbol*)> const& cb);

		void add_script(AstScript ast);
		void add_class(AstClass ast);
		void add_prototype(AstPrototype ast);
		void add_instance(AstInstance ast);
		void add_function(AstFunction ast);
		void add_external(AstExternal ast);
		void add_constant(AstConstant ast);
		void add_variable(AstVariable ast);

		void add(std::unique_ptr<Symbol> sym);

		Symbol* get(std::string const& name);
		Symbol* get(AstIdentifier const& name);
		Symbol* get(AstQualifiedIdentifier const& name);

		Type type(AstIdentifier const& name, bool array);

	private:
		std::unordered_map<std::string, std::unique_ptr<Symbol>> _m_symbols;
	};
} // namespace zenkit::daedalus
