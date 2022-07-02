// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/daedalus/script.hh>

#include <functional>
#include <optional>
#include <stack>
#include <tuple>
#include <typeinfo>
#include <variant>

namespace phoenix::daedalus {
	template <typename T>
	struct is_instance_ptr : std::false_type {};

	template <typename T> // clang-format off
	requires (std::derived_from<T, instance>)
	struct is_instance_ptr<std::shared_ptr<T>> : std::true_type { // clang-format on
		using instance_type = T;
	};

	class illegal_external : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	class illegal_external_rtype : public illegal_external {
	public:
		illegal_external_rtype(const symbol* sym, std::string_view provided);
	};

	class illegal_external_param : public illegal_external {
	public:
		illegal_external_param(const symbol* sym, std::string_view provided, std::uint8_t i);
	};

	struct daedalus_stack_frame {
		bool reference;
		std::variant<int32_t, float, symbol*, std::shared_ptr<instance>> value;
		uint16_t index {0};
	};

	struct daedalus_call_stack_frame {
		const symbol* function;
		std::uint32_t program_counter;
		std::shared_ptr<instance> context;
	};

	class vm : public script {
	public:
		/**
		 * @brief Creates a DaedalusVM instance for the given script.
		 * @param scr The script to load into the VM.
		 */
		explicit vm(script&& scr);

		/**
		 * @brief Calls a function by it's name.
		 * @param name The name of the function to call.
		 */
		void call_function(const std::string& name);

		/**
		 * @brief Calls a function by it's symbol.
		 * @param name The symbol of the function to call.
		 */
		void call_function(const symbol* sym);

		/**
		 * @brief Initializes an instance with the given type and name and returns it.
		 *
		 * This will result in a call into the VM to initialize the instance, so it may be slow.
		 * Try to initialize all your instances before doing time-critical stuff.
		 *
		 * @tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		 * @param name The name of the instance to initialize (ie. 'STT_309_WHISTLER')
		 * @return The initialized instance.
		 */
		template <class _instance_t> // clang-format off
		requires (std::derived_from<_instance_t, instance>)
		std::shared_ptr<_instance_t> init_instance(const std::string& name) { // clang-format on
			return init_instance<_instance_t>(find_symbol_by_name(name));
		}

