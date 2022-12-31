// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/script.hh>

#include <string>

namespace phoenix {
	symbol_not_found::symbol_not_found(std::string&& sym_name)
	    : script_error("symbol not found: " + sym_name), name(sym_name) {}

	member_registration_error::member_registration_error(const symbol* s, std::string&& msg)
	    : script_error("cannot register member " + s->name() + ": " + msg), sym(s) {}

	invalid_registration_datatype::invalid_registration_datatype(const symbol* s, std::string&& provided)
	    : member_registration_error(s,
	                                "wrong datatype: provided '" + provided + "' expected " +
	                                    DAEDALUS_DATA_TYPE_NAMES[(std::uint32_t) s->type()]) {}

	illegal_type_access::illegal_type_access(const symbol* s, datatype expected_dt)
	    : illegal_access("illegal access of type " + std::to_string(static_cast<int32_t>(expected_dt)) + " on symbol " +
	                     s->name() + " which is another type (" + std::to_string(static_cast<int32_t>(s->type())) +
	                     ")"),
	      sym(s), expected(expected_dt) {}

	illegal_index_access::illegal_index_access(const symbol* s, std::uint8_t idx)
	    : illegal_access("illegal access of out-of-bounds index " + std::to_string(idx) + " while reading " +
	                     s->name()),
	      sym(s), index(idx) {}

	illegal_const_access::illegal_const_access(const symbol* s)
	    : illegal_access("illegal mutable access of const symbol " + s->name()), sym(s) {}

	illegal_instance_access::illegal_instance_access(const symbol* s, std::uint32_t parent)
	    : illegal_access("illegal access of member " + s->name() +
	                     " which does not have the same parent "
	                     "class as the context instance (" +
	                     std::to_string(s->parent()) + " != " + std::to_string(parent) + ")"),
	      sym(s), expected_parent(parent) {}

	unbound_member_access::unbound_member_access(const symbol* s)
	    : illegal_access("illegal access of unbound member " + s->name()), sym(s) {}

	no_context::no_context(const symbol* s)
	    : illegal_access("illegal access of member " + s->name() + " without a context set."), sym(s) {}

	illegal_context_type::illegal_context_type(const symbol* s, const std::type_info& ctx)
	    : illegal_access("cannot access member " + s->name() + " on context instance of type " + ctx.name() +
	                     " because this symbol is registered to instances of type " + s->registered_to().name()),
	      sym(s), context_type(ctx) {}

	instruction instruction::decode(buffer& in) {
		instruction s {};
		s.op = static_cast<opcode>(in.get());
		s.size = 1;

		switch (s.op) {
		case opcode::bl:
		case opcode::bz:
		case opcode::b:
			s.address = in.get_uint();
			s.size += sizeof(std::uint32_t);
			break;
		case opcode::pushi:
			s.immediate = in.get_int();
			s.size += sizeof(std::uint32_t);
			break;
		case opcode::be:
		case opcode::pushv:
		case opcode::pushvi:
		case opcode::gmovi:
			s.symbol = in.get_uint();
			s.size += sizeof(std::uint32_t);
			break;
		case opcode::pushvv:
			s.symbol = in.get_uint();
			s.index = in.get();
			s.size += sizeof(std::uint32_t) + sizeof(std::uint8_t);
			break;
		default:
			break;
		}

		return s;
	}

	script script::parse(const std::string& file) {
		auto in = buffer::mmap(file);
		return parse(in);
	}

	script script::parse(phoenix::buffer& in) {
		script scr {};

		scr._m_version = in.get();
		auto symbol_count = in.get_uint();

		scr._m_symbols.reserve(symbol_count + 1 /* account for temporary strings */);
		scr._m_symbols_by_name.reserve(symbol_count + 1);
		scr._m_symbols_by_address.reserve(symbol_count);
		in.skip(symbol_count * sizeof(std::uint32_t)); // Sort table
		// The sort table is a list of indexes into the symbol table sorted lexicographically by symbol name!

		for (std::uint32_t i = 0; i < symbol_count; ++i) {
			auto* sym = &scr._m_symbols.emplace_back(symbol::parse(in));
			scr._m_symbols_by_name[sym->name()] = i;
			sym->_m_index = i;

			if (sym->type() == datatype::prototype || sym->type() == datatype::instance ||
			    (sym->type() == datatype::function && sym->is_const() && !sym->is_member())) {
				scr._m_symbols_by_address[sym->address()] = i;
			}
		}

		std::uint32_t text_size = in.get_uint();
		scr._m_text = in.extract(text_size);
		return scr;
	}

