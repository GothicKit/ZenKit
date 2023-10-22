// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/DaedalusVm.hh"

#include "Internal.hh"

#include <utility>

namespace zenkit {
	/// \brief A helper class for preventing stack corruption.
	///
	/// This class can be used to guard against stack corruption when a value is expected to be
	/// pushed onto the stack but the push does not happen for some reason. Unless #inhibit is called,
	/// when an instance of this class is destructed, a default value of the given datatype is pushed
	/// into the stack.
	///
	/// \code
	/// // construct the stack guard
	/// stack_guard guard {this, DaedalusDataType::int_};
	///
	/// // ... do something that might fail to push an int to the stack ...
	///
	/// // make sure to inhibit the guard if pushing the value succeeded
	/// guard.inhibit();
	/// \endcode
	struct StackGuard {
	public:
		/// \brief Creates a new stack guard.
		/// \param machine The VM this instance is guarding.
		/// \param type The type of value to push if the guard is triggered.
		StackGuard(DaedalusVm* machine, DaedalusDataType type) : _m_type(type), _m_machine(machine) {}

		/// \brief Triggers this guard.
		///
		/// Unless #inhibit was called, this destructor will push a value of the datatype passed in the
		/// constructor onto the stack of the VM passed in the constructor.
		StackGuard() {
			if (_m_inhibited)
				return;

			switch (_m_type) {
			case DaedalusDataType::VOID:
				break;
			case DaedalusDataType::FLOAT:
				_m_machine->push_float(0);
				break;
			case DaedalusDataType::INT:
			case DaedalusDataType::FUNCTION:
				_m_machine->push_int(0);
				break;
			case DaedalusDataType::STRING:
				_m_machine->push_string("");
				break;
			case DaedalusDataType::INSTANCE:
				_m_machine->push_instance(nullptr);
				break;
			case DaedalusDataType::CLASS:
				break;
			case DaedalusDataType::PROTOTYPE:
				break;
			}
		}

		/// \brief Inhibits this guard.
		///
		/// Calling this function will cause the guard to disengage and thus not push a
		/// value onto the stack.
		void inhibit() {
			_m_inhibited = true;
		}

	private:
		DaedalusDataType _m_type;
		DaedalusVm* _m_machine;
		bool _m_inhibited {false};
	};

	DaedalusVm::DaedalusVm(DaedalusScript&& scr, std::uint8_t flags) : DaedalusScript(std::move(scr)), _m_flags(flags) {
		_m_temporary_strings = add_temporary_strings_symbol();
		_m_self_sym = find_symbol_by_name("SELF");
		_m_other_sym = find_symbol_by_name("OTHER");
		_m_victim_sym = find_symbol_by_name("VICTIM");
		_m_hero_sym = find_symbol_by_name("HERO");
		_m_item_sym = find_symbol_by_name("ITEM");
	}

	std::shared_ptr<DaedalusInstance> DaedalusVm::init_opaque_instance(DaedalusSymbol* sym) {
		auto cls = sym;
		while (cls != nullptr && cls->type() != DaedalusDataType::CLASS) {
			cls = find_symbol_by_index(cls->parent());
		}
		if (cls == nullptr) {
			// We're probably trying to initialize $INSTANCE_HELP which is not permitted
			throw DaedalusVmException {"Cannot init " + sym->name() +
			                           ": parent class not found (did you try to initialize $INSTANCE_HELP?)"};
		}

		// create the instance
		auto inst = std::make_shared<DaedalusOpaqueInstance>(*cls, find_class_members(*cls));
		init_instance(inst, sym);
		return inst;
	}

	void DaedalusVm::unsafe_call(const DaedalusSymbol* sym) {
		push_call(sym);
		jump(sym->address());

		// execute until an op_return is reached
		while (exec())
			;

		pop_call();
	}

	void DaedalusVm::unsafe_jump(uint32_t address) {
		this->jump(address);
	}

