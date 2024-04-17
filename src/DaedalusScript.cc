// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/DaedalusScript.hh"
#include "zenkit/Stream.hh"

#include "Internal.hh"

#include "phoenix/buffer.hh"

namespace zenkit {
	DaedalusSymbolNotFound::DaedalusSymbolNotFound(std::string&& sym_name)
	    : DaedalusScriptError("symbol not found: " + sym_name), name(sym_name) {}

	DaedalusMemberRegistrationError::DaedalusMemberRegistrationError(DaedalusSymbol const* s, std::string&& msg)
	    : DaedalusScriptError("cannot register member " + s->name() + ": " + msg), sym(s) {}

	DaedalusInvalidRegistrationDataType::DaedalusInvalidRegistrationDataType(DaedalusSymbol const* s,
	                                                                         std::string&& provided)
	    : DaedalusMemberRegistrationError(s,
	                                      "wrong datatype: provided '" + provided + "' expected " +
	                                          DAEDALUS_DATA_TYPE_NAMES[static_cast<std::uint32_t>(s->type())]) {}

	DaedalusIllegalTypeAccess::DaedalusIllegalTypeAccess(DaedalusSymbol const* s, DaedalusDataType expected_dt)
	    : DaedalusIllegalAccess("illegal access of type " + std::to_string(static_cast<int32_t>(expected_dt)) +
	                            " on DaedalusSymbol " + s->name() + " which is another type (" +
	                            std::to_string(static_cast<int32_t>(s->type())) + ")"),
	      sym(s), expected(expected_dt) {}

	DaedalusIllegalIndexAccess::DaedalusIllegalIndexAccess(DaedalusSymbol const* s, std::size_t idx)
	    : DaedalusIllegalAccess("illegal access of out-of-bounds index " + std::to_string(idx) + " while reading " +
	                            s->name()),
	      sym(s), index(idx) {}

	DaedalusIllegalConstAccess::DaedalusIllegalConstAccess(DaedalusSymbol const* s)
	    : DaedalusIllegalAccess("illegal mutable access of const symbol " + s->name()), sym(s) {}

	DaedalusIllegalInstanceAccess::DaedalusIllegalInstanceAccess(DaedalusSymbol const* s, std::uint32_t parent)
	    : DaedalusIllegalAccess("illegal access of member " + s->name() +
	                            " which does not have the same parent "
	                            "class as the context instance (" +
	                            std::to_string(s->parent()) + " != " + std::to_string(parent) + ")"),
	      sym(s), expected_parent(parent) {}

	DaedalusUnboundMemberAccess::DaedalusUnboundMemberAccess(DaedalusSymbol const* s)
	    : DaedalusIllegalAccess("illegal access of unbound member " + s->name()), sym(s) {}

	DaedalusNoContextError::DaedalusNoContextError(DaedalusSymbol const* s)
	    : DaedalusIllegalAccess("illegal access of member " + s->name() + " without a context set."), sym(s) {}

	DaedalusIllegalContextType::DaedalusIllegalContextType(DaedalusSymbol const* s, std::type_info const& ctx)
	    : DaedalusIllegalAccess("cannot access member " + s->name() + " on context instance of type " + ctx.name() +
	                            " because this symbol is registered to instances of type " + s->registered_to().name()),
	      sym(s), context_type(ctx) {}

