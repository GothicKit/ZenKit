// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/script.hh>

#include <functional>
#include <optional>
#include <stack>
#include <tuple>
#include <typeinfo>
#include <variant>

namespace phoenix {
	struct _ignore_return_value {};

	template <typename T>
	static constexpr bool is_instance_ptr_v = false;

	template <typename T>
	static constexpr bool is_instance_ptr_v<std::shared_ptr<T>> = std::is_base_of_v<instance, T>;

	/// \brief An exception thrown if the definition of an external is incorrect.
	class illegal_external_definition : public script_error {
	public:
		illegal_external_definition(const symbol* sym, std::string&& message);

	public:
		/// \brief The symbol the external is being registered for.
		const symbol* sym;
	};

	/// \brief An exception thrown if the return type of a new external registration does not match
	///        the return type defined in the script.
	class illegal_external_rtype : public illegal_external_definition {
	public:
		illegal_external_rtype(const symbol* sym, std::string&& provided);
	};

	/// \brief An exception thrown if one of the parameter types of a new external registration does not match
	///        the type defined in the script.
	class illegal_external_param : public illegal_external_definition {
	public:
		illegal_external_param(const symbol* sym, std::string&& provided, std::uint8_t i);
	};

	class vm_exception : public script_error {
		using script_error::script_error;
	};

	/// \brief An exception handling strategy given by VM exception handlers.
	enum class vm_exception_strategy {
		/// \brief Continue with execution of the script.
		continue_,

		/// \brief Return from the current subroutine.
		return_,

		/// \brief Re-throw the exception and fail.
		fail_,
	};

	/// \brief A stack frame in the VM.
	struct daedalus_stack_frame {
		std::shared_ptr<instance> context;
		bool reference;
		std::variant<int32_t, float, symbol*, std::shared_ptr<instance>> value;
		uint16_t index {0};
	};

	/// \brief A call stack frame in the VM.
	struct daedalus_call_stack_frame {
		const symbol* function;
		std::uint32_t program_counter;
		std::shared_ptr<instance> context;
	};

	namespace execution_flag {
		static constexpr std::uint8_t none = 0;
		static constexpr std::uint8_t vm_allow_null_instance_access = 1 << 1;
	}; // namespace execution_flag

	class vm : public script {
	public:
		/// \brief Creates a DaedalusVM instance for the given script.
		/// \param scr The script to load into the VM.
		explicit vm(script&& scr, uint8_t flags = execution_flag::none);

		/// \brief Calls a function by it's name.
		/// \tparam P The types for the argument values.
		/// \param sym The name of the function to call.
		/// \param args The arguments for the function call.
		template <typename R = _ignore_return_value, typename... P>
		R call_function(const std::string& name, P... args) {
			return call_function<R, P...>(find_symbol_by_name(name), args...);
		}

		/// \brief Calls a function by it's symbol.
		/// \tparam P The types for the argument values.
		/// \param sym The symbol of the function to call.
		/// \param args The arguments for the function call.
		template <typename R = _ignore_return_value, typename... P>
		R call_function(const symbol* sym, P... args) {
			if (sym == nullptr) {
				throw vm_exception {"Cannot call function: not found"};
			}

			if (sym->type() != datatype::function) {
				throw vm_exception {"Cannot call " + sym->name() + ": not a function"};
			}

			std::vector<symbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P)) {
				throw vm_exception {"too many arguments provided for " + sym->name() + ": given " +
				                    std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};
			}

			if (params.size() > sizeof...(P)) {
				throw vm_exception {"not enough arguments provided for " + sym->name() + ": given " +
				                    std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};
			}

			if constexpr (!std::is_same_v<R, _ignore_return_value>) {
				check_call_return_type<R>(sym);
			}

			if constexpr (sizeof...(P) > 0) {
				push_call_parameters<0, P...>(params, args...);
			}

			PX_LOGD("vm: calling function {}", sym->name());
			call(sym);