	instruction script::instruction_at(std::uint32_t address) const {
		_m_text.position(address);
		return instruction::decode(_m_text);
	}

	const symbol* script::find_symbol_by_index(std::uint32_t index) const {
		if (index > _m_symbols.size()) {
			return nullptr;
		}
		return &_m_symbols[index];
	}

	const symbol* script::find_symbol_by_name(std::string_view name) const {
		std::string up {name};
		std::transform(up.begin(), up.end(), up.begin(), ::toupper);

		if (auto it = _m_symbols_by_name.find(up); it != _m_symbols_by_name.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	const symbol* script::find_symbol_by_address(std::uint32_t address) const {
		if (auto it = _m_symbols_by_address.find(address); it != _m_symbols_by_address.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	symbol* script::find_symbol_by_index(std::uint32_t index) {
		if (index > _m_symbols.size()) {
			return nullptr;
		}
		return &_m_symbols[index];
	}

	symbol* script::find_symbol_by_name(std::string_view name) {
		std::string up {name};
		std::transform(up.begin(), up.end(), up.begin(), ::toupper);

		if (auto it = _m_symbols_by_name.find(up); it != _m_symbols_by_name.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	symbol* script::find_symbol_by_address(std::uint32_t address) {
		if (auto it = _m_symbols_by_address.find(address); it != _m_symbols_by_address.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	void script::enumerate_instances_by_class_name(std::string_view name,
	                                               const std::function<void(symbol&)>& callback) {
		auto* cls = find_symbol_by_name(name);
		if (cls == nullptr)
			return;

		std::vector<uint32_t> prototypes {};
		for (auto& sym : _m_symbols) {
			if (sym.type() == datatype::prototype && sym.parent() == cls->index()) {
				prototypes.push_back(sym.index());
			} else if (sym.type() == datatype::instance &&
			           (std::find(prototypes.begin(), prototypes.end(), sym.parent()) != prototypes.end() ||
			            sym.parent() == cls->index())) {
				callback(sym);
			}
		}
	}

	std::vector<symbol*> script::find_parameters_for_function(const symbol* parent) {
		std::vector<symbol*> syms {};

		for (uint32_t i = 0; i < parent->count(); ++i) {
			syms.push_back(find_symbol_by_index(parent->index() + i + 1));
		}

		return syms;
	}

	std::vector<const symbol*> script::find_parameters_for_function(const symbol* parent) const {
		std::vector<const symbol*> syms {};

		for (uint32_t i = 0; i < parent->count(); ++i) {
			syms.push_back(find_symbol_by_index(parent->index() + i + 1));
		}

		return syms;
	}

	symbol symbol::parse(buffer& in) {
		symbol sym {};

		if (in.get_uint() != 0) {
			sym._m_name = in.get_line(false);

			// If the name starts with \xFF, this symbol was automatically generated by the compiler
			if (sym._m_name[0] == '\xFF') {
				sym._m_name[0] = '$';
				sym._m_generated = true;
			}
		}

		auto vary = in.get_uint();
		auto properties = in.get_uint();

		sym._m_count = (properties >> 0U) & 0xFFFU;                             // 12 bits
		sym._m_type = static_cast<datatype>((properties >> 12U) & 0xFU);        // 4 bits
		sym._m_flags = static_cast<std::uint32_t>((properties >> 16U) & 0x3FU); // 6 bits

		if (sym.is_member()) {
			sym._m_member_offset = vary;
		} else if (sym.type() == datatype::class_) {
			sym._m_class_size = vary;
		} else if (sym.type() == datatype::function) {
			sym._m_return_type = static_cast<datatype>(vary);
		}

		sym._m_file_index = in.get_uint() & 0x7FFFFU;  // 19 bits
		sym._m_line_start = in.get_uint() & 0x7FFFFU;  // 19 bits
		sym._m_line_count = in.get_uint() & 0x7FFFFU;  // 19 bits
		sym._m_char_start = in.get_uint() & 0xFFFFFFU; // 24 bits
		sym._m_char_count = in.get_uint() & 0xFFFFFFU; // 24 bits

		if (!sym.is_member()) {
			switch (sym._m_type) {
			case datatype::float_: {
				std::unique_ptr<float[]> value {new float[sym._m_count]};
				in.get((std::uint8_t*) value.get(), sym._m_count * sizeof(float));
				sym._m_value = std::move(value);
				break;
			}
			case datatype::integer: {
				std::unique_ptr<std::int32_t[]> value {new std::int32_t[sym._m_count]};
				in.get((std::uint8_t*) value.get(), sym._m_count * sizeof(std::uint32_t));
				sym._m_value = std::move(value);
				break;
			}
			case datatype::string: {
				std::unique_ptr<std::string[]> value {new std::string[sym._m_count]};
				for (std::uint32_t i = 0; i < sym._m_count; ++i) {
					value[i] = in.get_line_escaped(false);
				}
				sym._m_value = std::move(value);
				break;
			}
			case datatype::class_:
				sym._m_class_offset = in.get_int();
				break;
			case datatype::instance:
				sym._m_value = std::shared_ptr<instance> {nullptr};
				[[fallthrough]];
			case datatype::function:
			case datatype::prototype:
				sym._m_address = in.get_int();
				break;
			default:
				break;
			}
		}

		sym._m_parent = in.get_int();
		return sym;
	}

	const std::string& symbol::get_string(std::size_t index, const std::shared_ptr<instance>& context) const {
		if (type() != datatype::string) {
			throw illegal_type_access(this, datatype::string);
		}
		if (count() <= index) {
			throw illegal_index_access(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw no_context(this);
			}
			return *get_member_ptr<std::string>(index, context);
		} else {
			return std::get<std::unique_ptr<std::string[]>>(_m_value)[index];
		}
	}

	float symbol::get_float(std::size_t index, const std::shared_ptr<instance>& context) const {
		if (type() != datatype::float_) {
			throw illegal_type_access(this, datatype::float_);
		}
		if (count() <= index) {
			throw illegal_index_access(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw no_context(this);
			}
			return *get_member_ptr<float>(index, context);
		} else {
			return std::get<std::unique_ptr<float[]>>(_m_value)[index];
		}
	}

	std::int32_t symbol::get_int(std::size_t index, const std::shared_ptr<instance>& context) const {
		if (type() != datatype::integer && type() != datatype::function) {
			throw illegal_type_access(this, datatype::integer);
		}
		if (count() <= index) {
			throw illegal_index_access(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw no_context(this);
			}
			return *get_member_ptr<std::int32_t>(index, context);
		} else {
			return std::get<std::unique_ptr<std::int32_t[]>>(_m_value)[index];
		}
	}

	void symbol::set_string(std::string_view value, std::size_t index, const std::shared_ptr<instance>& context) {
		if (is_const()) {
			throw illegal_const_access(this);
		}
		if (type() != datatype::string) {
			throw illegal_type_access(this, datatype::string);
		}
		if (count() <= index) {
			throw illegal_index_access(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw no_context(this);
			}
			*get_member_ptr<std::string>(index, context) = value;
		} else {
			std::get<std::unique_ptr<std::string[]>>(_m_value).get()[index] = value;
		}
	}

	void symbol::set_float(float value, std::size_t index, const std::shared_ptr<instance>& context) {
		if (is_const()) {
			throw illegal_const_access(this);
		}
		if (type() != datatype::float_) {
			throw illegal_type_access(this, datatype::float_);
		}
		if (count() <= index) {
			throw illegal_index_access(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw no_context(this);
			}
			*get_member_ptr<float>(index, context) = value;
		} else {
			std::get<std::unique_ptr<float[]>>(_m_value)[index] = value;
		}
	}

	void symbol::set_int(std::int32_t value, std::size_t index, const std::shared_ptr<instance>& context) {
		if (is_const()) {
			throw illegal_const_access(this);
		}
		if (type() != datatype::integer && type() != datatype::function) {
			throw illegal_type_access(this, datatype::integer);
		}
		if (count() <= index) {
			throw illegal_index_access(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw no_context(this);
			}
			*get_member_ptr<std::int32_t>(index, context) = value;
		} else {
			std::get<std::unique_ptr<std::int32_t[]>>(_m_value)[index] = value;
		}
	}

	const std::shared_ptr<instance>& symbol::get_instance() {
		if (type() != datatype::instance) {
			throw illegal_type_access(this, datatype::instance);
		}

		return std::get<std::shared_ptr<instance>>(_m_value);
	}

	void symbol::set_instance(const std::shared_ptr<instance>& inst) {
		if (type() != datatype::instance) {
			throw illegal_type_access(this, datatype::instance);
		}

		std::get<std::shared_ptr<instance>>(_m_value) = inst;
	}
} // namespace phoenix