		template <class _instance_t> // clang-format off
		requires (std::derived_from<_instance_t, instance>)
		std::shared_ptr<_instance_t> init_instance(symbol* sym) { // clang-format on
			if (sym == nullptr) {
				throw std::runtime_error {"Cannot init instance: not found"};
			}
			if (sym->type() != dt_instance) {
				throw std::runtime_error {"Cannot init " + sym->name() + ": not an instance"};
			}

			// check that the parent class is registered for the given instance type
			auto* parent = find_symbol_by_index(sym->parent());
			while (parent->type() != dt_class) {
				parent = find_symbol_by_index(parent->parent());
			}

			if (parent->registered_to() != typeid(_instance_t)) {
				throw std::runtime_error {"Cannot init " + sym->name() +
				                          ": parent class is not registered or is "
				                          "registered to a different instance class"};
			}

			// create the instance
			auto inst = std::make_shared<_instance_t>();
			inst->_m_symbol_index = sym->index();
			inst->_m_type = &typeid(_instance_t);

			// set the proper instances
			_m_instance = inst;
			sym->set_instance(_m_instance);

			if (_m_self_sym)
				_m_self_sym->set_instance(_m_instance);

			call(sym);
			return inst;
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
		[[nodiscard]] std::tuple<symbol*, std::uint8_t> pop_reference();

		/**
		 * @brief Registers a Daedalus external function.
		 *
		 * <p>External functions are a way for Daedalus scripts to execute more complicated code in the C++ world rather
		 * than in the interpreter. These external functions need to be registered with the interpreter in order to be
		 * able to be called from within a script. This is what this function does. If the symbol with the give name
		 * does not exist in the script, this function does nothing.</p>
		 *
		 * <p>External functions are passed in as std::function objects and are rigorously checked to match their
		 * definition in the script. They also have to meet strict criteria for argument and return types. Here's how
		 * Daedalus <b>argument</b> types map to C++ types:</p>
		 *
		 * <table border="1">
		 * 	<thead>
		 * 	 <th>Daedalus Argument Type</th>
		 * 	 <th>C++ Argument Type</th>
		 * 	 <th>Comment</th>
		 * 	</thead>
		 * 	<tbody>
		 * 		<tr><td>int</td><td><tt>int32_t|bool</tt></td><td></td></tr>
		 * 		<tr><td>func</td><td><tt>int32_t</tt></td><td>The value passed is the index of the function</td></tr>
		 * 		<tr><td>float</td><td><tt>float</tt></td><td></td></tr>
		 * 		<tr><td>string</td><td><tt>std::string_view</tt></td><td>Until I can find a way of properly passing
	<tt>const std::string&</tt></td></tr>
		 * 		<tr><td>{instance}</td><td><tt>std::shared_ptr&lt;{instance}&gt;</tt></td><td>where {instance} is the
	type of instance (ie. C_NPC) [<sup>1</sup>]</td></tr>
		 * 	</tbody>
		 * </table>
		 *
		 * <p>Return values are a bit different. Due to C++ language restrictions the mapping of C++ types to Daedalus
		 * types in return values is different than in arguments:</p>
		 *
		 * <table border="1">
		 * 	<thead>
		 * 	 <th>C++ Return Type</th>
		 * 	 <th>Daedalus Return Type</th>
		 * 	 <th>Comment</th>
		 * 	</thead>
	type of the C++ function does not match it's definition in the script.	 * 	<tbody>
		 * 		<tr><td>int8_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>int16_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>int32_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>int64_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>uint8_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>uint16_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>uint32_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>uint64_t</td><td><tt>int|func</tt></td><td></td></tr>
		 * 		<tr><td>bool</td><td><tt></tt>int|func</td><td></td></tr>
		 * 		<tr><td>float</td><td><tt>float</tt></td><td></td></tr>
		 * 		<tr><td>double</td><td><tt>float</tt></td><td></td></tr>
		 * 		<tr><td>std::shared_ptr&lt;{instance}&gt;</td><td><tt>{instance}</tt></td><td>where {instance} is the
	type of instance (ie. C_NPC) [<sup>1</sup>]</td></td></tr>
		 * 		<tr><td>std::string</td><td><tt>string</tt></td><td></td></tr>
		 * 		<tr><td>void</td><td><tt>void</tt></td><td></td></tr>
		 * 	</tbody>
		 * </table>
		 *
		 * <p>
		 * [<sup>1</sup>] instances in the C++-World have to inherit from phoenix::instance.
		 * </p>
		 *
		 * @tparam R The return type of the external.
		 * @tparam P The parameters types of the external.
		 * @param name The name of the external to register.
		 * @param callback The C++ function to register as the external.
		 * @throws illegal_external_rtype if the return type of the C++ function does not match it's definition in the
	script.
		 * @throws illegal_external_param if a parameter type of the C++ function does not match it's definition in the
	script.
		 * @throws illegal_external if The number of parameters of the definition and callback is not the same.
		 * @throws runtime_error if any other error occurs.
		 */
		template <typename R, typename... P>
		void register_external(const std::string& name, const std::function<R(P...)>& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr)
				return;
			if (!sym->is_external())
				throw std::runtime_error {"symbol is not external"};

			if constexpr (!std::same_as<void, R>) {
				if (!sym->has_return())
					throw illegal_external_rtype(sym, "<non-void>");
				if constexpr (is_instance_ptr<R>::value) {
					if (sym->rtype() != dt_instance)
						throw illegal_external_rtype(sym, "instance");
				} else if constexpr (std::floating_point<R>) {
					if (sym->rtype() != dt_float)
						throw illegal_external_rtype(sym, "float");
				} else if constexpr (std::convertible_to<int32_t, R>) {
					if (sym->rtype() != dt_integer)
						throw illegal_external_rtype(sym, "int");
				} else if constexpr (std::convertible_to<std::string, R>) {
					if (sym->rtype() != dt_string)
						throw illegal_external_rtype(sym, "string");
				} else {
					throw std::runtime_error {"unsupported return type"};
				}
			} else {
				if (sym->has_return())
					throw illegal_external_rtype(sym, "void");
			}

			std::vector<symbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P))
				throw illegal_external {"too many arguments declared for external " + sym->name() + ": declared " +
				                        std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};