	bool DaedalusVm::exec() {
		auto instr = instruction_at(_m_pc);

		std::int32_t a {}, b {};
		DaedalusSymbol* sym {};

		try {
			switch (instr.op) {
			case DaedalusOpcode::ADD:
				push_int(pop_int() + pop_int());
				break;
			case DaedalusOpcode::SUB:
				a = pop_int();
				b = pop_int();
				push_int(a - b);
				break;
			case DaedalusOpcode::MUL:
				push_int(pop_int() * pop_int());
				break;
			case DaedalusOpcode::DIV:
				a = pop_int();
				b = pop_int();

				if (b == 0)
					throw DaedalusVmException {"vm: division by zero"};

				push_int(a / b);
				break;
			case DaedalusOpcode::MOD:
				a = pop_int();
				b = pop_int();

				if (b == 0)
					throw DaedalusVmException {"vm: division by zero"};

				push_int(a % b);
				break;
			case DaedalusOpcode::OR:
				push_int(pop_int() | pop_int());
				break;
			case DaedalusOpcode::ANDB:
				push_int(pop_int() & pop_int());
				break;
			case DaedalusOpcode::LT:
				a = pop_int();
				b = pop_int();
				push_int(a < b);
				break;
			case DaedalusOpcode::GT:
				a = pop_int();
				b = pop_int();
				push_int(a > b);
				break;
			case DaedalusOpcode::LSL:
				a = pop_int();
				b = pop_int();
				push_int(a << b);
				break;
			case DaedalusOpcode::LSR:
				a = pop_int();
				b = pop_int();
				push_int(a >> b);
				break;
			case DaedalusOpcode::LTE:
				a = pop_int();
				b = pop_int();
				push_int(a <= b);
				break;
			case DaedalusOpcode::EQ:
				push_int(pop_int() == pop_int());
				break;
			case DaedalusOpcode::NEQ:
				push_int(pop_int() != pop_int());
				break;
			case DaedalusOpcode::GTE:
				a = pop_int();
				b = pop_int();
				push_int(a >= b);
				break;
			case DaedalusOpcode::PLUS:
				push_int(+pop_int());
				break;
			case DaedalusOpcode::NEGATE:
				push_int(-pop_int());
				break;
			case DaedalusOpcode::NOT:
				push_int(!pop_int());
				break;
			case DaedalusOpcode::CMPL:
				push_int(~pop_int());
				break;
			case DaedalusOpcode::ORR:
				a = pop_int();
				b = pop_int();
				push_int(a || b);
				break;
			case DaedalusOpcode::AND:
				a = pop_int();
				b = pop_int();
				push_int(a && b);
				break;
			case DaedalusOpcode::NOP:
				// Do nothing
				break;
			case DaedalusOpcode::RSR:
				return false;
			case DaedalusOpcode::BL: {
				// Check if the function is overridden and if it is, call the resulting external.
				sym = find_symbol_by_address(instr.address);
				auto cb = _m_function_overrides.find(instr.address);
				if (cb != _m_function_overrides.end()) {
					// Guard against exceptions during external invocation.
					StackGuard guard {this, sym->rtype()};
					// Call maybe naked.
					cb->second(*this);
					// The stack is left intact.
					guard.inhibit();
				} else {
					if (sym == nullptr) {
						throw DaedalusVmException {"bl: no symbol found for address " + std::to_string(instr.address)};
					}

					unsafe_call(sym);
				}

				break;
			}
			case DaedalusOpcode::BE: {
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw DaedalusVmException {"be: no external found for index"};
				}

				// Guard against exceptions during external invocation.
				StackGuard guard {this, sym->rtype()};

				auto cb = _m_externals.find(sym);
				if (cb == _m_externals.end()) {
					if (_m_default_external.has_value()) {
						(*_m_default_external)(*this, *sym);
						guard.inhibit();
						break;
					} else {
						throw DaedalusVmException {"be: no external registered for " + sym->name()};
					}
				}

				push_call(sym);
				cb->second(*this);
				pop_call();

				// The stack is left intact.
				guard.inhibit();
				break;
			}
			case DaedalusOpcode::PUSHI:
				push_int(instr.immediate);
				break;
			case DaedalusOpcode::PUSHVI:
			case DaedalusOpcode::PUSHV:
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw DaedalusVmException {"pushv: no symbol found for index"};
				}
				if (sym->has_access_trap() && _m_access_trap) {
					_m_access_trap(*sym);
				} else {
					push_reference(sym, 0);
				}
				break;
			case DaedalusOpcode::MOVI:
			case DaedalusOpcode::MOVVF: {
				auto [ref, idx, context] = pop_reference();
				auto value = pop_int();

				this->set_int(context, ref, idx, value);
				break;
			}
			case DaedalusOpcode::MOVF: {
				auto [ref, idx, context] = pop_reference();
				auto value = pop_float();

				this->set_float(context, ref, idx, value);
				break;
			}
			case DaedalusOpcode::MOVS: {
				auto [target, target_idx, context] = pop_reference();
				auto source = pop_string();

				this->set_string(context, target, target_idx, source);
				break;
			}
			case DaedalusOpcode::MOVSS:
				throw DaedalusVmException {"not implemented: movss"};
			case DaedalusOpcode::ADDMOVI: {
				auto [ref, idx, context] = pop_reference();
				auto value = pop_int();

				if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
					throw DaedalusIllegalConstAccess(ref);
				}

				if (!ref->is_member() || context != nullptr ||
				    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
					auto result = ref->get_int(idx, context) + value;
					ref->set_int(result, idx, context);
				} else if (ref->is_member()) {
					ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
				}

				break;
			}
			case DaedalusOpcode::SUBMOVI: {
				auto [ref, idx, context] = pop_reference();
				auto value = pop_int();

				if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
					throw DaedalusIllegalConstAccess(ref);
				}