			if constexpr (std::is_same_v<R, _ignore_return_value>) {
				// clear the stack
				_m_stack = std::stack<daedalus_stack_frame> {};

				return {};
			} else if constexpr (!std::is_same_v<R, void>) {
				auto ret = pop_call_return_value<R>();

				// clear the stack
				_m_stack = std::stack<daedalus_stack_frame> {};

				return ret;
			} else {
				// clear the stack
				_m_stack = std::stack<daedalus_stack_frame> {};
			}
		}

		/// \brief Initializes an instance with the given type and name and returns it.
		///
		/// This will result in a call into the VM to initialize the instance, so it may be slow.
		/// Try to initialize all your instances before doing time-critical stuff.
		///
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param name The name of the instance to initialize (ie. 'STT_309_WHISTLER')
		/// \return The initialized instance.
		template <typename _instance_t>
		typename std::enable_if<std::is_base_of_v<instance, _instance_t>, std::shared_ptr<_instance_t>>::type
		init_instance(const std::string& name) {
			return init_instance<_instance_t>(find_symbol_by_name(name));
		}

		/// \brief Initializes an instance with the given type into \p instance
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param instance The instance to initialize.
		/// \param name The name of the instance to initialize (ie. 'STT_309_WHISTLER')
		template <typename _instance_t>
		typename std::enable_if<std::is_base_of_v<instance, _instance_t>, void>::type
		init_instance(const std::shared_ptr<_instance_t>& instance, const std::string& name) {
			init_instance<_instance_t>(instance, find_symbol_by_name(name));
		}

		/// \brief Initializes an instance with the given type and name and returns it.
		///
		/// This will result in a call into the VM to initialize the instance, so it may be slow.
		/// Try to initialize all your instances before doing time-critical stuff.
		///
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param sym The symbol to initialize.
		/// \return The initialized instance.
		template <typename _instance_t>
		typename std::enable_if<std::is_base_of_v<instance, _instance_t>, std::shared_ptr<_instance_t>>::type
		init_instance(symbol* sym) {
			// create the instance
			auto inst = std::make_shared<_instance_t>();
			init_instance(inst, sym);
			return inst;
		}

		/// \brief Initializes an instance with the given type into \p instance
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param instance The instance to initialize.
		/// \param sym The symbol to initialize.
		template <typename _instance_t>
		typename std::enable_if<std::is_base_of_v<instance, _instance_t>, void>::type
		init_instance(const std::shared_ptr<_instance_t>& instance, symbol* sym) {
			if (sym == nullptr) {
				throw vm_exception {"Cannot init instance: not found"};
			}
			if (sym->type() != datatype::instance) {
				throw vm_exception {"Cannot init " + sym->name() + ": not an instance"};
			}

			// check that the parent class is registered for the given instance type
			auto* parent = find_symbol_by_index(sym->parent());
			while (parent->type() != datatype::class_) {
				parent = find_symbol_by_index(parent->parent());
			}

			if (parent->registered_to() != typeid(_instance_t)) {
				throw vm_exception {"Cannot init " + sym->name() +
				                    ": parent class is not registered or is "
				                    "registered to a different instance class"};
			}

			PX_LOGD("vm: initializing instance {}", sym->name());

			instance->_m_symbol_index = sym->index();
			instance->_m_type = &typeid(_instance_t);

			// set the proper instances
			auto old_instance = _m_instance;
			auto old_self_instance = _m_self_sym != nullptr ? _m_self_sym->get_instance() : nullptr;
			_m_instance = instance;
			sym->set_instance(_m_instance);

			if (_m_self_sym)
				_m_self_sym->set_instance(_m_instance);

			call(sym);

			// reset the VM state
			_m_instance = old_instance;
			if (_m_self_sym)
				_m_self_sym->set_instance(old_self_instance);
		}

		void push_int(std::int32_t value);
		void push_float(float value);
		void push_instance(std::shared_ptr<instance> value);
		void push_reference(symbol* value, std::uint8_t index = 0);
		void push_string(const std::string& value);

		[[nodiscard]] std::int32_t pop_int();
		[[nodiscard]] float pop_float();
		[[nodiscard]] std::shared_ptr<instance> pop_instance();
		[[nodiscard]] const std::string& pop_string();
		[[nodiscard]] std::tuple<symbol*, std::uint8_t, std::shared_ptr<instance>> pop_reference();

		/// \brief Registers a Daedalus external function.
		///
		/// <p>External functions are a way for Daedalus scripts to execute more complicated code in the C++ world
		/// rather than in the interpreter. These external functions need to be registered with the interpreter in
		/// order to be able to be called from within a script. This is what this function does. If the symbol with
		/// the give name does not exist in the script, this function does nothing.</p>
		///
		/// <p>External functions are passed in as std::function objects and are rigorously checked to match their
		/// definition in the script. They also have to meet strict criteria for argument and return types. Here's
		/// how Daedalus <b>argument</b> types map to C++ types:</p>
		///
		/// <table border="1">
		/// 	<thead>
		/// 	 <th>Daedalus Argument Type</th>
		/// 	 <th>C++ Argument Type</th>
		/// 	 <th>Comment</th>
		/// 	</thead>
		/// 	<tbody>
		/// 		<tr><td>int</td><td><tt>int32_t|bool</tt></td><td></td></tr>
		/// 		<tr><td>func</td><td><tt>int32_t</tt></td><td>The value passed is the index of the
		/// function</td></tr>
		///         <tr><td>float</td><td><tt>float</tt></td><td></td></tr>
		/// 		<tr>
		///         	<td>string</td>
		/// 			<td><tt>std::string_view</tt></td>
		/// 			<td>Until I can find a way of properly passing <tt> const std::string&</tt></td>
		/// 		</tr>
		/// 		<tr>
		/// 			<td>{instance}</td>
		/// 			<td><tt>std::shared_ptr&lt;{instance}&gt;</tt></td>
		/// 			<td>where {instance} is the type of instance(ie.C_NPC)[<sup>1</sup>]</td></tr>
		/// 	</tbody>
		/// </table>
		///
		/// <p>Return values are a bit different. Due to C++ language restrictions the mapping of
		/// C++ types to Daedalus types in return values is different than in arguments:</p>
		///
		/// <table border="1">
		/// 	<thead>
		/// 	 <th>C++ Return Type</th>
		/// 	 <th>Daedalus Return Type</th>
		/// 	 <th>Comment</th>
		/// 	</thead>
		/// 	<tbody>
		/// 		<tr><td>int8_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>int16_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>int32_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>int64_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>uint8_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>uint16_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>uint32_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>uint64_t</td><td><tt>int|func</tt></td><td></td></tr>
		/// 		<tr><td>bool</td><td><tt></tt>int|func</td><td></td></tr>
		/// 		<tr><td>float</td><td><tt>float</tt></td><td></td></tr>
		/// 		<tr><td>double</td><td><tt>float</tt></td><td></td></tr>
		/// 		<tr>
		/// 			<td>std::shared_ptr&lt;{instance}&gt;</td>
		/// 			<td><tt>{instance}</tt></td>
		/// 			<td>where {instance} is the type of instance(ie.C_NPC)[<sup>1</sup>]</td></td>
		/// 		</tr>
		/// 		<tr><td>std::string</td><td><tt>string</tt></td><td></td></tr>
		/// 		<tr><td>void</td><td><tt>void</tt></td><td></td></tr>
		/// 	</tbody>
		/// </table>
		///
		/// <p>
		/// [<sup>1</sup>] instances in the C++-World have to inherit from phoenix::instance.
		/// </p>
		///
		/// \tparam R The return type of the external.
		/// \tparam P The parameters types of the external.
		/// \param name The name of the external to register.
		/// \param callback The C++ function to register as the external.
		/// \throws illegal_external_rtype if the return type of the C++ function does not match it's
		///                                definition in the script.
		/// \throws illegal_external_param if a parameter type of the C++ function does not match it's
		///                                definition in the script.
		/// \throws illegal_external if The number of parameters of the definition and callback is not the same.
		/// \throws runtime_error if any other error occurs.
		template <typename R, typename... P>
		void register_external(const std::string& name, const std::function<R(P...)>& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr)
				return;

			if (!sym->is_external())
				throw vm_exception {"symbol is not external"};

			if constexpr (!std::is_same_v<void, R>) {
				if (!sym->has_return())
					throw illegal_external_rtype(sym, "<non-void>");
				if constexpr (is_instance_ptr_v<R>) {
					if (sym->rtype() != datatype::instance)
						throw illegal_external_rtype(sym, "instance");
				} else if constexpr (std::is_floating_point_v<R>) {
					if (sym->rtype() != datatype::float_)
						throw illegal_external_rtype(sym, "float");
				} else if constexpr (std::is_convertible_v<int32_t, R>) {
					if (sym->rtype() != datatype::integer)
						throw illegal_external_rtype(sym, "int");
				} else if constexpr (std::is_convertible_v<std::string, R>) {
					if (sym->rtype() != datatype::string)
						throw illegal_external_rtype(sym, "string");
				} else {
					throw vm_exception {"unsupported return type"};
				}
			} else {
				if (sym->has_return())
					throw illegal_external_rtype(sym, "void");
			}

			std::vector<symbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P))
				throw illegal_external_definition {sym,
				                                   "too many arguments declared for external " + sym->name() +
				                                       ": declared " + std::to_string(sizeof...(P)) + " expected " +
				                                       std::to_string(params.size())};

			if (params.size() > sizeof...(P))
				throw illegal_external_definition {sym,
				                                   "not enough arguments declared for external " + sym->name() +
				                                       ": declared " + std::to_string(sizeof...(P)) + " expected " +
				                                       std::to_string(params.size())};

			if constexpr (sizeof...(P) > 0) {
				check_external_params<0, P...>(params);
			}

			// *evil template hacking ensues*
			_m_externals[sym] = [callback](vm& vm) {
				if constexpr (std::is_same_v<void, R>) {
					if constexpr (sizeof...(P) > 0) {
						auto v = vm.pop_values_for_external<P...>();
						std::apply(callback, v);
					} else {
						callback();
					}
				} else {
					if constexpr (sizeof...(P) > 0) {
						vm.push_value_from_external(std::apply(callback, vm.pop_values_for_external<P...>()));
					} else {
						vm.push_value_from_external(callback());
					}
				}
			};

			PX_LOGD("vm: registered external for {}", sym->name());
		}

		/// \brief Registers an external function.
		///
		/// Same as #register_external(const std::string&, std::function). This method is here to bypass a template
		/// deduction failure when passing lambdas directly.
		///
		/// \tparam T The type of external function to register.
		/// \param name The name of the external to register.
		/// \param cb The callback function to register.
		/// \see #register_external(const std::string&, std::function)
		template <typename T>
		void register_external(const std::string& name, const T& cb) {
			register_external(name, std::function {cb});
		}

		/// \brief Overrides a function in Daedalus code with an external definition.
		///
		/// Whenever the function with the given name would be called from within Daedalus code, redirect the call
		/// to the given external callback handler instead. Further documentation on these callback handlers can be
		/// found in the documentation for #register_external.
		///
		/// \tparam R The return type of the external.
		/// \tparam P The parameters types of the external.
		/// \param name The name of the function to override.
		/// \param callback The C++ function to register as the external.
		/// \see #register_external(const std::string&, std::function)
		/// \todo Deduplicate source code!
		template <typename R, typename... P>
		void override_function(const std::string& name, const std::function<R(P...)>& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr)
				throw vm_exception {"symbol not found"};
			if (sym->is_external())
				throw vm_exception {"symbol is already an external"};

			if constexpr (!std::is_same_v<void, R>) {
				if (!sym->has_return())
					throw illegal_external_rtype(sym, "<non-void>");
				if constexpr (is_instance_ptr_v<R>) {
					if (sym->rtype() != datatype::instance)
						throw illegal_external_rtype(sym, "instance");
				} else if constexpr (std::is_floating_point_v<R>) {
					if (sym->rtype() != datatype::float_)
						throw illegal_external_rtype(sym, "float");
				} else if constexpr (std::is_convertible_v<int32_t, R>) {
					if (sym->rtype() != datatype::integer)
						throw illegal_external_rtype(sym, "int");
				} else if constexpr (std::is_convertible_v<std::string, R>) {
					if (sym->rtype() != datatype::string)
						throw illegal_external_rtype(sym, "string");
				} else {
					throw vm_exception {"unsupported return type"};
				}
			} else {
				if (sym->has_return())
					throw illegal_external_rtype(sym, "void");
			}

			std::vector<symbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P))
				throw illegal_external_definition {sym,
				                                   "too many arguments declared for function override " + sym->name() +
				                                       ": declared " + std::to_string(sizeof...(P)) + " expected " +
				                                       std::to_string(params.size())};

			if (params.size() > sizeof...(P))
				throw illegal_external_definition {sym,
				                                   "not enough arguments declared for function override " +
				                                       sym->name() + ": declared " + std::to_string(sizeof...(P)) +
				                                       " expected " + std::to_string(params.size())};

			if constexpr (sizeof...(P) > 0) {
				check_external_params<0, P...>(params);
			}

			// *evil template hacking ensues*
			_m_function_overrides[sym->address()] = [callback](vm& vm) {
				if constexpr (std::is_same_v<void, R>) {
					if constexpr (sizeof...(P) > 0) {
						auto v = vm.pop_values_for_external<P...>();
						std::apply(callback, v);
					} else {
						callback();
					}
				} else {
					if constexpr (sizeof...(P) > 0) {
						vm.push_value_from_external(std::apply(callback, vm.pop_values_for_external<P...>()));
					} else {
						vm.push_value_from_external(callback());
					}
				}
			};

			PX_LOGD("vm: overrode function {}", sym->name());
		}

		/// \brief Overrides a function in Daedalus code with an external definition.
		///
		/// Same as #override_function(const std::string&, std::function). This method is here to bypass a template
		/// deduction failure when passing lambdas directly.
		///
		/// \tparam T The type of external function to register.
		/// \param name The name of the function to override.
		/// \param cb The callback function to register.
		/// \see #override_function(const std::string&, std::function)
		template <typename T>
		void override_function(const std::string& name, const T& cb) {
			override_function(name, std::function {cb});
		}

		/// \brief Registers a function to be called when the script tries to call an external which has not been
		///        registered.
		///
		/// Before the callback is invoked, the VM makes sure that all parameters for the external are popped off
		/// the stack and a default return value (if needed) is pushed onto the stack. This prevents stack
		/// underflows.
		///
		/// \param callback The function to call. The one parameter of the function is the name of the unresolved
		/// external.
		void register_default_external(const std::function<void(std::string_view)>& callback);

		/// \brief Registers a function to be called when script execution fails.
		///
		/// A variety of exceptions can occur within the VM while executing. The function passed to this handler can
		/// inspect and alter the VMs state to try to correct the error.
		///
		/// \param callback The function to call. It receives a reference to the VM and the exception that occurred.
		///                 If the function returns `true` the error is assumed to have been handled and execution will
		///                 continue as normal. If `false` is returned, the VM will re-raise the exception and thus,
		///                 halt execution.
		void register_exception_handler(
		    const std::function<vm_exception_strategy(vm&, const script_error&, const instruction&)>& callback);

		/// \return the symbol referring to the global <tt>var C_NPC self</tt>.
		inline symbol* global_self() {
			return _m_self_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC other</tt>.
		inline symbol* global_other() {
			return _m_other_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC victim</tt>.
		inline symbol* global_victim() {
			return _m_victim_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC hero</tt>.
		inline symbol* global_hero() {
			return _m_hero_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC item</tt>.
		inline symbol* global_item() {
			return _m_item_sym;
		}

		/// \brief Prints the contents of the function call stack and the VMs stack to stderr.
		void print_stack_trace() const;

		/// \return The current program counter (or instruction index) the VM is at.
		[[nodiscard]] inline uint32_t pc() const noexcept {
			return _m_pc;
		}

	protected:
		/// \brief Calls the given symbol as a function.
		///
		/// Automatically pushes a call stack frame. If the function has a return value, the caller
		/// is required to deal with it appropriately.
		///
		/// \param sym The symbol to call.
		void call(const symbol* sym);

		/// \brief Runs the instruction at the current program counter and advances it properly.
		/// \return false, the instruction executed was a op_return instruction, otherwise true.
		bool exec();

		/// \brief Validates the given address and jumps to it (sets the program counter).
		/// \param address The address to jump to.
		void jump(std::uint32_t address);

		/// \brief Pushes a call stack frame onto the call stack.
		///
		/// This method also pushes the current state of the interpreter to be restored as soon
		/// as #pop_call is invoked.
		///
		/// \param sym The symbol referring to the function called.
		void push_call(const symbol* sym);

		/// \brief Pops a call stack from from the call stack.
		///
		/// This method restores the interpreter's state to before the function which the
		/// call stack entry refers to was called.
		void pop_call();

		/// \brief Checks that the type of each symbol in the given set of defined symbols matches the given type
		/// parameters.
		///
		/// This is used to validate that the parameters declared on an external function actually match the
		/// parameters defined in the script file. This prevents these hard to debug errors when the definition of
		/// an external does not match what is required by the script.
		///
		/// \tparam i An iteration counter (pass in 0 when calling this from anywhere outside of itself)
		/// \tparam P A type
		/// \tparam Px more types.
		/// \param defined A list of symbols to check the types P and then P... against.
		/// \throws illegal_external_param If the types don't match.
		/// \note Requires that sizeof...(Px) + 1 == defined.size().
		template <int i, typename P, typename... Px>
		void check_external_params(const std::vector<symbol*>& defined) {
			if constexpr (is_instance_ptr_v<P> || std::is_same_v<symbol*, P>) {
				if (defined[i]->type() != datatype::instance)
					throw illegal_external_param(defined[i], "instance", i + 1);
			} else if constexpr (std::is_same_v<float, P>) {
				if (defined[i]->type() != datatype::float_)
					throw illegal_external_param(defined[i], "float", i + 1);
			} else if constexpr (std::is_same_v<int32_t, P> || std::is_same_v<bool, P>) {
				if (defined[i]->type() != datatype::integer && defined[i]->type() != datatype::function)
					throw illegal_external_param(defined[i], "int", i + 1);
			} else if constexpr (std::is_same_v<std::string_view, P>) {
				if (defined[i]->type() != datatype::string)
					throw illegal_external_param(defined[i], "string", i + 1);
			}

			if constexpr (sizeof...(Px) > 0) {
				check_external_params<i + 1, Px...>(defined);
			}
		}

		/// \brief Pops a value of the given type from the stack.
		///
		/// This is only used by #pop_values_for_external for now as it does checks related to external
		/// functions. It is not recommended to use this function for anything else.
		///
		/// \tparam T The type of the value to pop (one of std::shared_ptr<? extends instance>, float, int32_t,
		///            symbol*, std::string_view)
		/// \return The value popped.
		template <typename T>
		typename std::enable_if<is_instance_ptr_v<T> || std::is_same_v<T, float> || std::is_same_v<T, std::int32_t> ||
		                            std::is_same_v<T, bool> || std::is_same_v<T, std::string_view> ||
		                            std::is_same_v<T, symbol*>,
		                        T>::type
		pop_value_for_external() {
			if constexpr (is_instance_ptr_v<T>) {
				auto r = pop_instance();

				if (r != nullptr && !std::is_same_v<T, std::shared_ptr<phoenix::instance>>) {
					auto& expected = typeid(typename T::element_type);

					if (!r->_m_type) {
						throw vm_exception {"Popping instance of unregistered type: " +
						                    std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}

					if (*r->_m_type != expected) {
						throw vm_exception {"Popping instance of wrong type: " + std::string {r->_m_type->name()} +
						                    ", expected " + expected.name()};
					}
				}

				return std::static_pointer_cast<typename T::element_type>(r);
			} else if constexpr (std::is_same_v<float, T>) {
				return pop_float();
			} else if constexpr (std::is_same_v<std::int32_t, T>) {
				return pop_int();
			} else if constexpr (std::is_same_v<bool, T>) {
				return pop_int() != 0;
			} else if constexpr (std::is_same_v<std::string_view, T>) {
				return pop_string();
			} else if constexpr (std::is_same_v<symbol*, T>) {
				return std::get<0>(pop_reference());
			} else {
				throw vm_exception {"pop: unsupported stack frame type"};
			}
		}

		/// \brief Pushes a value of the given type onto the stack.
		///
		/// This is only used by #register_external for pushing the return value of external functions onto
		/// the stack. It is not recommended to use this function for anything else.
		///
		/// \tparam T The type of value to push.
		/// \param v The value to push.
		template <typename T>
		typename std::enable_if<is_instance_ptr_v<T> || std::is_convertible_v<T, float> ||
		                            std::is_convertible_v<T, std::int32_t> || std::is_same_v<T, std::string>,
		                        void>::type
		push_value_from_external(T v) { // clang-format on
			if constexpr (is_instance_ptr_v<T>) {
				push_instance(std::static_pointer_cast<instance>(v));
			} else if constexpr (std::is_floating_point_v<T>) {
				push_float(static_cast<float>(v));
			} else if constexpr (std::is_convertible_v<std::int32_t, T>) {
				push_int(static_cast<std::int32_t>(v));
			} else if constexpr (std::is_same_v<T, std::string>) {
				push_string(v);
			} else {
				throw vm_exception {"push: unsupported stack frame type"};
			}
		}

		/// \brief The given types in reverse order using #pop_value_for_external.
		///
		/// This is used to pop values from the stack in the correct order for calling an external function
		/// since the latest value on the stack is the last parameter to the function call.
		///
		/// \tparam P A value type to pop from the stack (one of std::shared_ptr<? extends instance>, float,
		/// int32_t,
		///           symbol*, std::string_view)
		/// \tparam Px more value types to pop.
		/// \return
		template <typename P, typename... Px>
		std::tuple<P, Px...> pop_values_for_external() {
			// TODO: find a way to actually retain constref-ness of strings and instances through this chain of
			//  tuple-making
			if constexpr (sizeof...(Px) > 0) {
				auto v = pop_values_for_external<Px...>();
				return std::tuple_cat(std::make_tuple(pop_value_for_external<P>()), std::move(v));
			} else {
				return {pop_value_for_external<P>()};
			}
		}

		template <typename R>
		typename std::enable_if<is_instance_ptr_v<R> || std::is_same_v<R, float> || std::is_same_v<R, std::int32_t> ||
		                            std::is_same_v<R, std::string> || std::is_same_v<R, void>,
		                        void>::type
		check_call_return_type(const symbol* sym) {
			if constexpr (is_instance_ptr_v<R>) {
				if (sym->rtype() != datatype::instance)
					throw vm_exception {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<float, R>) {
				if (sym->rtype() != datatype::float_)
					throw vm_exception {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<int32_t, R>) {
				if (sym->rtype() != datatype::integer && sym->rtype() != datatype::function)
					throw vm_exception {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<std::string, R>) {
				if (sym->rtype() != datatype::string)
					throw vm_exception {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<void, R>) {
				if (sym->rtype() != datatype::void_)
					throw vm_exception {"invalid return type for function " + sym->name()};
			}
		}

		template <int i, typename P, typename... Px>
		typename std::enable_if<is_instance_ptr_v<P> || std::is_same_v<std::remove_reference_t<P>, float> ||
		                            std::is_same_v<std::remove_reference_t<P>, std::int32_t> ||
		                            std::is_same_v<std::remove_reference_t<P>, bool> ||
		                            std::is_same_v<std::remove_reference_t<P>, std::string_view> ||
		                            std::is_same_v<std::remove_reference_t<P>, symbol*>,
		                        void>::type
		push_call_parameters(const std::vector<symbol*>& defined, P value, Px... more) { // clang-format on
			if constexpr (is_instance_ptr_v<P> || std::is_same_v<symbol*, P>) {
				if (defined[i]->type() != datatype::instance)
					throw illegal_external_param(defined[i], "instance", i + 1);

				push_instance(value);
			} else if constexpr (std::is_same_v<float, P>) {
				if (defined[i]->type() != datatype::float_)
					throw illegal_external_param(defined[i], "float", i + 1);

				push_float(value);
			} else if constexpr (std::is_same_v<int32_t, P> || std::is_same_v<bool, P>) {
				if (defined[i]->type() != datatype::integer && defined[i]->type() != datatype::function)
					throw illegal_external_param(defined[i], "int", i + 1);

				push_int(value);
			} else if constexpr (std::is_same_v<std::string_view, P>) {
				if (defined[i]->type() != datatype::string)
					throw illegal_external_param(defined[i], "string", i + 1);

				push_string(value);
			}

			if constexpr (sizeof...(Px) > 0) {
				push_call_parameters<i + 1, Px...>(defined, more...);
			}
		}

		template <typename R>
		R pop_call_return_value() {
			if constexpr (is_instance_ptr_v<R>) {
				auto r = pop_instance();

				if (r != nullptr) {
					auto& expected = typeid(typename R::element_type);

					if (!r->_m_type) {
						throw vm_exception {"Popping instance of unregistered type: " +
						                    std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}

					if (*r->_m_type != expected) {
						throw vm_exception {"Popping instance of wrong type: " + std::string {r->_m_type->name()} +
						                    ", expected " + expected.name()};
					}
				}

				return std::static_pointer_cast<typename R::element_type>(r);
			} else if constexpr (std::is_same_v<float, R>) {
				return pop_float();
			} else if constexpr (std::is_same_v<int32_t, R>) {
				return pop_int();
			} else if constexpr (std::is_same_v<std::string, R>) {
				return pop_string();
			}
		}

	private:
		std::stack<daedalus_stack_frame> _m_stack;
		std::stack<daedalus_call_stack_frame> _m_call_stack;
		std::unordered_map<symbol*, std::function<void(vm&)>> _m_externals;
		std::unordered_map<uint32_t, std::function<void(vm&)>> _m_function_overrides;
		std::optional<std::function<void(vm&, symbol&)>> _m_default_external {std::nullopt};
		std::optional<std::function<vm_exception_strategy(vm&, const script_error&, const instruction&)>>
		    _m_exception_handler {std::nullopt};

		symbol* _m_self_sym;
		symbol* _m_other_sym;
		symbol* _m_victim_sym;
		symbol* _m_hero_sym;
		symbol* _m_item_sym;

		symbol* _m_temporary_strings;

		std::shared_ptr<instance> _m_instance;
		std::uint32_t _m_pc {0};
		std::uint8_t _m_flags {execution_flag::none};
	};

	/// \brief A VM exception handler which handles some and pretends to handle other VM exceptions.
	///
	/// Some exceptions are just ignored and some are handled properly.
	///
	/// \param v The VM the exception occurred in.
	/// \param exc The exception being handled.
	/// \param instr The instruction being executed.
	/// \return vm_exception_strategy::continue_
	vm_exception_strategy lenient_vm_exception_handler(vm& v, const script_error& exc, const instruction& instr);
} // namespace phoenix
