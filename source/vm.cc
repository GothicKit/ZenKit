// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/vm.hh>

#include <iostream>
#include <utility>

namespace phoenix {
	/// \brief A helper class for preventing stack corruption.
	///
	/// This class can be used to guard against stack corruption when a value is expected to be
	/// pushed onto the stack but the push does not happen for some reason. Unless #inhibit is called,
	/// when an instance of this class is destructed, a default value of the given datatype is pushed
	/// into the stack.
	///
	/// \code
	/// // construct the stack guard
	/// stack_guard guard {this, datatype::int_};
	///
	/// // ... do something that might fail to push an int to the stack ...
	///
	/// // make sure to inhibit the guard if pushing the value succeeded
	/// guard.inhibit();
	/// \endcode
	struct stack_guard {
	public:
		/// \brief Creates a new stack guard.
		/// \param machine The VM this instance is guarding.
		/// \param type The type of value to push if the guard is triggered.
		stack_guard(vm* machine, datatype type) : _m_type(type), _m_machine(machine) {}

		/// \brief Triggers this guard.
		///
		/// Unless #inhibit was called, this destructor will push a value of the datatype passed in the
		/// constructor onto the stack of the VM passed in the constructor.
		~stack_guard() {
			if (_m_inhibited)
				return;

			switch (_m_type) {
			case datatype::void_:
				break;
			case datatype::float_:
				_m_machine->push_float(0);
				break;
			case datatype::integer:
			case datatype::function:
				_m_machine->push_int(0);
				break;
			case datatype::string:
				_m_machine->push_string("");
				break;
			case datatype::instance:
				_m_machine->push_instance(nullptr);
				break;
			case datatype::class_:
				break;
			case datatype::prototype:
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
		datatype _m_type;
		vm* _m_machine;
		bool _m_inhibited {false};
	};

	vm::vm(script&& scr, std::uint8_t flags) : script(std::move(scr)), _m_flags(flags) {
		_m_self_sym = find_symbol_by_name("SELF");
		_m_other_sym = find_symbol_by_name("OTHER");
		_m_victim_sym = find_symbol_by_name("VICTIM");
		_m_hero_sym = find_symbol_by_name("HERO");
		_m_item_sym = find_symbol_by_name("ITEM");
		_m_temporary_strings = add_temporary_strings_symbol();
	}

	void vm::call(const symbol* sym) {
		push_call(sym);
		jump(sym->address());

		// execute until an op_return is reached
		while (exec())
			;

		pop_call();
	}

	bool vm::exec() {
		auto instr = instruction_at(_m_pc);

		std::int32_t a {}, b {};
		symbol* sym {};

		try {
			switch (instr.op) {
			case opcode::add:
				push_int(pop_int() + pop_int());
				break;
			case opcode::sub:
				a = pop_int();
				b = pop_int();
				push_int(a - b);
				break;
			case opcode::mul:
				push_int(pop_int() * pop_int());
				break;
			case opcode::div:
				a = pop_int();
				b = pop_int();
				push_int(a / b);
				break;
			case opcode::mod:
				a = pop_int();
				b = pop_int();
				push_int(a % b);
				break;
			case opcode::or_:
				push_int(pop_int() | pop_int());
				break;
			case opcode::andb:
				push_int(pop_int() & pop_int());
				break;
			case opcode::lt:
				a = pop_int();
				b = pop_int();
				push_int(a < b);
				break;
			case opcode::gt:
				a = pop_int();
				b = pop_int();
				push_int(a > b);
				break;
			case opcode::lsl:
				a = pop_int();
				b = pop_int();
				push_int(a << b);
				break;
			case opcode::lsr:
				a = pop_int();
				b = pop_int();
				push_int(a >> b);
				break;
			case opcode::lte:
				a = pop_int();
				b = pop_int();
				push_int(a <= b);
				break;
			case opcode::eq:
				push_int(pop_int() == pop_int());
				break;
			case opcode::neq:
				push_int(pop_int() != pop_int());
				break;
			case opcode::gte:
				a = pop_int();
				b = pop_int();
				push_int(a >= b);
				break;
			case opcode::plus:
				push_int(+pop_int());
				break;
			case opcode::negate:
				push_int(-pop_int());
				break;
			case opcode::not_:
				push_int(!pop_int());
				break;
			case opcode::cmpl:
				push_int(~pop_int());
				break;
			case opcode::orr:
				a = pop_int();
				b = pop_int();
				push_int(a || b);
				break;
			case opcode::and_:
				a = pop_int();
				b = pop_int();
				push_int(a && b);
				break;
			case opcode::nop:
				// Do nothing
				break;
			case opcode::rsr:
				return false;
			case opcode::bl: {
				// Check if the function is overridden and if it is, call the resulting external.
				sym = find_symbol_by_address(instr.address);
				auto cb = _m_function_overrides.find(instr.address);
				if (cb != _m_function_overrides.end()) {
					// Guard against exceptions during external invocation.
					stack_guard guard {this, sym->rtype()};

					push_call(sym);
					cb->second(*this);
					pop_call();

					// The stack is left intact.
					guard.inhibit();
				} else {
					if (sym == nullptr) {
						throw vm_exception {"bl: no symbol found for address " + std::to_string(instr.address)};
					}

					call(sym);
				}

				break;
			}
			case opcode::be: {
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw vm_exception {"be: no external found for index"};
				}

				// Guard against exceptions during external invocation.
				stack_guard guard {this, sym->rtype()};

				auto cb = _m_externals.find(sym);
				if (cb == _m_externals.end()) {
					if (_m_default_external.has_value()) {
						(*_m_default_external)(*this, *sym);
						guard.inhibit();
						break;
					} else {
						throw vm_exception {"be: no external registered for " + sym->name()};
					}
				}

				push_call(sym);
				cb->second(*this);
				pop_call();

				// The stack is left intact.
				guard.inhibit();
				break;
			}
			case opcode::pushi:
				push_int(instr.immediate);
				break;
			case opcode::pushvi:
			case opcode::pushv:
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw vm_exception {"pushv: no symbol found for index"};
				}
				push_reference(sym, 0);
				break;
			case opcode::movi:
			case opcode::movvf: {
				auto [ref, idx, context] = pop_reference();

				if (!ref->is_member() || context != nullptr ||
				    !(_m_flags & execution_flag::vm_allow_null_instance_access)) {
					ref->set_int(pop_int(), idx, context);
				} else if (ref->is_member()) {
					PX_LOGE("vm: accessing member \"{}\" without an instance set", ref->name());
					_m_stack.pop();
				}

				break;
			}
			case opcode::movf: {
				auto [ref, idx, context] = pop_reference();

				if (!ref->is_member() || context != nullptr ||
				    !(_m_flags & execution_flag::vm_allow_null_instance_access)) {
					ref->set_float(pop_float(), idx, context);
				} else if (ref->is_member()) {
					PX_LOGE("vm: accessing member \"{}\" without an instance set", ref->name());
					_m_stack.pop();
				}

				break;
			}
			case opcode::movs: {
				auto [target, target_idx, context] = pop_reference();
				auto source = pop_string();

				if (!target->is_member() || context != nullptr ||
				    !(_m_flags & execution_flag::vm_allow_null_instance_access)) {
					target->set_string(source, target_idx, context);
				} else if (target->is_member()) {
					PX_LOGE("vm: accessing member \"{}\" without an instance set", target->name());
				}

				break;
			}
			case opcode::movss:
				throw vm_exception {"not implemented: movss"};
			case opcode::addmovi: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) + pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case opcode::submovi: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) - pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case opcode::mulmovi: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) * pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case opcode::divmovi: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) / pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case opcode::movvi: {
				auto [target, target_idx, _] = pop_reference();
				target->set_instance(pop_instance());
				break;
			}
			case opcode::b:
				jump(instr.address);
				return true;
			case opcode::bz:
				if (pop_int() == 0) {
					jump(instr.address);
					return true;
				}
				break;
			case opcode::gmovi: {
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw vm_exception {"gmovi: no symbol found for index"};
				}
				_m_instance = sym->get_instance();
				break;
			}
			case opcode::pushvv:
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw vm_exception {"pushvv: no symbol found for index"};
				}

				push_reference(sym, instr.index);
				break;
			}

			_m_pc += instr.size;
		} catch (phoenix::script_error& err) {
			uint32_t prev_pc = _m_pc;

			if (_m_exception_handler) {
				auto strategy = (*_m_exception_handler)(*this, err, instr);

				if (strategy == vm_exception_strategy::fail_) {
					std::cerr << "+++ Error while executing script: " << err.what() << "+++\n\n";
					print_stack_trace();
					throw err;
				} else if (strategy == vm_exception_strategy::return_) {
					return false;
				}
			} else {
				std::cerr << "+++ Error while executing script: " << err.what() << "+++\n\n";
				print_stack_trace();
				throw err;
			}

			if (_m_pc == prev_pc) {
				_m_pc += instr.size;
			}
		}

		return true;
	}

	void vm::push_call(const symbol* sym) {
		_m_call_stack.push({sym, _m_pc, _m_instance});
	}

	void vm::pop_call() {
		const auto& call = _m_call_stack.top();
		_m_pc = call.program_counter;
		_m_instance = call.context;
		_m_call_stack.pop();
	}

	void vm::push_int(std::int32_t value) {
		_m_stack.push({nullptr, false, value});
	}

	void vm::push_reference(symbol* value, std::uint8_t index) {
		_m_stack.push({_m_instance, true, value, index});
	}

	void vm::push_string(const std::string& value) {
		_m_temporary_strings->set_string(value);
		push_reference(_m_temporary_strings);
	}

	void vm::push_float(float value) {
		_m_stack.push({nullptr, false, value});
	}

	void vm::push_instance(std::shared_ptr<instance> value) {
		_m_stack.push({nullptr, false, value});
	}

	std::int32_t vm::pop_int() {
		if (_m_stack.empty()) {
			return 0;
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			auto* sym = std::get<symbol*>(v.value);

			// compatibility: sometimes the context might be zero, but we can't fail so when
			//                the compatibility flag is set, we just return 0
			if (sym->is_member() && v.context == nullptr) {
				if (!(_m_flags & execution_flag::vm_allow_null_instance_access)) {
					throw no_context {sym};
				}

				PX_LOGE("vm: accessing member \"{}\" without an instance set", sym->name());
				return 0;
			}

			return sym->get_int(v.index, v.context);
		} else if (std::holds_alternative<int32_t>(v.value)) {
			return std::get<int32_t>(v.value);
		} else {
			throw vm_exception {"tried to pop_int but frame does not contain a int."};
		}
	}

	float vm::pop_float() {
		if (_m_stack.empty()) {
			return 0.0f;
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			auto* sym = std::get<symbol*>(v.value);

			// compatibility: sometimes the context might be zero, but we can't fail so when
			//                the compatibility flag is set, we just return 0
			if (sym->is_member() && v.context == nullptr) {
				if (!(_m_flags & execution_flag::vm_allow_null_instance_access)) {
					throw no_context {sym};
				}

				PX_LOGE("vm: accessing member \"{}\" without an instance set", sym->name());
				return 0;
			}

			return sym->get_float(v.index, v.context);
		} else if (std::holds_alternative<float>(v.value)) {
			return std::get<float>(v.value);
		} else if (std::holds_alternative<std::int32_t>(v.value)) {
			auto k = std::get<std::int32_t>(v.value);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
			return *(float*) &k;
#pragma GCC diagnostic pop
		} else {
			throw vm_exception {"tried to pop_float but frame does not contain a float."};
		}
	}

	std::tuple<symbol*, std::uint8_t, std::shared_ptr<instance>> vm::pop_reference() {
		if (_m_stack.empty()) {
			throw vm_exception {"popping reference from empty stack"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (!v.reference) {
			throw vm_exception {"tried to pop_reference but frame does not contain a reference."};
		}

		return {std::get<symbol*>(v.value), v.index, v.context};
	}

	std::shared_ptr<instance> vm::pop_instance() {
		if (_m_stack.empty()) {
			throw vm_exception {"popping instance from empty stack"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			return std::get<symbol*>(v.value)->get_instance();
		} else if (std::holds_alternative<std::shared_ptr<instance>>(v.value)) {
			return std::get<std::shared_ptr<instance>>(v.value);
		} else {
			throw vm_exception {"tried to pop_instance but frame does not contain am instance."};
		}
	}

	const std::string& vm::pop_string() {
		static std::string empty {};
		auto [s, i, context] = pop_reference();

		// compatibility: sometimes the context might be zero, but we can't fail so when
		//                the compatibility flag is set, we just return 0
		if (s->is_member() && context == nullptr) {
			if (!(_m_flags & execution_flag::vm_allow_null_instance_access)) {
				throw no_context {s};
			}

			PX_LOGE("vm: accessing member \"{}\" without an instance set", s->name());
			return empty;
		}

		return s->get_string(i, context);
	}

	void vm::jump(std::uint32_t address) {
		if (address > size()) {
			throw vm_exception {"Cannot jump to " + std::to_string(address) + ": illegal address"};
		}

		_m_pc = address;
	}

	void vm::register_default_external(const std::function<void(std::string_view)>& callback) {
		_m_default_external = [this, callback](vm& v, symbol& sym) {
			// pop all parameters from the stack
			auto params = find_parameters_for_function(&sym);
			for (int32_t i = params.size() - 1; i >= 0; --i) {
				auto par = params[i];

				if (par->type() == datatype::integer)
					(void) v.pop_int();
				else if (par->type() == datatype::float_)
					(void) v.pop_float();
				else if (par->type() == datatype::instance || par->type() == datatype::string)
					(void) v.pop_reference();
			}

			if (sym.has_return()) {
				if (sym.rtype() == datatype::float_)
					v.push_float(0.0f);
				else if (sym.rtype() == datatype::integer)
					v.push_int(0);
				else if (sym.rtype() == datatype::string)
					v.push_string("");
				else if (sym.rtype() == datatype::instance)
					v.push_instance(nullptr);
			}

			callback(sym.name());
		};
	}

	void vm::register_exception_handler(
	    const std::function<vm_exception_strategy(vm&, const script_error&, const instruction&)>& callback) {
		_m_exception_handler = callback;
	}

	void vm::print_stack_trace() const {
		auto last_pc = _m_pc;
		std::stack<daedalus_call_stack_frame> callstack {_m_call_stack};
		std::stack<daedalus_stack_frame> stack {_m_stack};

		std::cerr << "\n"
		          << "------- CALL STACK (MOST RECENT CALL FIRST) -------"
		          << "\n";

		while (!callstack.empty()) {
			auto v = callstack.top();
			std::cerr << "in " << v.function->name() << " at 0x" << std::hex << last_pc << std::dec << "\n";

			last_pc = v.program_counter;
			callstack.pop();
		}

		std::cerr << "\n"
		          << "------- STACK (MOST RECENT PUSH FIRST) -------"
		          << "\n";

		int32_t i = 0;
		while (!stack.empty()) {
			auto v = stack.top();

			if (v.reference) {
				auto ref = std::get<symbol*>(v.value);
				std::cerr << i << ": [REFERENCE] " << ref->name() << "[" << (int32_t) v.index << "] = ";

				switch (ref->type()) {
				case datatype::float_:
					std::cerr << ref->get_float(v.index, _m_instance) << "\n";
					break;
				case datatype::integer:
					std::cerr << ref->get_int(v.index, _m_instance) << "\n";
					break;
				case datatype::string:
					std::cerr << "'" << ref->get_string(v.index, _m_instance) << "'\n";
					break;
				case datatype::function: {
					auto index = ref->get_int(v.index, _m_instance);
					auto sym = find_symbol_by_index(index);

					std::cout << "&" << sym->name() << "\n";
					break;
				}
				case datatype::instance: {
					auto& inst = ref->get_instance();
					if (inst != nullptr) {
						std::cerr << "<instance of '" << inst->_m_type->name() << "'>\n";
					} else {
						std::cerr << "NULL\n";
					}
					break;
				}
				default:
					std::cerr << "<invalid stack frame>\n";
				}
			} else {
				if (std::holds_alternative<float>(v.value)) {
					std::cerr << i << ": [IMMEDIATE FLOAT] = " << std::get<float>(v.value) << "\n";
				} else if (std::holds_alternative<int32_t>(v.value)) {
					std::cerr << i << ": [IMMEDIATE INT] = " << std::get<int32_t>(v.value) << "\n";
				} else if (std::holds_alternative<std::shared_ptr<instance>>(v.value)) {
					auto& inst = std::get<std::shared_ptr<instance>>(v.value);
					std::cerr << i << ": [IMMEDIATE INSTANCE] = ";
					if (inst == nullptr) {
						std::cerr << "NULL\n";
					} else {
						std::cerr << "<instance of '" << inst->_m_type->name() << "'>\n";
					}
				}
			}

			i += 1;
			stack.pop();
		}

		std::cerr << "\n";
	}

	illegal_external_definition::illegal_external_definition(const symbol* sym, std::string&& message)
	    : script_error(std::move(message)), sym(sym) {}

	illegal_external_rtype::illegal_external_rtype(const symbol* sym, std::string&& provided)
	    : illegal_external_definition(sym,
	                                  "external " + sym->name() + " has illegal return type '" + provided +
	                                      "', expected '" + DAEDALUS_DATA_TYPE_NAMES[(std::uint32_t) sym->rtype()] +
	                                      "'") {}

	illegal_external_param::illegal_external_param(const symbol* sym, std::string&& provided, std::uint8_t i)
	    : illegal_external_definition(sym,
	                                  "external " + sym->name() + " has illegal parameter type '" + provided +
	                                      "' (no. " + std::to_string(i) + "), expected '" +
	                                      DAEDALUS_DATA_TYPE_NAMES[(std::uint32_t) sym->type()] + "'") {}

	vm_exception_strategy lenient_vm_exception_handler(vm& v, const script_error& exc, const instruction& instr) {
		PX_LOGE("vm: internal exception: {}", exc.what());

		switch (instr.op) {
		case opcode::add:
		case opcode::sub:
		case opcode::mul:
		case opcode::div:
		case opcode::mod:
		case opcode::or_:
		case opcode::andb:
		case opcode::lt:
		case opcode::gt:
		case opcode::orr:
		case opcode::and_:
		case opcode::lsl:
		case opcode::lsr:
		case opcode::lte:
		case opcode::eq:
		case opcode::neq:
		case opcode::gte:
		case opcode::plus:
		case opcode::negate:
		case opcode::not_:
		case opcode::cmpl:
			v.push_int(0);
			break;

		case opcode::addmovi:
		case opcode::submovi:
		case opcode::mulmovi:
		case opcode::divmovi:
		case opcode::movi:
		case opcode::movs:
		case opcode::movss:
		case opcode::movvf:
		case opcode::movf:
		case opcode::movvi:
			// do nothing for now but ideally, this would check the exception and/or remove the offending values
			// from the stack.
			break;
		case opcode::nop:
		case opcode::rsr:
		case opcode::b:
		case opcode::bz:
			// if these fail, something has gone horribly wrong. still ignore the error though.
			break;

		case opcode::bl:
		case opcode::be:
			// ignore the branch
			break;

		case opcode::pushi:
			v.push_int(0);
			break;
		case opcode::pushv:
		case opcode::pushvi:
		case opcode::pushvv:
			// push an int and hope it's the right type!
			v.push_int(0);
			break;

		case opcode::gmovi:
			// do nothing for now but ideally, this would set the `null` instance
			break;
		}

		return vm_exception_strategy::continue_;
	}
} // namespace phoenix