	DaedalusInstruction DaedalusInstruction::decode(Read* in) {
		DaedalusInstruction s {};
		s.op = static_cast<DaedalusOpcode>(in->read_ubyte());
		s.size = 1;

		switch (s.op) {
		case DaedalusOpcode::BL:
		case DaedalusOpcode::BZ:
		case DaedalusOpcode::B:
			s.address = in->read_uint();
			s.size += sizeof(std::uint32_t);
			break;
		case DaedalusOpcode::PUSHI:
			s.immediate = in->read_int();
			s.size += sizeof(std::uint32_t);
			break;
		case DaedalusOpcode::BE:
		case DaedalusOpcode::PUSHV:
		case DaedalusOpcode::PUSHVI:
		case DaedalusOpcode::GMOVI:
			s.symbol = in->read_uint();
			s.size += sizeof(std::uint32_t);
			break;
		case DaedalusOpcode::PUSHVV:
			s.symbol = in->read_uint();
			s.index = in->read_ubyte();
			s.size += sizeof(std::uint32_t) + sizeof(std::uint8_t);
			break;
		default:
			break;
		}

		return s;
	}

	DaedalusScript DaedalusScript::parse(std::string const& file) {
		DaedalusScript scr {};

		auto r = Read::from(file);
		scr.load(r.get());

		return scr;
	}

	DaedalusScript DaedalusScript::parse(phoenix::buffer& in) {
		DaedalusScript scr {};

		auto r = Read::from(&in);
		scr.load(r.get());

		return scr;
	}

	void DaedalusScript::load(Read* r) {
		this->_m_version = r->read_ubyte();
		auto symbol_count = r->read_uint();

		this->_m_symbols.resize(symbol_count);
		this->_m_symbols_by_name.reserve(symbol_count + 1);
		this->_m_symbols_by_address.reserve(symbol_count);

		r->seek(static_cast<ssize_t>(symbol_count * sizeof(std::uint32_t)), Whence::CUR); // Sort table
		// The sort table is a list of indexes into the symbol table sorted lexicographically by symbol name!

		for (std::uint32_t i = 0; i < symbol_count; ++i) {
			auto& sym = this->_m_symbols[i];
			sym.load(r);

			this->_m_symbols_by_name[sym.name()] = i;
			sym._m_index = i;

			if (sym.type() == DaedalusDataType::PROTOTYPE || sym.type() == DaedalusDataType::INSTANCE ||
			    (sym.type() == DaedalusDataType::FUNCTION && sym.is_const() && !sym.is_member())) {
				this->_m_symbols_by_address[sym.address()] = i;
			}
		}

		std::uint32_t text_size = r->read_uint();
		std::vector code(text_size, std::byte {});
		r->read(code.data(), text_size);

		this->_m_text = Read::from(std::move(code));
	}

	DaedalusInstruction DaedalusScript::instruction_at(std::uint32_t address) const {
		_m_text->seek(address, Whence::BEG);
		return DaedalusInstruction::decode(_m_text.get());
	}

	DaedalusSymbol const* DaedalusScript::find_symbol_by_index(std::uint32_t index) const {
		if (index >= _m_symbols.size()) {
			return nullptr;
		}
		return &_m_symbols[index];
	}