				if (!ref->is_member() || context != nullptr ||
				    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
					auto result = ref->get_int(idx, context) - value;
					ref->set_int(result, idx, context);
				} else if (ref->is_member()) {
					ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
				}
				break;
			}
			case DaedalusOpcode::MULMOVI: {
				auto [ref, idx, context] = pop_reference();
				auto value = pop_int();

				if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
					throw DaedalusIllegalConstAccess(ref);
				}

				if (!ref->is_member() || context != nullptr ||
				    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
					auto result = ref->get_int(idx, context) * value;
					ref->set_int(result, idx, context);
				} else if (ref->is_member()) {
					ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
				}

				break;
			}
			case DaedalusOpcode::DIVMOVI: {
				auto [ref, idx, context] = pop_reference();
				auto value = pop_int();

				if (value == 0) {
					throw DaedalusVmException {"vm: division by zero"};
				}

				if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
					throw DaedalusIllegalConstAccess(ref);
				}

				if (!ref->is_member() || context != nullptr ||
				    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
					auto result = ref->get_int(idx, context) / value;
					ref->set_int(result, idx, context);
				} else if (ref->is_member()) {
					ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
				}

				break;
			}
			case DaedalusOpcode::MOVVI: {
				auto [target, target_idx, _] = pop_reference();
				target->set_instance(pop_instance());
				break;
			}
			case DaedalusOpcode::B:
				jump(instr.address);
				return true;
			case DaedalusOpcode::BZ:
				if (pop_int() == 0) {
					jump(instr.address);
					return true;
				}
				break;
			case DaedalusOpcode::GMOVI: {
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw DaedalusVmException {"gmovi: no symbol found for index"};
				}
				_m_instance = sym->get_instance();
				break;
			}
			case DaedalusOpcode::PUSHVV:
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw DaedalusVmException {"pushvv: no symbol found for index"};
				}

				push_reference(sym, instr.index);
				break;
			}

			_m_pc += instr.size;
		} catch (zenkit::DaedalusScriptError& err) {
			uint32_t prev_pc = _m_pc;

			if (_m_exception_handler) {
				auto strategy = (*_m_exception_handler)(*this, err, instr);

				if (strategy == DaedalusVmExceptionStrategy::FAIL) {
					ZKLOGE("DaedalusVm", "+++ Error while executing script: %s +++", err.what());
					print_stack_trace();
					throw err;
				} else if (strategy == DaedalusVmExceptionStrategy::RETURN) {
					return false;
				}
			} else {
				ZKLOGE("DaedalusVm", "+++ Error while executing script: %s +++", err.what());
				print_stack_trace();
				throw err;
			}

			if (_m_pc == prev_pc) {
				_m_pc += instr.size;
			}
		}

		return true;
	}

	void DaedalusVm::push_call(const DaedalusSymbol* sym) {
		_m_call_stack.push({sym, _m_pc, _m_instance});
	}

	void DaedalusVm::pop_call() {
		const auto& call = _m_call_stack.top();
		_m_pc = call.program_counter;
		_m_instance = call.context;
		_m_call_stack.pop();
	}

	void DaedalusVm::push_int(std::int32_t value) {
		if (_m_stack_ptr == DaedalusVm::stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {nullptr, false, value};
	}

	void DaedalusVm::push_reference(DaedalusSymbol* value, std::uint8_t index) {
		if (_m_stack_ptr == DaedalusVm::stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {_m_instance, true, value, index};
	}

	void DaedalusVm::push_string(std::string_view value) {
		_m_temporary_strings->set_string(value);
		push_reference(_m_temporary_strings);
	}

	void DaedalusVm::push_float(float value) {
		if (_m_stack_ptr == DaedalusVm::stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {nullptr, false, value};
	}

	void DaedalusVm::push_instance(std::shared_ptr<DaedalusInstance> value) {
		if (_m_stack_ptr == DaedalusVm::stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {nullptr, false, value};
	}

	std::int32_t DaedalusVm::pop_int() {
		if (_m_stack_ptr == 0) {
			return 0;
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (v.reference) {
			return this->get_int(v.context, v.value, v.index);
		} else if (std::holds_alternative<int32_t>(v.value)) {
			return std::get<int32_t>(v.value);
		} else {
			throw DaedalusVmException {"tried to pop_int but frame does not contain a int."};
		}
	}

	float DaedalusVm::pop_float() {
		if (_m_stack_ptr == 0) {
			return 0.0f;
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (v.reference) {
			return this->get_float(v.context, v.value, v.index);
		} else if (std::holds_alternative<float>(v.value)) {
			return std::get<float>(v.value);
		} else if (std::holds_alternative<std::int32_t>(v.value)) {
			auto k = std::get<std::int32_t>(v.value);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
			return *(float*) &k;
#pragma GCC diagnostic pop
		} else {
			throw DaedalusVmException {"tried to pop_float but frame does not contain a float."};
		}
	}

	std::tuple<DaedalusSymbol*, std::uint8_t, std::shared_ptr<DaedalusInstance>> DaedalusVm::pop_reference() {
		if (_m_stack_ptr == 0) {
			throw DaedalusVmException {"popping reference from empty stack"};
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (!v.reference) {
			throw DaedalusVmException {"tried to pop_reference but frame does not contain a reference."};
		}

		return {std::get<DaedalusSymbol*>(v.value), v.index, v.context};
	}

	std::shared_ptr<DaedalusInstance> DaedalusVm::pop_instance() {
		if (_m_stack_ptr == 0) {
			throw DaedalusVmException {"popping instance from empty stack"};
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (v.reference) {
			return std::get<DaedalusSymbol*>(v.value)->get_instance();
		} else if (std::holds_alternative<std::shared_ptr<DaedalusInstance>>(v.value)) {
			return std::get<std::shared_ptr<DaedalusInstance>>(v.value);
		} else {
			throw DaedalusVmException {"tried to pop_instance but frame does not contain am instance."};
		}
	}

	const std::string& DaedalusVm::pop_string() {
		static std::string empty {};
		auto [s, i, context] = pop_reference();

		// compatibility: sometimes the context might be zero, but we can't fail so when
		//                the compatibility flag is set, we just return 0
		if (s->is_member() && context == nullptr) {
			if (!(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
				throw DaedalusNoContextError {s};
			}

			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", s->name().c_str());
			return empty;
		}

		return s->get_string(i, context);
	}

	void DaedalusVm::jump(std::uint32_t address) {
		if (address > size()) {
			throw DaedalusVmException {"Cannot jump to " + std::to_string(address) + ": illegal address"};
		}

		_m_pc = address;
	}

	void DaedalusVm::register_default_external(const std::function<void(std::string_view)>& callback) {
		_m_default_external = [this, callback](DaedalusVm& v, DaedalusSymbol& sym) {
			// pop all parameters from the stack
			auto params = find_parameters_for_function(&sym);
			for (int32_t i = params.size() - 1; i >= 0; --i) {
				auto par = params[i];

				if (par->type() == DaedalusDataType::INT)
					(void) v.pop_int();
				else if (par->type() == DaedalusDataType::FLOAT)
					(void) v.pop_float();
				else if (par->type() == DaedalusDataType::INSTANCE || par->type() == DaedalusDataType::STRING)
					(void) v.pop_reference();
			}

			if (sym.has_return()) {
				if (sym.rtype() == DaedalusDataType::FLOAT)
					v.push_float(0.0f);
				else if (sym.rtype() == DaedalusDataType::INT)
					v.push_int(0);
				else if (sym.rtype() == DaedalusDataType::STRING)
					v.push_string("");
				else if (sym.rtype() == DaedalusDataType::INSTANCE)
					v.push_instance(nullptr);
			}

			callback(sym.name());
		};
	}

	void
	DaedalusVm::register_default_external_custom(const std::function<void(DaedalusVm&, DaedalusSymbol&)>& callback) {
		_m_default_external = callback;
	}

	void DaedalusVm::register_access_trap(const std::function<void(DaedalusSymbol&)>& callback) {
		_m_access_trap = callback;
	}

	void DaedalusVm::register_exception_handler(
	    const std::function<DaedalusVmExceptionStrategy(DaedalusVm&,
	                                                    const DaedalusScriptError&,
	                                                    const DaedalusInstruction&)>& callback) {
		_m_exception_handler = callback;
	}

	void DaedalusVm::print_stack_trace() const {
		auto last_pc = _m_pc;
		auto tmp_stack_ptr = _m_stack_ptr;

		std::stack<DaedalusCallStackFrame> callstack {_m_call_stack};

		ZKLOGE("DaedalusVm", "------- CALL STACK (MOST RECENT CALL FIRST) -------");

		while (!callstack.empty()) {
			auto v = callstack.top();
			ZKLOGE("DaedalusVm", "in %s at %x", v.function->name().c_str(), last_pc);

			last_pc = v.program_counter;
			callstack.pop();
		}

		ZKLOGE("DaedalusVm", "------- STACK (MOST RECENT PUSH FIRST) -------");

		while (tmp_stack_ptr > 0) {
			auto& v = _m_stack[--tmp_stack_ptr];

			if (v.reference) {
				auto ref = std::get<DaedalusSymbol*>(v.value);
				std::string value;

				switch (ref->type()) {
				case DaedalusDataType::FLOAT:
					value = std::to_string(ref->get_float(v.index, _m_instance));
					break;
				case DaedalusDataType::INT:
					value = std::to_string(ref->get_int(v.index, _m_instance));
					break;
				case DaedalusDataType::STRING:
					value = "'" + ref->get_string(v.index, _m_instance) + "'";
					break;
				case DaedalusDataType::FUNCTION: {
					auto index = ref->get_int(v.index, _m_instance);
					auto sym = find_symbol_by_index(index);
					value = "&" + sym->name();
					break;
				}
				case DaedalusDataType::INSTANCE: {
					auto& inst = ref->get_instance();
					if (inst != nullptr) {
						value = "<instance of '" + std::string(inst->_m_type->name()) + "'>";
					} else {
						value = "NULL";
					}
					break;
				}
				default:
					value = "<invalid stack frame>";
				}

				ZKLOGE("DaedalusVm",
				       "%u: [REFERENCE] %s[%hu] = %s",
				       tmp_stack_ptr,
				       ref->name().c_str(),
				       v.index,
				       value.c_str());
			} else {
				if (std::holds_alternative<float>(v.value)) {
					ZKLOGE("DaedalusVm", "%d: [IMMEDIATE FLOAT] = %f", tmp_stack_ptr, std::get<float>(v.value));
				} else if (std::holds_alternative<int32_t>(v.value)) {
					ZKLOGE("DaedalusVm", "%d: [IMMEDIATE INT] = %d", tmp_stack_ptr, std::get<int32_t>(v.value));
				} else if (std::holds_alternative<std::shared_ptr<DaedalusInstance>>(v.value)) {
					auto& inst = std::get<std::shared_ptr<DaedalusInstance>>(v.value);
					if (inst == nullptr) {
						ZKLOGE("DaedalusVm", "%d: [IMMEDIATE INSTANCE] = NULL", tmp_stack_ptr);
					} else {
						ZKLOGE("DaedalusVm",
						       "%d: [IMMEDIATE INSTANCE] = <instance of '%s'>",
						       tmp_stack_ptr,
						       inst->_m_type->name());
					}
				}
			}
		}
	}

	DaedalusIllegalExternalDefinition::DaedalusIllegalExternalDefinition(const DaedalusSymbol* s, std::string&& msg)
	    : DaedalusScriptError(std::move(msg)), sym(s) {}

	DaedalusIllegalExternalReturnType::DaedalusIllegalExternalReturnType(const DaedalusSymbol* s,
	                                                                     std::string&& provided)
	    : DaedalusIllegalExternalDefinition(s,
	                                        "external " + s->name() + " has illegal return type '" + provided +
	                                            "', expected '" + DAEDALUS_DATA_TYPE_NAMES[(std::uint32_t) s->rtype()] +
	                                            "'") {}

	DaedalusIllegalExternalParameter::DaedalusIllegalExternalParameter(const DaedalusSymbol* s,
	                                                                   std::string&& provided,
	                                                                   std::uint8_t i)
	    : DaedalusIllegalExternalDefinition(s,
	                                        "external " + s->name() + " has illegal parameter type '" + provided +
	                                            "' (no. " + std::to_string(i) + "), expected '" +
	                                            DAEDALUS_DATA_TYPE_NAMES[(std::uint32_t) s->type()] + "'") {}

	DaedalusVmExceptionStrategy
	lenient_vm_exception_handler(DaedalusVm& v, const DaedalusScriptError& exc, const DaedalusInstruction& instr) {
		ZKLOGE("DaedalusVm", "Internal Exception: %s", exc.what());

		switch (instr.op) {
		case DaedalusOpcode::ADD:
		case DaedalusOpcode::SUB:
		case DaedalusOpcode::MUL:
		case DaedalusOpcode::DIV:
		case DaedalusOpcode::MOD:
		case DaedalusOpcode::OR:
		case DaedalusOpcode::ANDB:
		case DaedalusOpcode::LT:
		case DaedalusOpcode::GT:
		case DaedalusOpcode::ORR:
		case DaedalusOpcode::AND:
		case DaedalusOpcode::LSL:
		case DaedalusOpcode::LSR:
		case DaedalusOpcode::LTE:
		case DaedalusOpcode::EQ:
		case DaedalusOpcode::NEQ:
		case DaedalusOpcode::GTE:
		case DaedalusOpcode::PLUS:
		case DaedalusOpcode::NEGATE:
		case DaedalusOpcode::NOT:
		case DaedalusOpcode::CMPL:
			v.push_int(0);
			break;

		case DaedalusOpcode::ADDMOVI:
		case DaedalusOpcode::SUBMOVI:
		case DaedalusOpcode::MULMOVI:
		case DaedalusOpcode::DIVMOVI:
		case DaedalusOpcode::MOVI:
		case DaedalusOpcode::MOVS:
		case DaedalusOpcode::MOVSS:
		case DaedalusOpcode::MOVVF:
		case DaedalusOpcode::MOVF:
		case DaedalusOpcode::MOVVI:
			// do nothing for now but ideally, this would check the exception and/or remove the offending values
			// from the stack.
			break;
		case DaedalusOpcode::NOP:
		case DaedalusOpcode::RSR:
		case DaedalusOpcode::B:
		case DaedalusOpcode::BZ:
			// if these fail, something has gone horribly wrong. still ignore the error though.
			break;

		case DaedalusOpcode::BL:
		case DaedalusOpcode::BE:
			// ignore the branch
			break;

		case DaedalusOpcode::PUSHI:
			v.push_int(0);
			break;
		case DaedalusOpcode::PUSHV:
		case DaedalusOpcode::PUSHVI:
		case DaedalusOpcode::PUSHVV:
			// push an int and hope it's the right type!
			v.push_int(0);
			break;

		case DaedalusOpcode::GMOVI:
			// do nothing for now but ideally, this would set the `null` instance
			break;
		}

		return DaedalusVmExceptionStrategy::CONTINUE;
	}

	std::int32_t
	DaedalusVm::get_int(std::shared_ptr<DaedalusInstance>& context,
	                    std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>>& value,
	                    uint16_t index) {
		auto* sym = std::get<DaedalusSymbol*>(value);

		// compatibility: sometimes the context might be zero, but we can't fail so when
		//                the compatibility flag is set, we just return 0
		if (sym->is_member() && context == nullptr) {
			if (!(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
				throw DaedalusNoContextError {sym};
			}

			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", sym->name().c_str());
			return 0;
		}

		return sym->get_int(index, context);
	}

	float DaedalusVm::get_float(std::shared_ptr<DaedalusInstance>& context,
	                            std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>>& value,
	                            uint16_t index) {
		auto* sym = std::get<DaedalusSymbol*>(value);

		// compatibility: sometimes the context might be zero, but we can't fail so when
		//                the compatibility flag is set, we just return 0
		if (sym->is_member() && context == nullptr) {
			if (!(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
				throw DaedalusNoContextError {sym};
			}

			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", sym->name().c_str());
			return 0;
		}

		return sym->get_float(index, context);
	}

	void DaedalusVm::set_int(std::shared_ptr<DaedalusInstance>& context,
	                         DaedalusSymbol* ref,
	                         uint16_t index,
	                         std::int32_t value) {
		if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
			throw DaedalusIllegalConstAccess {ref};
		}

		if (!ref->is_member() || context != nullptr ||
		    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
			ref->set_int(value, index, context);
		} else if (ref->is_member()) {
			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
		}
	}

	void DaedalusVm::set_float(std::shared_ptr<DaedalusInstance>& context,
	                           DaedalusSymbol* ref,
	                           uint16_t index,
	                           float value) {
		if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
			throw DaedalusIllegalConstAccess {ref};
		}

		if (!ref->is_member() || context != nullptr ||
		    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
			ref->set_float(value, index, context);
		} else if (ref->is_member()) {
			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
		}
	}

	void DaedalusVm::set_string(std::shared_ptr<DaedalusInstance>& context,
	                            DaedalusSymbol* ref,
	                            uint16_t index,
	                            std::string_view value) {
		if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
			throw DaedalusIllegalConstAccess {ref};
		}

		if (!ref->is_member() || context != nullptr ||
		    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
			ref->set_string(value, index, context);
		} else if (ref->is_member()) {
			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
		}
	}
} // namespace zenkit
