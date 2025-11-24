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
		/// \brief Creates a new stack guard.
		/// \param machine The VM this instance is guarding.
		/// \param type The type of value to push if the guard is triggered.
		StackGuard(DaedalusVm* machine, DaedalusDataType type) : _m_type(type), _m_machine(machine) {}

		/// \brief Triggers this guard.
		///
		/// Unless #inhibit was called, this destructor will push a value of the datatype passed in the
		/// constructor onto the stack of the VM passed in the constructor.
		~StackGuard() {
			if (_m_inhibited) return;

			StackGuard::fix(_m_machine, _m_type);
		}

		/// \brief Inhibits this guard.
		///
		/// Calling this function will cause the guard to disengage and thus not push a
		/// value onto the stack.
		void inhibit() {
			_m_inhibited = true;
		}

		static void fix(DaedalusVm* vm, DaedalusDataType type) {
			switch (type) {
			case DaedalusDataType::FLOAT:
				vm->push_float(0);
				break;
			case DaedalusDataType::INT:
			case DaedalusDataType::FUNCTION:
				vm->push_int(0);
				break;
			case DaedalusDataType::STRING:
				vm->push_string("");
				break;
			case DaedalusDataType::INSTANCE:
				vm->push_instance(nullptr);
				break;
			case DaedalusDataType::VOID:
			case DaedalusDataType::CLASS:
			case DaedalusDataType::PROTOTYPE:
				break;
			}
		}

	private:
		DaedalusDataType _m_type;
		DaedalusVm* _m_machine;
		bool _m_inhibited {false};
	};

	DaedalusVm::DaedalusVm(DaedalusScript&& scr, std::uint8_t flags) : DaedalusScript(std::move(scr)), _m_flags(flags) {
		_m_temporary_strings = add_temporary_strings_symbol();
		_m_temporary_strings_free.push(0);

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

	void DaedalusVm::unsafe_call(DaedalusSymbol const* sym) {
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

	std::shared_ptr<DaedalusInstance> DaedalusVm::unsafe_get_gi() {
		return _m_instance;
	}

	void DaedalusVm::unsafe_set_gi(std::shared_ptr<DaedalusInstance> i) {
		_m_instance = std::move(i);
	}

	bool DaedalusVm::exec() {
		auto instr = instruction_at(_m_pc);

		try {
			std::int32_t a, b;
			DaedalusSymbol* sym;

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

				if (b == 0) throw DaedalusVmException {"vm: division by zero"};

				push_int(a / b);
				break;
			case DaedalusOpcode::MOD:
				a = pop_int();
				b = pop_int();

				if (b == 0) throw DaedalusVmException {"vm: division by zero"};

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
				if (auto cb = _m_function_overrides.find(instr.address); cb != _m_function_overrides.end()) {
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
					}

					throw DaedalusVmException {"be: no external registered for " + sym->name()};
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
					auto result = ref->get_int(idx, context.get()) + value;
					ref->set_int(result, idx, context.get());
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
					auto result = ref->get_int(idx, context.get()) - value;
					ref->set_int(result, idx, context.get());
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
					auto result = ref->get_int(idx, context.get()) * value;
					ref->set_int(result, idx, context.get());
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
					auto result = ref->get_int(idx, context.get()) / value;
					ref->set_int(result, idx, context.get());
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
		} catch (DaedalusScriptError& err) {
			uint32_t prev_pc = _m_pc;

			if (_m_exception_handler) {
				auto strategy = (*_m_exception_handler)(*this, err, instr);

				if (strategy == DaedalusVmExceptionStrategy::FAIL) {
					ZKLOGE("DaedalusVm", "+++ Error while executing script: %s +++", err.what());
					print_stack_trace();
					throw;
				}

				if (strategy == DaedalusVmExceptionStrategy::RETURN) {
					return false;
				}
			} else {
				ZKLOGE("DaedalusVm", "+++ Error while executing script: %s +++", err.what());
				print_stack_trace();
				throw;
			}

			if (_m_pc == prev_pc) {
				_m_pc += instr.size;
			}
		}

		return true;
	}

	void DaedalusVm::push_call(DaedalusSymbol const* sym) {
		if(sym->has_local_variables_enabled()) {
			push_local_variables(sym);
		}
		auto var_count = this->find_parameters_for_function(sym).size();
		_m_call_stack.push_back({sym, _m_pc, _m_stack_ptr - static_cast<uint32_t>(var_count), _m_instance});
	}

	void DaedalusVm::pop_call() {
		auto const& call = _m_call_stack.back();

		// First, try to fix up the stack.
		if (!call.function->has_return()) {
			// No special logic needed, there are supposed to be no more stack frames for
			// this function, so just reset the stack for the caller.
			_m_stack_ptr = call.stack_ptr;
		} else {
			auto remaining_locals = _m_stack_ptr - call.stack_ptr;
			if (remaining_locals == 0) {
				// The function is supposed to have a return value, but it does not seem to have one.
				// To fix this, we just insert a default value (either 0, "" or NULL).
				StackGuard::fix(this, call.function->rtype());
			} else if (remaining_locals > 1) {
				// Now we have too many items left on the stack. Remove all of them, except the topmost one,
				// since that one is supposed to be the return value of the function.
				DaedalusStackFrame frame = _m_stack[--_m_stack_ptr];
				_m_stack_ptr = call.stack_ptr;
				_m_stack[_m_stack_ptr++] = std::move(frame);
			}
			// else {
			//     We have exactly one value to be returned (as indicated by the symbol's return type).
			//     That means, that the compiler did not mess up the stack management, so we can just
			//     return that value.
			// }
		}

		if(call.function->has_local_variables_enabled()) {
			pop_local_variables(call.function);
		}

		// Second, reset PC and context, then remove the call stack frame
		_m_pc = call.program_counter;
		_m_instance = call.context;
		_m_call_stack.pop_back();
	}

	void DaedalusVm::push_local_variables(DaedalusSymbol const* sym) {
		bool has_recursion = false;
		for(auto& i:_m_call_stack)
			if(i.function==sym) {
				has_recursion = true;
				break;
			}
		if(!has_recursion)
			return;

		auto params = this->find_parameters_for_function(sym);
		auto locals = this->find_locals_for_function(sym);

		if (_m_stack_ptr+locals.size() > stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		if (_m_stack_ptr<params.size()) {
			throw DaedalusVmException {"stack underoverflow"};
		}

		// move function arguments futher
		_m_stack_ptr -= params.size();
		for(size_t i=0; i<params.size(); ++i) {
			_m_stack[_m_stack_ptr+locals.size()+i] = std::move(_m_stack[_m_stack_ptr+i]);
		}

		for(auto& l:locals) {
			switch (l.type()) {
			case DaedalusDataType::VOID:
				break;
			case DaedalusDataType::FLOAT:
				for(std::uint32_t i=0; i<l.count(); ++i) {
					push_float(l.get_float(i));
				}
				break;
			case DaedalusDataType::FUNCTION:
			case DaedalusDataType::INT:
				for(std::uint32_t i=0; i<l.count(); ++i) {
					push_int(l.get_int(i));
				}
				break;
			case DaedalusDataType::STRING:
				for(std::uint32_t i=0; i<l.count(); ++i) {
					push_string(l.get_string(i));
				}
				break;
			case DaedalusDataType::INSTANCE:
				push_instance(l.get_instance());
				break;
			case DaedalusDataType::CLASS:
			case DaedalusDataType::PROTOTYPE:
				throw DaedalusVmException {"unexpected"};
				break;
			}
		}
		_m_stack_ptr += params.size();
	}

	void DaedalusVm::pop_local_variables(DaedalusSymbol const* sym) {
		int has_recursion = 0;
		for(auto& i:_m_call_stack)
			if(i.function==sym) {
				++has_recursion;
			}
		if(has_recursion<=1)
			return;

		DaedalusStackFrame ret;
		if(sym->has_return()) {
			ret = std::move(_m_stack[--_m_stack_ptr]);
		}

		auto locals = this->find_locals_for_function(sym);
		for(size_t i=locals.size(); i>0;) {
			--i;
			auto& l = locals[i];
			switch (l.type()) {
			case DaedalusDataType::VOID:
				break;
			case DaedalusDataType::FLOAT:
				for(std::uint32_t r=l.count(); r>0; ) {
					--r;
					l.set_float(pop_float(), r);
				}
				break;
			case DaedalusDataType::FUNCTION:
			case DaedalusDataType::INT:
				for(std::uint32_t r=l.count(); r>0; ) {
					--r;
					l.set_int(pop_int(), r);
				}
				break;
			case DaedalusDataType::STRING:
				for(std::uint32_t r=l.count(); r>0; ) {
					--r;
					l.set_string(pop_string(), r);
				}
				break;
			case DaedalusDataType::INSTANCE:
				l.set_instance(pop_instance());
			break;
			case DaedalusDataType::CLASS:
			case DaedalusDataType::PROTOTYPE:
				throw DaedalusVmException {"unexpected"};
				break;
			}
		}

		if(sym->has_return()) {
			_m_stack[_m_stack_ptr++] = std::move(ret);
		}
	}

	void DaedalusVm::push_int(std::int32_t value) {
		if (_m_stack_ptr == stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {nullptr, false, value};
	}

	void DaedalusVm::push_reference(DaedalusSymbol* value, std::uint8_t index) {
		if (_m_stack_ptr == stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {_m_instance, true, value, index};
	}

	void DaedalusVm::push_string(std::string_view value) {
		// We need to push strings without an explicitly attached variable to a
		// virtual string container variable.
		auto index = this->get_free_string();

		_m_temporary_strings->set_string(value, index);
		push_reference(_m_temporary_strings, index);
	}

	void DaedalusVm::push_float(float value) {
		if (_m_stack_ptr == stack_size) {
			throw DaedalusVmException {"stack overflow"};
		}

		_m_stack[_m_stack_ptr++] = {nullptr, false, value};
	}

	void DaedalusVm::push_instance(std::shared_ptr<DaedalusInstance> value) {
		if (_m_stack_ptr == stack_size) {
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
		}

		if (std::holds_alternative<int32_t>(v.value)) {
			return std::get<int32_t>(v.value);
		}

		throw DaedalusVmException {"tried to pop_int but frame does not contain a int."};
	}

	float DaedalusVm::pop_float() {
		if (_m_stack_ptr == 0) {
			return 0.0f;
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (v.reference) {
			return this->get_float(v.context, v.value, v.index);
		}

		if (std::holds_alternative<float>(v.value)) {
			return std::get<float>(v.value);
		}

		if (std::holds_alternative<std::int32_t>(v.value)) {
			auto k = std::get<std::int32_t>(v.value);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
			return *reinterpret_cast<float*>(&k);
#pragma GCC diagnostic pop
		}

		throw DaedalusVmException {"tried to pop_float but frame does not contain a float."};
	}

	std::tuple<DaedalusSymbol*, std::uint8_t, std::shared_ptr<DaedalusInstance>> DaedalusVm::pop_reference() {
		if (_m_stack_ptr == 0) {
			throw DaedalusVmException {"popping reference from empty stack"};
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (!v.reference) {
			throw DaedalusVmException {"tried to pop_reference but frame does not contain a reference."};
		}

		auto sym = std::get<DaedalusSymbol*>(v.value);
		if (sym->index() == _m_temporary_strings->index()) {
			// For virtual strings, we'll mark this string as free when it's popped.
			// This is likely unreliable but until it causes issues, it'll stay as-is.
			this->mark_free_string(v.index);
		}

		return {std::get<DaedalusSymbol*>(v.value), v.index, v.context};
	}

	bool DaedalusVm::top_is_reference() const {
		if (_m_stack_ptr == 0) {
			throw DaedalusVmException {"popping from empty stack"};
		}

		return _m_stack[_m_stack_ptr - 1].reference;
	}

	std::shared_ptr<DaedalusInstance> DaedalusVm::pop_instance() {
		if (_m_stack_ptr == 0) {
			throw DaedalusVmException {"popping instance from empty stack"};
		}

		DaedalusStackFrame v = std::move(_m_stack[--_m_stack_ptr]);

		if (v.reference) {
			return std::get<DaedalusSymbol*>(v.value)->get_instance();
		}

		if (std::holds_alternative<std::shared_ptr<DaedalusInstance>>(v.value)) {
			return std::get<std::shared_ptr<DaedalusInstance>>(v.value);
		}

		throw DaedalusVmException {"tried to pop_instance but frame does not contain am instance."};
	}

	std::string const& DaedalusVm::pop_string() {
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

		return s->get_string(i, context.get());
	}

	void DaedalusVm::jump(std::uint32_t address) {
		if (address >= size()) {
			throw DaedalusVmException {"Cannot jump to " + std::to_string(address) + ": illegal address"};
		}

		_m_pc = address;
	}

	void DaedalusVm::register_default_external(std::function<void(std::string_view)> const& callback) {
		this->register_default_external([callback](DaedalusSymbol const& sym) { callback(sym.name()); });
	}

	void DaedalusVm::register_default_external(std::function<void(DaedalusSymbol const&)> const& callback) {
		_m_default_external = [this, callback](DaedalusVm& v, DaedalusSymbol const& sym) {
			// pop all parameters from the stack
			auto params = find_parameters_for_function(&sym);
			for (int i = static_cast<int>(params.size()) - 1; i >= 0; --i) {
				auto& par = params[static_cast<unsigned>(i)];
				if (par.type() == DaedalusDataType::INT)
					(void) v.pop_int();
				else if (par.type() == DaedalusDataType::FLOAT)
					(void) v.pop_float();
				else if (par.type() == DaedalusDataType::INSTANCE || par.type() == DaedalusDataType::STRING)
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

			callback(sym);
		};
	}

	void
	DaedalusVm::register_default_external_custom(std::function<void(DaedalusVm&, DaedalusSymbol&)> const& callback) {
		_m_default_external = callback;
	}

	void DaedalusVm::register_access_trap(std::function<void(DaedalusSymbol&)> const& callback) {
		_m_access_trap = callback;
	}

	void DaedalusVm::register_exception_handler(
	    std::function<DaedalusVmExceptionStrategy(DaedalusVm&,
	                                              DaedalusScriptError const&,
	                                              DaedalusInstruction const&)> const& callback) {
		_m_exception_handler = callback;
	}

	void DaedalusVm::print_stack_trace() const {
		auto last_pc = _m_pc;
		auto tmp_stack_ptr = _m_stack_ptr;

		std::stack callstack {_m_call_stack};

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
					value = std::to_string(ref->get_float(v.index, _m_instance.get()));
					break;
				case DaedalusDataType::INT:
					value = std::to_string(ref->get_int(v.index, _m_instance.get()));
					break;
				case DaedalusDataType::STRING:
					value = "'" + ref->get_string(v.index, _m_instance.get()) + "'";
					break;
				case DaedalusDataType::FUNCTION: {
					auto index = ref->get_int(v.index, _m_instance.get());
					auto sym = find_symbol_by_index(static_cast<uint32_t>(index));
					value = "&" + sym->name();
					break;
				}
				case DaedalusDataType::INSTANCE: {
					if (auto& inst = ref->get_instance(); inst != nullptr) {
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
					if (auto& inst = std::get<std::shared_ptr<DaedalusInstance>>(v.value); inst == nullptr) {
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

		ZKLOGE("DaedalusVm", "------- GLOBAL VARIABLES -------");

		DaedalusSymbol* symbols[] = {
		    _m_hero_sym,
		    _m_self_sym,
		    _m_other_sym,
		    _m_victim_sym,
		    _m_item_sym,
		};

		for (auto* sym : symbols) {
			if (sym == nullptr) continue;

			auto instance = sym->get_instance();
			if (instance != nullptr && instance->symbol_index() != static_cast<uint32_t>(-1)) {
				DaedalusSymbol const* instance_symbol = find_symbol_by_instance(instance);
				ZKLOGE("DaedalusVm",
				       "%s = %s (%u)",
				       sym->name().c_str(),
				       instance_symbol->name().c_str(),
				       instance->symbol_index());
			} else if (instance == nullptr) {
				ZKLOGE("DaedalusVm", "%s = NULL", sym->name().c_str());
			} else {
				ZKLOGE("DaedalusVm", "%s = <TRANSIENT>", sym->name().c_str());
			}
		}

		if (_m_instance != nullptr && _m_instance->symbol_index() != static_cast<uint32_t>(-1)) {
			auto instance_symbol = find_symbol_by_instance(_m_instance);
			ZKLOGE("DaedalusVm", "<__THIS__> = %s (%u)", instance_symbol->name().c_str(), _m_instance->symbol_index());
		} else if (_m_instance == nullptr) {
			ZKLOGE("DaedalusVm", "<__THIS__> = NULL");
		} else {
			ZKLOGE("DaedalusVm", "<__THIS__> = <TRANSIENT>");
		}
	}

	DaedalusIllegalExternalDefinition::DaedalusIllegalExternalDefinition(DaedalusSymbol const* s, std::string&& msg)
	    : DaedalusScriptError(std::move(msg)), sym(s) {}

	DaedalusIllegalExternalReturnType::DaedalusIllegalExternalReturnType(DaedalusSymbol const* s,
	                                                                     std::string&& provided)
	    : DaedalusIllegalExternalDefinition(
	          s,
	          "external " + s->name() + " has illegal return type '" + provided + "', expected '" +
	              DAEDALUS_DATA_TYPE_NAMES[static_cast<std::uint32_t>(s->rtype())] + "'") {}

	DaedalusIllegalExternalParameter::DaedalusIllegalExternalParameter(DaedalusSymbol const* s,
	                                                                   std::string&& provided,
	                                                                   std::uint8_t i)
	    : DaedalusIllegalExternalDefinition(s,
	                                        "external " + s->name() + " has illegal parameter type '" + provided +
	                                            "' (no. " + std::to_string(i) + "), expected '" +
	                                            DAEDALUS_DATA_TYPE_NAMES[static_cast<std::uint32_t>(s->type())] + "'") {
	}

	DaedalusVmExceptionStrategy
	lenient_vm_exception_handler(DaedalusVm& v, DaedalusScriptError const& exc, DaedalusInstruction const& instr) {
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
	DaedalusVm::get_int(std::shared_ptr<DaedalusInstance> const& context,
	                    std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>> const& value,
	                    uint16_t index) const {
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

		return sym->get_int(index, context.get());
	}

	float
	DaedalusVm::get_float(std::shared_ptr<DaedalusInstance> const& context,
	                      std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>> const& value,
	                      uint16_t index) const {
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

		return sym->get_float(index, context.get());
	}

	void DaedalusVm::set_int(std::shared_ptr<DaedalusInstance> const& context,
	                         DaedalusSymbol* ref,
	                         uint16_t index,
	                         std::int32_t value) {
		if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
			throw DaedalusIllegalConstAccess {ref};
		}

		if (!ref->is_member() || context != nullptr ||
		    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
			ref->set_int(value, index, context.get());
		} else if (ref->is_member()) {
			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
		}
	}

	void DaedalusVm::set_float(std::shared_ptr<DaedalusInstance> const& context,
	                           DaedalusSymbol* ref,
	                           uint16_t index,
	                           float value) {
		if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
			throw DaedalusIllegalConstAccess {ref};
		}

		if (!ref->is_member() || context != nullptr ||
		    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
			ref->set_float(value, index, context.get());
		} else if (ref->is_member()) {
			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
		}
	}

	void DaedalusVm::set_string(std::shared_ptr<DaedalusInstance> const& context,
	                            DaedalusSymbol* ref,
	                            uint16_t index,
	                            std::string_view value) {
		if (ref->is_const() && !(_m_flags & DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER)) {
			throw DaedalusIllegalConstAccess {ref};
		}

		if (!ref->is_member() || context != nullptr ||
		    !(_m_flags & DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS)) {
			ref->set_string(value, index, context.get());
		} else if (ref->is_member()) {
			ZKLOGE("DaedalusVm", "Accessing member \"%s\" without an instance set", ref->name().c_str());
		}
	}

	void DaedalusVm::mark_free_string(uint32_t index) {
		this->_m_temporary_strings_free.push(index);
	}

	uint32_t DaedalusVm::get_free_string() {
		if (_m_temporary_strings_free.size() == 0) {
			// Like a normal array, we'll grow exponentially.
			auto prev_size = _m_temporary_strings->count();
			_m_temporary_strings->grow(prev_size * 2);

			// Mark new strings as unused.
			for (uint32_t i = prev_size; i < _m_temporary_strings->count(); ++i) {
				_m_temporary_strings_free.push(i);
			}
		}

		auto index = this->_m_temporary_strings_free.top();
		this->_m_temporary_strings_free.pop();
		return index;
	}
} // namespace zenkit