	DaedalusSymbol const* DaedalusScript::find_symbol_by_name(std::string_view name) const {
		std::string up {name};
		std::transform(up.begin(), up.end(), up.begin(), toupper);

		if (auto it = _m_symbols_by_name.find(up); it != _m_symbols_by_name.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	DaedalusSymbol const* DaedalusScript::find_symbol_by_address(std::uint32_t address) const {
		if (auto it = _m_symbols_by_address.find(address); it != _m_symbols_by_address.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	DaedalusSymbol* DaedalusScript::find_symbol_by_index(std::uint32_t index) {
		if (index >= _m_symbols.size()) {
			return nullptr;
		}
		return &_m_symbols[index];
	}

	DaedalusSymbol* DaedalusScript::find_symbol_by_name(std::string_view name) {
		std::string up {name};
		std::transform(up.begin(), up.end(), up.begin(), toupper);

		if (auto it = _m_symbols_by_name.find(up); it != _m_symbols_by_name.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	DaedalusSymbol* DaedalusScript::find_symbol_by_address(std::uint32_t address) {
		if (auto it = _m_symbols_by_address.find(address); it != _m_symbols_by_address.end()) {
			return find_symbol_by_index(it->second);
		}

		return nullptr;
	}

	void DaedalusScript::enumerate_instances_by_class_name(std::string_view name,
	                                                       std::function<void(DaedalusSymbol&)> const& callback) {
		auto* cls = find_symbol_by_name(name);
		if (cls == nullptr) return;

		std::vector<uint32_t> prototypes {};
		for (auto& sym : _m_symbols) {
			if (sym.type() == DaedalusDataType::PROTOTYPE && sym.parent() == cls->index()) {
				prototypes.push_back(sym.index());
			} else if (sym.type() == DaedalusDataType::INSTANCE && sym.is_const() &&
			           (std::find(prototypes.begin(), prototypes.end(), sym.parent()) != prototypes.end() ||
			            sym.parent() == cls->index())) {
				callback(sym);
			}
		}
	}

	std::vector<DaedalusSymbol*> DaedalusScript::find_parameters_for_function(DaedalusSymbol const* parent) {
		std::vector<DaedalusSymbol*> syms {};

		for (uint32_t i = 0; i < parent->count(); ++i) {
			syms.push_back(find_symbol_by_index(parent->index() + i + 1));
		}

		return syms;
	}

	std::vector<DaedalusSymbol const*>
	DaedalusScript::find_parameters_for_function(DaedalusSymbol const* parent) const {
		std::vector<DaedalusSymbol const*> syms {};

		for (uint32_t i = 0; i < parent->count(); ++i) {
			syms.push_back(find_symbol_by_index(parent->index() + i + 1));
		}

		return syms;
	}

	std::vector<DaedalusSymbol*> DaedalusScript::find_class_members(DaedalusSymbol const& cls) {
		std::vector<DaedalusSymbol*> members {};

		for (auto& sym : _m_symbols) {
			if (!sym.is_member() || sym.parent() != cls.index()) continue;
			members.push_back(&sym);
		}

		return members;
	}

	void DaedalusScript::register_as_opaque(DaedalusSymbol* sym) {
		auto members = find_class_members(*sym);

		auto registered_to = &typeid(DaedalusOpaqueInstance);
		uint32_t class_size = 0;

		for (auto* member : members) {
			member->_m_registered_to = registered_to;

			switch (member->type()) {
			case DaedalusDataType::VOID:
			case DaedalusDataType::FLOAT:
			case DaedalusDataType::INT:
			case DaedalusDataType::CLASS:
			case DaedalusDataType::FUNCTION:
			case DaedalusDataType::PROTOTYPE:
			case DaedalusDataType::INSTANCE:
				member->_m_member_offset = class_size;
				class_size += 4 * member->count();
				break;
			case DaedalusDataType::STRING: {
				auto align = alignof(std::string);
				auto offset = class_size;

				auto remain = offset % align;
				auto offset_remain = remain == 0 ? 0 : align - remain;

				class_size += offset_remain;
				member->_m_member_offset = class_size;

				class_size += sizeof(std::string) * member->count();
				break;
			}
			}
		}

		sym->_m_registered_to = registered_to;
		sym->_m_class_size = class_size;
	}

	DaedalusSymbol* DaedalusScript::add_temporary_strings_symbol() {
		DaedalusSymbol sym {};
		sym._m_name = "$PHOENIX_FAKE_STRINGS";
		sym._m_generated = true;
		sym._m_type = DaedalusDataType::STRING;
		sym._m_count = 1;
		sym._m_value = std::unique_ptr<std::string[]> {new std::string[sym._m_count]};
		sym._m_index = static_cast<std::uint32_t>(_m_symbols.size());

		return &_m_symbols.emplace_back(std::move(sym));
	}

	std::uint32_t DaedalusScript::size() const noexcept {
		auto off = _m_text->tell();
		_m_text->seek(0, Whence::END);
		auto size = _m_text->tell();
		_m_text->seek(static_cast<ssize_t>(off), Whence::BEG);
		return static_cast<uint32_t>(size);
	}

	DaedalusSymbol DaedalusSymbol::parse(phoenix::buffer& in) {
		DaedalusSymbol sym {};

		auto r = Read::from(&in);
		sym.load(r.get());

		return sym;
	}

	void zk_internal_escape(std::string& s) {
		for (auto i = 0u; i < s.size(); ++i) {
			if (s[i] == '\\') {
				switch (s[i + 1]) {
				case 'n':
					s[i] = '\n';
					s.erase(i + 1, 1);
					break;
				case 't':
					s[i] = '\t';
					s.erase(i + 1, 1);
					break;
				default:
					break;
				}
			}
		}
	}

	void DaedalusSymbol::load(Read* r) {
		if (r->read_uint() != 0) {
			this->_m_name = r->read_line(false);

			// If the name starts with \xFF, this DaedalusSymbol was automatically generated by the compiler
			if (this->_m_name[0] == '\xFF') {
				this->_m_name[0] = '$';
				this->_m_generated = true;
			}
		}

		auto vary = r->read_uint();
		auto properties = r->read_uint();

		this->_m_count = (properties >> 0U) & 0xFFFU;                              // 12 bits
		this->_m_type = static_cast<DaedalusDataType>((properties >> 12U) & 0xFU); // 4 bits
		this->_m_flags = properties >> 16U & 0x3FU;                                // 6 bits

		if (this->is_member()) {
			this->_m_member_offset = vary;
		} else if (this->type() == DaedalusDataType::CLASS) {
			this->_m_class_size = vary;
		} else if (this->type() == DaedalusDataType::FUNCTION) {
			this->_m_return_type = static_cast<DaedalusDataType>(vary);
		}

		this->_m_file_index = r->read_uint() & 0x7FFFFU;  // 19 bits
		this->_m_line_start = r->read_uint() & 0x7FFFFU;  // 19 bits
		this->_m_line_count = r->read_uint() & 0x7FFFFU;  // 19 bits
		this->_m_char_start = r->read_uint() & 0xFFFFFFU; // 24 bits
		this->_m_char_count = r->read_uint() & 0xFFFFFFU; // 24 bits

		if (!this->is_member()) {
			switch (this->_m_type) {
			case DaedalusDataType::FLOAT: {
				std::unique_ptr<float[]> value {new float[this->_m_count]};
				r->read(value.get(), this->_m_count * sizeof(float));
				this->_m_value = std::move(value);
				break;
			}
			case DaedalusDataType::INT: {
				std::unique_ptr<std::int32_t[]> value {new std::int32_t[this->_m_count]};
				r->read(value.get(), this->_m_count * sizeof(std::uint32_t));
				this->_m_value = std::move(value);
				break;
			}
			case DaedalusDataType::STRING: {
				std::unique_ptr<std::string[]> value {new std::string[this->_m_count]};
				for (std::uint32_t i = 0; i < this->_m_count; ++i) {
					value[i] = r->read_line(false);
					zk_internal_escape(value[i]);
				}
				this->_m_value = std::move(value);
				break;
			}
			case DaedalusDataType::CLASS:
				this->_m_class_offset = r->read_int();
				break;
			case DaedalusDataType::INSTANCE:
				this->_m_value = std::shared_ptr<DaedalusInstance> {nullptr};
				this->_m_address = r->read_int();
				break;
			case DaedalusDataType::FUNCTION:
				if (!this->is_const()) {
					this->_m_value = std::unique_ptr<std::int32_t[]>(new int32_t[1]);
				}
				this->_m_address = r->read_int();
				break;
			case DaedalusDataType::PROTOTYPE:
				this->_m_address = r->read_int();
				break;
			default:
				break;
			}
		}

		this->_m_parent = r->read_int();
		if (this->type() == DaedalusDataType::STRING && !this->is_member() && this->is_const() &&
		    std::isspace(this->_m_parent & 0xFF)) {

			// Possible string newline issues here.
			auto savepoint = r->tell();

			// Only lookahead four bytes max.
			unsigned byte = 4;
			for (; byte > 0; --byte) {
				r->seek(-3, Whence::CUR);

				if (auto parent_index = r->read_int(); parent_index == -1) {
					// This parent index is valid.
					this->_m_parent = parent_index;
					break;
				}
			}

			if (byte == 0) {
				// We didn't find any match.
				r->seek(static_cast<ssize_t>(savepoint), Whence::BEG);
				this->_m_parent = r->read_int();
				ZKLOGW("DaedalusSymbol", "Heuristic: No valid endpoint found. Aborting search. Issues might arise.");
			}
		}
	}

	std::string const& DaedalusSymbol::get_string(std::uint16_t index, DaedalusInstance const* context) const {
		if (type() != DaedalusDataType::STRING) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::STRING);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}

			if (context->symbol_index() == static_cast<uint32_t>(-1) &&
			    context->_m_type == &typeid(DaedalusTransientInstance)) {
				return reinterpret_cast<DaedalusTransientInstance const&>(*context).get_string(*this, index);
			}

			return *get_member_ptr<std::string>(index, context);
		}

		return std::get<std::unique_ptr<std::string[]>>(_m_value)[index];
	}

	float DaedalusSymbol::get_float(std::uint16_t index, DaedalusInstance const* context) const {
		if (type() != DaedalusDataType::FLOAT) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::FLOAT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}

			if (context->symbol_index() == static_cast<uint32_t>(-1) &&
			    context->_m_type == &typeid(DaedalusTransientInstance)) {
				return reinterpret_cast<DaedalusTransientInstance const&>(*context).get_float(*this, index);
			}

			return *get_member_ptr<float>(static_cast<uint8_t>(index), context);
		}

		return std::get<std::unique_ptr<float[]>>(_m_value)[index];
	}

	std::int32_t DaedalusSymbol::get_int(std::uint16_t index, DaedalusInstance const* context) const {
		if (type() != DaedalusDataType::INT && type() != DaedalusDataType::FUNCTION) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}

			if (context->symbol_index() == static_cast<uint32_t>(-1) &&
			    context->_m_type == &typeid(DaedalusTransientInstance)) {
				return reinterpret_cast<DaedalusTransientInstance const&>(*context).get_int(*this, index);
			}

			return *get_member_ptr<std::int32_t>(index, context);
		}

