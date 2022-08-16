// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/daedalus/interpreter.hh>

#include <iostream>
#include <utility>

namespace phoenix::daedalus {
	vm::vm(script&& scr) : script(std::move(scr)) {
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
			case op_add:
				push_int(pop_int() + pop_int());
				break;
			case op_subtract:
				a = pop_int();
				b = pop_int();
				push_int(a - b);
				break;
			case op_multiply:
				push_int(pop_int() * pop_int());
				break;
			case op_divide:
				a = pop_int();
				b = pop_int();
				push_int(a / b);
				break;
			case op_modulo:
				a = pop_int();
				b = pop_int();
				push_int(a % b);
				break;
			case op_bitor:
				push_int(pop_int() | pop_int());
				break;
			case op_bitand:
				push_int(pop_int() & pop_int());
				break;
			case op_less:
				a = pop_int();
				b = pop_int();
				push_int(a < b);
				break;
			case op_greater:
				a = pop_int();
				b = pop_int();
				push_int(a > b);
				break;
			case op_shift_left:
				a = pop_int();
				b = pop_int();
				push_int(a << b);
				break;
			case op_shift_right:
				a = pop_int();
				b = pop_int();
				push_int(a >> b);
				break;
			case op_less_or_equal:
				a = pop_int();
				b = pop_int();
				push_int(a <= b);
				break;
			case op_equal:
				push_int(pop_int() == pop_int());
				break;
			case op_not_equal:
				push_int(pop_int() != pop_int());
				break;
			case op_greater_or_equal:
				a = pop_int();
				b = pop_int();
				push_int(a >= b);
				break;
			case op_plus:
				push_int(+pop_int());
				break;
			case op_minus:
				push_int(-pop_int());
				break;
			case op_not:
				push_int(!pop_int());
				break;
			case op_complement:
				push_int(~pop_int());
				break;
			case op_or:
				a = pop_int();
				b = pop_int();
				push_int(a || b);
				break;
			case op_and:
				a = pop_int();
				b = pop_int();
				push_int(a && b);
				break;
			case op_noop:
				// Do nothing
				break;
			case op_return:
				return false;
			case op_call: {
				// Check if the function is overridden and if it is, call the resulting external.
				auto cb = _m_function_overrides.find(instr.address);
				if (cb != _m_function_overrides.end()) {
					push_call(sym);
					cb->second(*this);
					pop_call();
				} else {
					sym = find_symbol_by_address(instr.address);
					if (sym == nullptr) {
						throw std::runtime_error {"op_call: no symbol found for address " +
						                          std::to_string(instr.address)};
					}

					call(sym);
				}

				break;
			}
			case op_call_external: {
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw std::runtime_error {"op_call_external: no external found for index " +
					                          std::to_string(instr.symbol)};
				}

				auto cb = _m_externals.find(sym);
				if (cb == _m_externals.end()) {
					if (_m_external_error_handler.has_value()) {
						(*_m_external_error_handler)(*this, *sym);
						break;
					} else {
						throw std::runtime_error {"op_call_external: no external registered for " + sym->name()};
					}
				}

				push_call(sym);
				cb->second(*this);
				pop_call();
				break;
			}
			case op_push_int:
				push_int(instr.immediate);
				break;
			case op_push_instance:
			case op_push_var:
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw std::runtime_error {"op_push_var: no symbol found for index " + std::to_string(instr.symbol)};
				}
				push_reference(sym, 0);
				break;
			case op_assign_int:
			case op_assign_func: {
				auto [ref, idx, context] = pop_reference();
				ref->set_int(pop_int(), idx, context);
				break;
			}
			case op_assign_float: {
				auto [ref, idx, context] = pop_reference();
				ref->set_float(pop_float(), idx, context);
				break;
			}
			case op_assign_string: {
				auto [target, target_idx, context] = pop_reference();
				auto source = pop_string();
				target->set_string(source, target_idx, context);
				break;
			}
			case op_assign_stringref:
				throw std::runtime_error {"not implemented: op_assign_stringref"};
			case op_assign_add: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) + pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case op_assign_subtract: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) - pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case op_assign_multiply: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) * pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case op_assign_divide: {
				auto [ref, idx, context] = pop_reference();
				auto result = ref->get_int(idx, context) / pop_int();
				ref->set_int(result, idx, context);
				break;
			}
			case op_assign_instance: {
				auto [target, target_idx, _] = pop_reference();
				target->set_instance(pop_instance());
				break;
			}
			case op_jump:
				jump(instr.address);
				return true;
			case op_jump_if_zero:
				if (pop_int() == 0) {
					jump(instr.address);
					return true;
				}
				break;
			case op_set_instance: {
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw std::runtime_error {"op_set_instance: no symbol found for index " +
					                          std::to_string(instr.symbol)};
				}
				_m_instance = sym->get_instance();
				break;
			}
			case op_push_array_var:
				sym = find_symbol_by_index(instr.symbol);
				if (sym == nullptr) {
					throw std::runtime_error {"op_push_array_var: no symbol found for index " +
					                          std::to_string(instr.symbol)};
				}

				push_reference(sym, instr.index);
				break;
			}
		} catch (const std::runtime_error& err) {
			std::cerr << "+++ Error while executing script: " << err.what() << "+++\n\n";
			print_stack_trace();
			throw std::domain_error {std::string {err.what()}};
		}

		_m_pc += instr.size;
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
			// std::cerr << "WARN: popping 0 from empty stack!\n";
			return 0;
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			return std::get<symbol*>(v.value)->get_int(v.index, v.context);
		} else if (std::holds_alternative<int32_t>(v.value)) {
			return std::get<int32_t>(v.value);
		} else {
			throw std::runtime_error {"Tried to pop_int but frame does not contain an int."};
		}
	}

	float vm::pop_float() {
		if (_m_stack.empty()) {
			throw std::runtime_error {"Popping from empty stack!"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			return std::get<symbol*>(v.value)->get_float(v.index, v.context);
		} else if (std::holds_alternative<float>(v.value)) {
			return std::get<float>(v.value);
		} else if (std::holds_alternative<std::int32_t>(v.value)) {
			// std::cerr << "WARN: Popping int and reinterpreting as float\n";
			auto k = std::get<std::int32_t>(v.value);
			return std::bit_cast<float>(k);
		} else {
			throw std::runtime_error {"Tried to pop_float but frame does not contain an float."};
		}
	}

	std::tuple<symbol*, std::uint8_t, std::shared_ptr<instance>> vm::pop_reference() {
		if (_m_stack.empty()) {
			throw std::runtime_error {"Popping from empty stack!"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (!v.reference) {
			throw std::runtime_error {"Tried to pop_reference but frame does not contain a reference."};
		}

		return {std::get<symbol*>(v.value), v.index, v.context};
	}

	std::shared_ptr<instance> vm::pop_instance() {
		if (_m_stack.empty()) {
			throw std::runtime_error {"Popping from empty stack!"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			return std::get<symbol*>(v.value)->get_instance();
		} else if (std::holds_alternative<std::shared_ptr<instance>>(v.value)) {
			return std::get<std::shared_ptr<instance>>(v.value);
		} else {
			throw std::runtime_error {"Tried to pop_float but frame does not contain an float."};
		}
	}

	const std::string& vm::pop_string() {
		auto [s, i, context] = pop_reference();
		return s->get_string(i, context);
	}

	void vm::jump(std::uint32_t address) {
		if (address > size()) {
			throw std::runtime_error {"Cannot jump to " + std::to_string(address) + ": illegal address"};
		}
		_m_pc = address;
	}

	void vm::register_default_external(const std::function<void(std::string_view)>& callback) {
		_m_external_error_handler = [this, callback](vm& v, symbol& sym) {
			// pop all parameters from the stack
			auto params = find_parameters_for_function(&sym);
			for (int i = params.size() - 1; i >= 0; --i) {
				auto par = params[i];

				if (par->type() == dt_integer)
					(void) v.pop_int();
				else if (par->type() == dt_float)
					(void) v.pop_float();
				else if (par->type() == dt_instance || par->type() == dt_string)
					(void) v.pop_reference();
			}

			if (sym.has_return()) {
				if (sym.rtype() == dt_float)
					v.push_float(0.0f);
				else if (sym.rtype() == dt_integer)
					v.push_int(0);
				else if (sym.rtype() == dt_string)
					v.push_string("");

				// TODO: We can't really push an instance since we'd need to take the type into account. Sadly,
				//       compiled scripts don't store the type of instance being returned so pushing one would
				//       most likely just result in the VM panicking anyways.
			}

			callback(sym.name());
		};
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

		int i = 0;
		while (!stack.empty()) {
			auto v = stack.top();

			if (v.reference) {
				auto ref = std::get<symbol*>(v.value);
				std::cerr << i << ": [REFERENCE] " << ref->name() << "[" << (int) v.index << "] = ";

				switch (ref->type()) {
				case dt_float:
					std::cerr << ref->get_float(v.index, _m_instance) << "\n";
					break;
				case dt_integer:
					std::cerr << ref->get_int(v.index, _m_instance) << "\n";
					break;
				case dt_string:
					std::cerr << "'" << ref->get_string(v.index, _m_instance) << "'\n";
					break;
				case dt_function: {
					auto index = ref->get_int(v.index, _m_instance);
					auto sym = find_symbol_by_index(index);

					std::cout << "&" << sym->name() << "\n";
					break;
				}
				case dt_instance: {
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

	illegal_external_rtype::illegal_external_rtype(const symbol* sym, std::string_view provided)
	    : illegal_external("external " + sym->name() + " has illegal return type '" + provided.data() +
	                       "', expected '" + DAEDALUS_DATA_TYPE_NAMES[sym->rtype()] + "'") {}

	illegal_external_param::illegal_external_param(const symbol* sym, std::string_view provided, std::uint8_t i)
	    : illegal_external("external " + sym->name() + " has illegal parameter type '" + provided.data() + "' (no. " +
	                       std::to_string(i) + "), expected '" + DAEDALUS_DATA_TYPE_NAMES[sym->type()] + "'") {}
} // namespace phoenix::daedalus
