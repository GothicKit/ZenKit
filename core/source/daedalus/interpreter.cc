// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/daedalus/interpreter.hh>

#include <iostream>
#include <utility>

namespace phoenix {
	daedalus_interpreter::daedalus_interpreter(script& scr) : _m_script(scr) {
		_m_self_sym = scr.find_symbol_by_name("SELF");
	}

	void daedalus_interpreter::call_function(const std::string& name) {
		call_function(_m_script.find_symbol_by_name(name));
	}

	void daedalus_interpreter::call_function(const symbol* sym) {
		if (sym == nullptr) { throw std::runtime_error {"Cannot call function: not found"}; }
		if (sym->type() != dt_function) { throw std::runtime_error {"Cannot call " + sym->name() + ": not a function"}; }

		call(sym);

		if (sym->has_return()) {
			_m_stack.pop();
		}
	}

	void daedalus_interpreter::call(const symbol* sym) {
		push_call(sym);
		jump(sym->address());

		// execute until an op_return is reached
		while (exec())
			;

		pop_call();
	}

	bool daedalus_interpreter::exec() {
		auto instr = _m_script.instruction_at(_m_pc);

		s32 a {}, b {};
		symbol* sym {};

		try {

			switch (instr.op) {
				case op_add:
					push_int(pop_int() + pop_int());
					break;
				case op_subtract:
					push_int(pop_int() - pop_int());
					break;
				case op_multiply:
					push_int(pop_int() * pop_int());
					break;
				case op_divide:
					push_int(pop_int() / pop_int());
					break;
				case op_modulo:
					push_int(pop_int() % pop_int());
					break;
				case op_bitor:
					push_int(pop_int() | pop_int());
					break;
				case op_bitand:
					push_int(pop_int() & pop_int());
					break;
				case op_less:
					push_int(pop_int() < pop_int());
					break;
				case op_greater:
					push_int(pop_int() > pop_int());
					break;
				case op_shift_left:
					push_int(pop_int() << pop_int());
					break;
				case op_shift_right:
					push_int(pop_int() >> pop_int());
					break;
				case op_less_or_equal:
					push_int(pop_int() <= pop_int());
					break;
				case op_equal:
					push_int(pop_int() == pop_int());
					break;
				case op_not_equal:
					push_int(pop_int() != pop_int());
					break;
				case op_greater_or_equal:
					push_int(pop_int() >= pop_int());
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
				case op_call:
					sym = _m_script.find_symbol_by_address(instr.address);
					if (sym == nullptr) { throw std::runtime_error {"op_call: no symbol found for address " + std::to_string(instr.address)}; }

					call(sym);
					break;
				case op_call_external: {
					sym = _m_script.find_symbol_by_index(instr.symbol);
					if (sym == nullptr) { throw std::runtime_error {"op_call_external: no external found for index " + std::to_string(instr.symbol)}; }

					auto cb = _m_externals.find(sym);
					if (cb == _m_externals.end()) {
						throw std::runtime_error {"op_call_external: no external registered for " + sym->name()};
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
					sym = _m_script.find_symbol_by_index(instr.symbol);
					if (sym == nullptr) { throw std::runtime_error {"op_push_var: no symbol found for index " + std::to_string(instr.symbol)}; }
					push_reference(sym, 0);
					break;
				case op_assign_int:
				case op_assign_func: {
					auto [ref, idx] = pop_reference();
					ref->set_int(pop_int(), idx, _m_instance);
					break;
				}
				case op_assign_float: {
					auto [ref, idx] = pop_reference();
					ref->set_float(pop_float(), idx, _m_instance);
					break;
				}
				case op_assign_string: {
					auto [target, target_idx] = pop_reference();
					auto [source, source_idx] = pop_reference();
					target->set_string(source->get_string(source_idx, _m_instance), target_idx, _m_instance);
					break;
				}
				case op_assign_stringref:
					throw std::runtime_error {"not implemented: op_assign_stringref"};
				case op_assign_add: {
					auto [ref, idx] = pop_reference();
					auto result = ref->get_int(idx, _m_instance) + pop_int();
					ref->set_int(result, idx, _m_instance);
					break;
				}
				case op_assign_subtract: {
					auto [ref, idx] = pop_reference();
					auto result = ref->get_int(idx, _m_instance) - pop_int();
					ref->set_int(result, idx, _m_instance);
					break;
				}
				case op_assign_multiply: {
					auto [ref, idx] = pop_reference();
					auto result = ref->get_int(idx, _m_instance) * pop_int();
					ref->set_int(result, idx, _m_instance);
					break;
				}
				case op_assign_divide: {
					auto [ref, idx] = pop_reference();
					auto result = ref->get_int(idx, _m_instance) / pop_int();
					ref->set_int(result, idx, _m_instance);
					break;
				}
				case op_assign_instance: {
					auto [target, target_idx] = pop_reference();
					target->set_instance(pop_instance());
					break;
				}
				case op_jump:
					jump(instr.address);
					return true;
				case op_jump_if_zero:
					if (pop_int() == 0) {
						jump(instr.address);
					}
					return true;
				case op_set_instance: {
					sym = _m_script.find_symbol_by_index(instr.symbol);
					if (sym == nullptr) { throw std::runtime_error {"op_set_instance: no symbol found for index " + std::to_string(instr.symbol)}; }
					_m_instance = sym->get_instance();
					break;
				}
				case op_push_array_var:
					sym = _m_script.find_symbol_by_index(instr.symbol);
					if (sym == nullptr) { throw std::runtime_error {"op_push_array_var: no symbol found for index " + std::to_string(instr.symbol)}; }

					push_reference(sym, instr.index);
					break;
			}
		} catch (const std::runtime_error& err) {
			print_stack_trace();
			throw std::domain_error {std::string {err.what()}};
		}

		_m_pc += instr.size;
		return true;
	}

	void daedalus_interpreter::push_call(const symbol* sym) {
		_m_call_stack.push({sym, _m_pc, _m_dynamic_string_index});
	}

	void daedalus_interpreter::pop_call() {
		const auto& call = _m_call_stack.top();
		_m_pc = call.program_counter;

		if (call.function->has_return() && call.function->rtype() != dt_string) {
			_m_dynamic_string_index = call.dynamic_string_index;
		}

		_m_call_stack.pop();
	}

	void daedalus_interpreter::push_int(s32 value) {
		_m_stack.push({false, value});
	}

	void daedalus_interpreter::push_reference(symbol* value, u8 index) {
		_m_stack.push({true, value, index});
	}

	void daedalus_interpreter::push_string(const std::string& value) {
		if (_m_dynamic_string_index >= _m_script.dynamic_strings().count()) { throw std::runtime_error {"Cannot allocate dynamic string object: out of slots"}; }
		auto& sym = _m_script.dynamic_strings();
		sym.set_string(value, _m_dynamic_string_index);
		push_reference(&sym, _m_dynamic_string_index);

		_m_dynamic_string_index++;
	}

	void daedalus_interpreter::push_float(f32 value) {
		_m_stack.push({false, value});
	}

	void daedalus_interpreter::push_instance(std::shared_ptr<instance> value) {
		_m_stack.push({false, value});
	}

	s32 daedalus_interpreter::pop_int() {
		if (_m_stack.empty()) {
			// throw std::runtime_error {"Popping from empty stack!"};
			std::cerr << "WARN: popping 0 from empty stack!\n";
			return 0;
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			return std::get<symbol*>(v.value)->get_int(v.index, _m_instance);
		} else if (std::holds_alternative<int32_t>(v.value)) {
			return std::get<int32_t>(v.value);
		} else {
			throw std::runtime_error {"Tried to pop_int but frame does not contain an int."};
		}
	}

	f32 daedalus_interpreter::pop_float() {
		if (_m_stack.empty()) {
			throw std::runtime_error {"Popping from empty stack!"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (v.reference) {
			return std::get<symbol*>(v.value)->get_float(v.index, _m_instance);
		} else if (std::holds_alternative<float>(v.value)) {
			return std::get<float>(v.value);
		} else if (std::holds_alternative<s32>(v.value)) {
			std::cerr << "WARN: Popping int and reinterpreting as float\n";
			auto k = std::get<s32>(v.value);
			return reinterpret_cast<float&>(k);
		} else {
			throw std::runtime_error {"Tried to pop_float but frame does not contain an float."};
		}
	}

	std::tuple<symbol*, u8> daedalus_interpreter::pop_reference() {
		if (_m_stack.empty()) {
			throw std::runtime_error {"Popping from empty stack!"};
		}

		auto v = _m_stack.top();
		_m_stack.pop();

		if (!v.reference) {
			throw std::runtime_error {"Tried to pop_reference but frame does not contain a reference."};
		}

		return {std::get<symbol*>(v.value), v.index};
	}

	std::shared_ptr<instance> daedalus_interpreter::pop_instance() {
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

	const std::string& daedalus_interpreter::pop_string() {
		auto [s, i] = pop_reference();
		return s->get_string(i, _m_instance);
	}

	void daedalus_interpreter::jump(u32 address) {
		if (address == 0 || address > _m_script.size()) { throw std::runtime_error {"Cannot jump to " + std::to_string(address) + ": illegal address"}; }
		_m_pc = address;
	}

	void daedalus_interpreter::print_stack_trace() {
		auto last_pc = _m_pc;

		std::cerr << "\n"
				  << "------- CALL STACK (MOST RECENT CALL FIRST) -------"
				  << "\n";

		while (!_m_call_stack.empty()) {
			auto v = _m_call_stack.top();
			std::cerr << "in " << v.function->name() << " at " << last_pc << "\n";

			last_pc = v.program_counter;
			_m_call_stack.pop();
		}

		std::cerr << "\n"
				  << "------- STACK (MOST RECENT PUSH FIRST) -------"
				  << "\n";

		int i = 0;
		while (!_m_stack.empty()) {
			auto v = _m_stack.top();

			if (v.reference) {
				auto ref = std::get<symbol*>(v.value);
				std::cerr << i << ":  "
						  << "[REFERENCE] " << ref->name() << "[" << (int) v.index << "] = ";

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
						auto sym = _m_script.find_symbol_by_index(index);

						std::cout << "(func) " << sym->name() << "\n";
						break;
					}
					case dt_instance: {
						// auto inst = v.reference_value->get_instance();
						std::cerr << "(instance)\n";
						break;
					}
					default:
						std::cerr << "<UNKNOWN>\n";
				}
			} else {
				if (std::holds_alternative<float>(v.value)) {
					std::cerr << i << ":  "
							  << "[IMMEDIATE FLOAT] " << std::get<float>(v.value) << "\n";
				} else if (std::holds_alternative<int32_t>(v.value)) {
					std::cerr << i << ":  "
							  << "[IMMEDIATE INT] " << std::get<int32_t>(v.value) << "\n";
				} else if (std::holds_alternative<std::shared_ptr<instance>>(v.value)) {
					std::cerr << i << ":  "
							  << "[IMMEDIATE INSTANCE] " << std::get<std::shared_ptr<instance>>(v.value).get() << "\n";
				}
			}

			i += 1;
			_m_stack.pop();
		}

		std::cerr << "\n";
	}

	illegal_external_rtype::illegal_external_rtype(const symbol* sym, std::string_view provided)
		: illegal_external("external " + sym->name() + " has illegal return type '" + provided.data() +
						   "', expected '" + DAEDALUS_DATA_TYPE_NAMES[sym->rtype()] + "'") {
	}

	illegal_external_param::illegal_external_param(const symbol* sym, std::string_view provided, u8 i)
			: illegal_external("external " + sym->name() + " has illegal parameter type '" + provided.data() +
							 "' (no. " + std::to_string(i) + "), expected '" + DAEDALUS_DATA_TYPE_NAMES[sym->type()] + "'"){
	}
}// namespace phoenix