			if (params.size() > sizeof...(P))
				throw illegal_external {"not enough arguments declared for external " + sym->name() + ": declared " +
				                        std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};

			if constexpr (sizeof...(P) > 0) {
				check_external_params<0, P...>(params);
			}

			// *evil template hacking ensues*
			_m_externals[sym] = [callback](vm& vm) {
				if constexpr (std::same_as<void, R>) {
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
		}

		/**
		 * @brief Registers an external function.
		 *
		 * Same as #register_external(const std::string&, std::function). This method is here to bypass a template
		 * deduction failure when passing lambdas directly.
		 *
		 * @tparam T The type of external function to register.
		 * @param name The name of the external to register.
		 * @param cb The callback function to register.
		 * @see #register_external(const std::string&, std::function)
		 */
		template <typename T>
		void register_external(const std::string& name, const T& cb) {
			register_external(name, std::function {cb});
		}

		/**
		 * @brief Overrides a function in Daedalus code with an external definition.
		 *
		 * Whenever the function with the given name would be called from within Daedalus code, redirect the call
		 * to the given external callback handler instead. Further documentation on these callback handlers can be
		 * found in the documentation for #register_external.
		 *
		 * @tparam R The return type of the external.
		 * @tparam P The parameters types of the external.
		 * @param name The name of the function to override.
		 * @param callback The C++ function to register as the external.
		 * @see #register_external(const std::string&, std::function)
		 * @todo Deduplicate source code!
		 */
		template <typename R, typename... P>
		void override_function(const std::string& name, const std::function<R(P...)>& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr)
				throw std::runtime_error {"symbol not found"};
			if (sym->is_external())
				throw std::runtime_error {"symbol is already an external"};

			if constexpr (!std::same_as<void, R>) {
				if (!sym->has_return())
					throw illegal_external_rtype(sym, "<non-void>");
				if constexpr (is_instance_ptr<R>::value) {
					if (sym->rtype() != dt_instance)
						throw illegal_external_rtype(sym, "instance");
				} else if constexpr (std::floating_point<R>) {
					if (sym->rtype() != dt_float)
						throw illegal_external_rtype(sym, "float");
				} else if constexpr (std::convertible_to<int32_t, R>) {
					if (sym->rtype() != dt_integer)
						throw illegal_external_rtype(sym, "int");
				} else if constexpr (std::convertible_to<std::string, R>) {
					if (sym->rtype() != dt_string)
						throw illegal_external_rtype(sym, "string");
				} else {
					throw std::runtime_error {"unsupported return type"};
				}
			} else {
				if (sym->has_return())
					throw illegal_external_rtype(sym, "void");
			}

			std::vector<symbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P))
				throw illegal_external {"too many arguments declared for function override " + sym->name() +
				                        ": declared " + std::to_string(sizeof...(P)) + " expected " +
				                        std::to_string(params.size())};

			if (params.size() > sizeof...(P))
				throw illegal_external {"not enough arguments declared for function override " + sym->name() +
				                        ": declared " + std::to_string(sizeof...(P)) + " expected " +
				                        std::to_string(params.size())};

			if constexpr (sizeof...(P) > 0) {
				check_external_params<0, P...>(params);
			}

			// *evil template hacking ensues*
			_m_function_overrides[sym->address()] = [callback](vm& vm) {
				if constexpr (std::same_as<void, R>) {
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
		}

		/**
		 * @brief Overrides a function in Daedalus code with an external definition.
		 *
		 * Same as #override_function(const std::string&, std::function). This method is here to bypass a template
		 * deduction failure when passing lambdas directly.
		 *
		 * @tparam T The type of external function to register.
		 * @param name The name of the function to override.
		 * @param cb The callback function to register.
		 * @see #override_function(const std::string&, std::function)
		 */
		template <typename T>
		void override_function(const std::string& name, const T& cb) {
			override_function(name, std::function {cb});
		}

		/**
		 * @brief Registers a function to be called when the script tries to call an external which has not been
		 *        registered.
		 *
		 * Before the callback is invoked, the VM makes sure that all parameters for the external are popped off the
		 * stack and a default return value (if needed) is pushed onto the stack. This prevents stack underflows.
		 *
		 * @param callback The function to call. The one parameter of the function is the name of the unresolved
		 * external.
		 */
		void register_default_external(const std::function<void(std::string_view)>& callback);

		/**
		 * @return the symbol referring to the global <tt>var C_NPC self</tt>.
		 */
		inline symbol* global_self() {
			return _m_self_sym;
		}

		/**
		 * @return the symbol referring to the global <tt>var C_NPC other</tt>.
		 */
		inline symbol* global_other() {
			return _m_other_sym;
		}

		/**
		 * @return the symbol referring to the global <tt>var C_NPC victim</tt>.
		 */
		inline symbol* global_victim() {
			return _m_victim_sym;
		}

		/**
		 * @return the symbol referring to the global <tt>var C_NPC hero</tt>.
		 */
		inline symbol* global_hero() {
			return _m_hero_sym;
		}

		/**
		 * @return the symbol referring to the global <tt>var C_NPC item</tt>.
		 */
		inline symbol* global_item() {
			return _m_item_sym;
		}

	protected:
		/**
		 * @brief Calls the given symbol as a function.
		 *
		 * Automatically pushes a call stack frame. If the function has a return value, the caller
		 * is required to deal with it appropriately.
		 *
		 * @param sym The symbol to call.
		 */
		void call(const symbol* sym);

		/**
		 * @brief Runs the instruction at the current program counter and advances it properly.
		 * @return false, the instruction executed was a op_return instruction, otherwise true.
		 */
		bool exec();

		/**
		 * @brief Validates the given address and jumps to it (sets the program counter).
		 * @param address The address to jump to.-
		 */
		void jump(std::uint32_t address);

		/**
		 * @brief Pushes a call stack frame onto the call stack.
		 *
		 * This method also pushes the current state of the interpreter to be restored as soon
		 * as #pop_call is invoked.
		 *
		 * @param sym The symbol referring to the function called.
		 */
		void push_call(const symbol* sym);

		/**
		 * @brief Pops a call stack from from the call stack.
		 *
		 * This method restores the interpreter's state to before the function which the
		 * call stack entry refers to was called.
		 */
		void pop_call();

		/**
		 * @brief Checks that the type of each symbol in the given set of defined symbols matches the given type
		 * parameters.
		 *
		 * This is used to validate that the parameters declared on an external function actually match the parameters
		 * defined in the script file. This prevents these hard to debug errors when the definition of an external does
		 * not match what is required by the script.
		 *
		 * @tparam i An iteration counter (pass in 0 when calling this from anywhere outside of itself)
		 * @tparam P A type
		 * @tparam Px more types.
		 * @param defined A list of symbols to check the types P and then P... against.
		 * @throws illegal_external_param If the types don't match.
		 * @note Requires that sizeof...(Px) + 1 == defined.size().
		 */
		template <int i, typename P, typename... Px>
		void check_external_params(const std::vector<symbol*>& defined) {
			if constexpr (is_instance_ptr<P>::value || std::same_as<symbol*, P>) {
				if (defined[i]->type() != dt_instance)
					throw illegal_external_param(defined[i], "instance", i + 1);
			} else if constexpr (std::same_as<float, P>) {
				if (defined[i]->type() != dt_float)
					throw illegal_external_param(defined[i], "float", i + 1);
			} else if constexpr (std::same_as<int32_t, P> || std::same_as<bool, P>) {
				if (defined[i]->type() != dt_integer && defined[i]->type() != dt_function)
					throw illegal_external_param(defined[i], "int", i + 1);
			} else if constexpr (std::same_as<std::string_view, P>) {
				if (defined[i]->type() != dt_string)
					throw illegal_external_param(defined[i], "string", i + 1);
			}

			if constexpr (sizeof...(Px) > 0) {
				check_external_params<i + 1, Px...>(defined);
			}
		}

		/**
		 * @brief Pops a value of the given type from the stack.
		 *
		 * This is only used by #pop_values_for_external for now as it does checks related to external
		 * functions. It is not recommended to use this function for anything else.
		 *
		 * @tparam T The type of the value to pop (one of std::shared_ptr<? extends instance>, float, int32_t,
		 *            symbol*, std::string_view)
		 * @return The value popped.
		 */
		template <typename T> // clang-format off
		requires (is_instance_ptr<T>::value || std::same_as<float, T> || std::same_as<std::int32_t, T> ||
		          std::same_as<bool, T> || std::same_as<std::string_view, T> || std::same_as<symbol*, T>)
		inline T pop_value_for_external() { // clang-format on
			if constexpr (is_instance_ptr<T>::value) {
				auto r = pop_instance();

				if (r != nullptr) {
					auto& expected = typeid(typename is_instance_ptr<T>::instance_type);

					if (!r->_m_type) {
						throw std::runtime_error {"Popping instance of unregistered type: " +
						                          std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}

					if (*r->_m_type != expected) {
						throw std::runtime_error {"Popping instance of wrong type: " +
						                          std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}
				}

				return std::static_pointer_cast<typename is_instance_ptr<T>::instance_type>(r);
			} else if constexpr (std::same_as<float, T>) {
				return pop_float();
			} else if constexpr (std::same_as<std::int32_t, T>) {
				return pop_int();
			} else if constexpr (std::same_as<bool, T>) {
				return pop_int() != 0;
			} else if constexpr (std::same_as<std::string_view, T>) {
				return pop_string();
			} else if constexpr (std::same_as<symbol*, T>) {
				return std::get<0>(pop_reference());
			} else {
				throw std::runtime_error {"pop: unsupported stack frame type"};
			}
		}

		/**
		 * @brief Pushes a value of the given type onto the stack.
		 *
		 * This is only used by #register_external for pushing the return value of external functions onto
		 * the stack. It is not recommended to use this function for anything else.
		 *
		 * @tparam T The type of value to push.
		 * @param v The value to push.
		 */
		template <typename T> // clang-format off
		requires (std::floating_point<T> || std::convertible_to<std::int32_t, T> || std::convertible_to<std::string, T> ||
		          is_instance_ptr<T>::value)
		void push_value_from_external(T v) { // clang-format on
			if constexpr (is_instance_ptr<T>::value) {
				push_instance(std::static_pointer_cast<instance>(v));
			} else if constexpr (std::floating_point<T>) {
				push_float(static_cast<float>(v));
			} else if constexpr (std::convertible_to<std::int32_t, T>) {
				push_int(static_cast<std::int32_t>(v));
			} else if constexpr (std::convertible_to<std::string, T>) {
				push_string(v);
			} else {
				throw std::runtime_error {"push: unsupported stack frame type"};
			}
		}

		/**
		 * @brief The given types in reverse order using #pop_value_for_external.
		 *
		 * This is used to pop values from the stack in the correct order for calling an external function
		 * since the latest value on the stack is the last parameter to the function call.
		 *
		 * @tparam P A value type to pop from the stack (one of std::shared_ptr<? extends instance>, float, int32_t,
		 *           symbol*, std::string_view)
		 * @tparam Px more value types to pop.
		 * @return
		 */
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

	private:
		std::stack<daedalus_stack_frame> _m_stack;
		std::stack<daedalus_call_stack_frame> _m_call_stack;
		std::unordered_map<symbol*, std::function<void(vm&)>> _m_externals;
		std::unordered_map<uint32_t, std::function<void(vm&)>> _m_function_overrides;
		std::optional<std::function<void(vm&, symbol&)>> _m_external_error_handler {std::nullopt};

		symbol* _m_self_sym;
		symbol* _m_other_sym;
		symbol* _m_victim_sym;
		symbol* _m_hero_sym;
		symbol* _m_item_sym;

		symbol* _m_temporary_strings;

		std::shared_ptr<instance> _m_instance;
		std::uint32_t _m_pc {0};

		void print_stack_trace();
	};
} // namespace phoenix::daedalus