		return std::get<std::unique_ptr<std::int32_t[]>>(_m_value)[index];
	}

	void DaedalusSymbol::set_string(std::string_view value, std::uint16_t index, DaedalusInstance* context) {
		if (type() != DaedalusDataType::STRING) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::STRING);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}

			if (context->symbol_index() == static_cast<uint32_t>(-1) &&
			    context->_m_type == &typeid(DaedalusTransientInstance)) {
				reinterpret_cast<DaedalusTransientInstance&>(*context).set_string(*this, index, value);
				return;
			}

			*get_member_ptr<std::string>(index, context) = value;
		} else {
			std::get<std::unique_ptr<std::string[]>>(_m_value).get()[index] = value;
		}
	}

	void DaedalusSymbol::set_float(float value, std::uint16_t index, DaedalusInstance* context) {
		if (type() != DaedalusDataType::FLOAT) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::FLOAT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}

			if (context->symbol_index() == static_cast<uint32_t>(-1) &&
			    context->_m_type == &typeid(DaedalusTransientInstance)) {
				reinterpret_cast<DaedalusTransientInstance&>(*context).set_float(*this, index, value);
				return;
			}

			*get_member_ptr<float>(index, context) = value;
		} else {
			std::get<std::unique_ptr<float[]>>(_m_value)[index] = value;
		}
	}

	void DaedalusSymbol::set_int(std::int32_t value, std::uint16_t index, DaedalusInstance* context) {
		if (type() != DaedalusDataType::INT && type() != DaedalusDataType::FUNCTION) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INT);
		}
		if (count() <= index) {
			throw DaedalusIllegalIndexAccess(this, index);
		}

		if (is_member()) {
			if (context == nullptr) {
				throw DaedalusNoContextError(this);
			}

			if (context->symbol_index() == static_cast<uint32_t>(-1) &&
			    context->_m_type == &typeid(DaedalusTransientInstance)) {
				reinterpret_cast<DaedalusTransientInstance&>(*context).set_int(*this, index, value);
				return;
			}

			*get_member_ptr<std::int32_t>(index, context) = value;
		} else {
			std::get<std::unique_ptr<std::int32_t[]>>(_m_value)[index] = value;
		}
	}

	std::shared_ptr<DaedalusInstance> const& DaedalusSymbol::get_instance() {
		if (type() != DaedalusDataType::INSTANCE) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INSTANCE);
		}

		return std::get<std::shared_ptr<DaedalusInstance>>(_m_value);
	}

	void DaedalusSymbol::set_instance(std::shared_ptr<DaedalusInstance> const& inst) {
		if (type() != DaedalusDataType::INSTANCE) {
			throw DaedalusIllegalTypeAccess(this, DaedalusDataType::INSTANCE);
		}

		std::get<std::shared_ptr<DaedalusInstance>>(_m_value) = inst;
	}

	void DaedalusSymbol::set_access_trap_enable(bool enable) noexcept {
		if (enable)
			_m_flags |= DaedalusSymbolFlag::TRAP_ACCESS;
		else
			_m_flags &= ~DaedalusSymbolFlag::TRAP_ACCESS;
	}

	DaedalusOpaqueInstance::DaedalusOpaqueInstance(DaedalusSymbol const& sym,
	                                               std::vector<DaedalusSymbol*> const& members) {
		size_t str_count = 0;
		for (auto* member : members) {
			if (member->type() != DaedalusDataType::STRING) continue;
			str_count += member->count();
		}

		_m_storage.reset(new uint8_t[sym.class_size()]);
		_m_strings.resize(str_count, nullptr);

		str_count = 0;
		for (auto* member : members) {
			unsigned offset = member->offset_as_member();

			for (auto i = 0U; i < member->count(); ++i) {
				switch (member->type()) {
				case DaedalusDataType::FLOAT:
					this->construct_at<float>(offset, 0.f);
					offset += 4;
					break;
				case DaedalusDataType::INT:
					this->construct_at<int>(offset, 0);
					offset += 4;
					break;
				case DaedalusDataType::STRING:
					_m_strings[str_count] = this->construct_at<std::string>(offset, "");
					str_count++;
					offset += sizeof(std::string);
					break;
				case DaedalusDataType::FUNCTION:
					this->construct_at<int>(offset, 0);
					offset += 4;
					break;
				case DaedalusDataType::CLASS:
				case DaedalusDataType::PROTOTYPE:
				case DaedalusDataType::INSTANCE:
				case DaedalusDataType::VOID:
					this->construct_at<int>(offset);
					offset += 4;
					break;
				}
			}
		}
	}

	DaedalusOpaqueInstance::~DaedalusOpaqueInstance() {
		for (auto& i : _m_strings)
			i->std::string::~string();
	}

	template <typename T, typename... Args>
	T* DaedalusOpaqueInstance::construct_at(size_t offset, Args&&... args) {
		auto align = alignof(T);
		auto remain = offset % align;
		auto real_offset = remain == 0 ? offset : offset + (align - remain);
		return new (static_cast<void*>(&_m_storage[real_offset])) T(std::forward<Args>(args)...);
	}

	DaedalusTransientInstance::DaedalusTransientInstance() {
		_m_type = &typeid(DaedalusTransientInstance);
	}
} // namespace zenkit
