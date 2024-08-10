// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/DaedalusScript.hh"
#include "zenkit/Library.hh"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <unordered_map>

namespace zenkit {
	struct IgnoreReturnValue {};

	struct DaedalusNakedCall {};

	struct DaedalusFunction {
		DaedalusSymbol* value;
	};

	template <typename T>
	static constexpr bool is_instance_ptr_v = false;

	template <typename T>
	static constexpr bool is_instance_ptr_v<std::shared_ptr<T>> = std::is_base_of_v<DaedalusInstance, T>;

	template <typename T>
	static constexpr bool is_raw_instance_ptr_v = std::is_base_of_v<DaedalusInstance, std::remove_pointer_t<T>>;

	/// \brief An exception thrown if the definition of an external is incorrect.
	class DaedalusIllegalExternalDefinition : public DaedalusScriptError {
	public:
		ZKAPI DaedalusIllegalExternalDefinition(DaedalusSymbol const* sym, std::string&& message);

		/// \brief The symbol the external is being registered for.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if the return type of a new external registration does not match
	///        the return type defined in the script.
	class DaedalusIllegalExternalReturnType final : public DaedalusIllegalExternalDefinition {
	public:
		ZKAPI DaedalusIllegalExternalReturnType(DaedalusSymbol const* sym, std::string&& provided);
	};

	/// \brief An exception thrown if one of the parameter types of a new external registration does not match
	///        the type defined in the script.
	class DaedalusIllegalExternalParameter final : public DaedalusIllegalExternalDefinition {
	public:
		ZKAPI DaedalusIllegalExternalParameter(DaedalusSymbol const* sym, std::string&& provided, std::uint8_t i);
	};

	class DaedalusVmException final : public DaedalusScriptError {
		using DaedalusScriptError::DaedalusScriptError;
	};

	/// \brief An exception handling strategy given by VM exception handlers.
	enum class DaedalusVmExceptionStrategy {
		CONTINUE = 0, ///< Continue with execution of the script.
		RETURN = 1,   ///< Return from the current subroutine.
		FAIL = 2,     ///< Re-throw the exception and fail.

		// Deprecated entries.
		continue_ ZKREM("renamed to DaedalusVmExceptionStrategy::CONTINUE") = CONTINUE,
		return_ ZKREM("renamed to DaedalusVmExceptionStrategy::RETURN") = RETURN,
		fail_ ZKREM("renamed to DaedalusVmExceptionStrategy::FAIL") = FAIL,
	};

	/// \brief A stack frame in the VM.
	struct DaedalusStackFrame {
		std::shared_ptr<DaedalusInstance> context;
		bool reference;
		std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>> value;
		uint16_t index {0};
	};

	/// \brief A call stack frame in the VM.
	struct DaedalusCallStackFrame {
		DaedalusSymbol const* function;
		std::uint32_t program_counter;
		std::uint32_t stack_ptr;
		std::shared_ptr<DaedalusInstance> context;
	};

	namespace DaedalusVmExecutionFlag {
		static constexpr std::uint8_t NONE = 0;
		static constexpr std::uint8_t ALLOW_NULL_INSTANCE_ACCESS = 1 << 1;
		static constexpr std::uint8_t IGNORE_CONST_SPECIFIER = 1 << 2;

		// Deprecated entries.
		ZKREM("renamed to DaedalusVmExecutionFlag::NONE") static constexpr std::uint8_t none = NONE;

		ZKREM("renamed to DaedalusVmExecutionFlag::ALLOW_NULL_INSTANCE_ACCESS")
		static constexpr std::uint8_t vm_allow_null_instance_access = ALLOW_NULL_INSTANCE_ACCESS;

		ZKREM("renamed to DaedalusVmExecutionFlag::IGNORE_CONST_SPECIFIER")
		static constexpr std::uint8_t vm_ignore_const_specifier = IGNORE_CONST_SPECIFIER;
	} // namespace DaedalusVmExecutionFlag

	class DaedalusVm : public DaedalusScript {
	public:
		static constexpr auto stack_size = 2048;

		/// \brief Creates a DaedalusVM DaedalusInstance for the given script.
		/// \param scr The script to load into the VM.
		ZKAPI explicit DaedalusVm(DaedalusScript&& scr, uint8_t flags = DaedalusVmExecutionFlag::NONE);

		/// \brief Calls a function by it's name.
		/// \tparam P The types for the argument values.
		/// \param sym The name of the function to call.
		/// \param args The arguments for the function call.
		template <typename R = IgnoreReturnValue, typename... P>
		R call_function(std::string_view sym, P... args) {
			return call_function<R, P...>(find_symbol_by_name(sym), args...);
		}

		/// \brief Calls a function by it's symbol.
		/// \tparam P The types for the argument values.
		/// \param sym The symbol of the function to call.
		/// \param args The arguments for the function call.
		template <typename R = IgnoreReturnValue, typename... P>
		R call_function(DaedalusSymbol const* sym, P... args) {
			if (sym == nullptr) {
				throw DaedalusVmException {"Cannot call function: not found"};
			}

			if (sym->type() != DaedalusDataType::FUNCTION) {
				throw DaedalusVmException {"Cannot call " + sym->name() + ": not a function"};
			}

			std::vector<DaedalusSymbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P)) {
				throw DaedalusVmException {"too many arguments provided for " + sym->name() + ": given " +
				                           std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};
			}

			if (params.size() > sizeof...(P)) {
				throw DaedalusVmException {"not enough arguments provided for " + sym->name() + ": given " +
				                           std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};
			}

			if constexpr (!std::is_same_v<R, IgnoreReturnValue>) {
				check_call_return_type<R>(sym);
			}

			if constexpr (sizeof...(P) > 0) {
				push_call_parameters<0, P...>(params, args...);
			}

			unsafe_call(sym);

			if constexpr (std::is_same_v<R, IgnoreReturnValue>) {
				this->unsafe_clear_stack();
				return {};
			} else if constexpr (!std::is_same_v<R, void>) {
				auto ret = pop_call_return_value<R>();

				this->unsafe_clear_stack();
				return ret;
			} else {
				this->unsafe_clear_stack();
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
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, std::shared_ptr<_instance_t>>
		init_instance(std::string_view name) {
			return init_instance<_instance_t>(find_symbol_by_name(name));
		}

		/// \brief Initializes an instance with the given type into \p instance
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param instance The instance to initialize.
		/// \param name The name of the instance to initialize (ie. 'STT_309_WHISTLER')
		template <typename _instance_t>
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, void>
		init_instance(std::shared_ptr<_instance_t> const& instance, std::string_view name) {
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
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, std::shared_ptr<_instance_t>>
		init_instance(DaedalusSymbol* sym) {
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
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, void>
		init_instance(std::shared_ptr<_instance_t> const& instance, DaedalusSymbol* sym) {
			// Perform initial instance setup
			this->allocate_instance(instance, sym);

			// set the proper instances
			auto old_instance = _m_instance;
			auto old_self_instance = _m_self_sym != nullptr ? _m_self_sym->get_instance() : nullptr;
			_m_instance = instance;

			if (_m_self_sym) _m_self_sym->set_instance(_m_instance);

			unsafe_call(sym);

			// reset the VM state
			_m_instance = old_instance;
			if (_m_self_sym) _m_self_sym->set_instance(old_self_instance);
		}

		std::shared_ptr<DaedalusInstance> init_opaque_instance(DaedalusSymbol* sym);

		/// \brief Allocates an instance with the given type and name and returns it.
		///
		/// In contrast to #init_instance, this function will only create an instance of _instance_t and assign
		/// required internal state. The script function to initialize the instance is not called.
		///
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param name The name of the instance to initialize (ie. 'STT_309_WHISTLER')
		/// \return The initialized instance.
		template <typename _instance_t>
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, std::shared_ptr<_instance_t>>
		allocate_instance(std::string_view name) {
			return allocate_instance<_instance_t>(find_symbol_by_name(name));
		}

		/// \brief Allocates an instance with the given type into \p instance
		///
		/// In contrast to #init_instance, this function will only create an instance of _instance_t and assign
		/// required internal state. The script function to initialize the instance is not called.
		///
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param instance The instance to initialize.
		/// \param name The name of the instance to initialize (ie. 'STT_309_WHISTLER')
		template <typename _instance_t>
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, void>
		allocate_instance(std::shared_ptr<_instance_t> const& instance, std::string_view name) {
			allocate_instance<_instance_t>(instance, find_symbol_by_name(name));
		}

		/// \brief Allocates an instance with the given and returns it.
		///
		/// In contrast to #init_instance, this function will only create an instance of _instance_t and assign
		/// required internal state. The script function to initialize the instance is not called.
		///
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param sym The symbol to initialize.
		/// \return The initialized instance.
		template <typename _instance_t>
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, std::shared_ptr<_instance_t>>
		allocate_instance(DaedalusSymbol* sym) {
			// create the instance
			auto inst = std::make_shared<_instance_t>();
			allocate_instance(inst, sym);
			return inst;
		}

		/// \brief Allocates an instance with the given type into \p instance
		///
		/// In contrast to #init_instance, this function will only create an instance of _instance_t and assign
		/// required internal state. The script function to initialize the instance is not called.
		///
		/// \tparam _instance_t The type of the instance to initialize (ie. C_NPC).
		/// \param instance The instance to initialize.
		/// \param sym The symbol to initialize.
		template <typename _instance_t>
		std::enable_if_t<std::is_base_of_v<DaedalusInstance, _instance_t>, void>
		allocate_instance(std::shared_ptr<_instance_t> const& instance, DaedalusSymbol* sym) {
			if (sym == nullptr) {
				throw DaedalusVmException {"Cannot init instance: not found"};
			}
			if (sym->type() != DaedalusDataType::INSTANCE) {
				throw DaedalusVmException {"Cannot init " + sym->name() + ": not an instance"};
			}

			// check that the parent class is registered for the given instance type
			auto* parent = find_symbol_by_index(sym->parent());

			if (parent == nullptr) {
				// We're probably trying to initialize $INSTANCE_HELP which is not permitted
				throw DaedalusVmException {"Cannot init " + sym->name() +
				                           ": parent class not found (did you try to initialize $INSTANCE_HELP?)"};
			}

			while (parent->type() != DaedalusDataType::CLASS) {
				parent = find_symbol_by_index(parent->parent());
			}

			if (parent->registered_to() != typeid(_instance_t)) {
				throw DaedalusVmException {"Cannot init " + sym->name() +
				                           ": parent class is not registered or is "
				                           "registered to a different instance class"};
			}

			instance->_m_symbol_index = sym->index();
			instance->_m_type = &typeid(_instance_t);
			sym->set_instance(instance);
		}

		ZKAPI void push_int(std::int32_t value);
		ZKAPI void push_float(float value);
		ZKAPI void push_instance(std::shared_ptr<DaedalusInstance> value);
		ZKAPI void push_reference(DaedalusSymbol* value, std::uint8_t index = 0);
		ZKAPI void push_string(std::string_view value);

		[[nodiscard]] ZKAPI std::int32_t pop_int();
		[[nodiscard]] ZKAPI float pop_float();
		[[nodiscard]] ZKAPI std::shared_ptr<DaedalusInstance> pop_instance();
		[[nodiscard]] ZKAPI std::string const& pop_string();
		[[nodiscard]] ZKAPI std::tuple<DaedalusSymbol*, std::uint8_t, std::shared_ptr<DaedalusInstance>>
		pop_reference();

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
		/// 			<td><tt>std::shared_ptr&lt;{instance}&gt;|{instance}*</tt></td>
		/// 			<td>where {instance} is the type of instance(ie.C_NPC)[<sup>1</sup>]</td>
		/// 		</tr>
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
		/// [<sup>1</sup>] instances in the C++-World have to inherit from zenkit::DaedalusInstance.
		/// </p>
		///
		/// \tparam R The return type of the external.
		/// \tparam P The parameters types of the external.
		/// \param name The name of the external to register.
		/// \param callback The C++ function to register as the external.
		/// \throws DaedalusIllegalExternalReturnType if the return type of the C++ function does not match it's
		///                                definition in the script.
		/// \throws DaedalusIllegalExternalParameter if a parameter type of the C++ function does not match it's
		///                                definition in the script.
		/// \throws illegal_external if The number of parameters of the definition and callback is not the same.
		/// \throws runtime_error if any other error occurs.
		template <typename R, typename... P>
		void register_external(std::string_view name, std::function<R(P...)> const& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr) return;

			if (!sym->is_external()) throw DaedalusVmException {"symbol is not external"};

			if constexpr (!std::is_same_v<void, R>) {
				if (!sym->has_return()) throw DaedalusIllegalExternalReturnType(sym, "<non-void>");
				if constexpr (is_instance_ptr_v<R>) {
					if (sym->rtype() != DaedalusDataType::INSTANCE)
						throw DaedalusIllegalExternalReturnType(sym, "instance");
				} else if constexpr (std::is_floating_point_v<R>) {
					if (sym->rtype() != DaedalusDataType::FLOAT) throw DaedalusIllegalExternalReturnType(sym, "float");
				} else if constexpr (std::is_convertible_v<int32_t, R>) {
					if (sym->rtype() != DaedalusDataType::INT) throw DaedalusIllegalExternalReturnType(sym, "int");
				} else if constexpr (std::is_convertible_v<std::string, R>) {
					if (sym->rtype() != DaedalusDataType::STRING)
						throw DaedalusIllegalExternalReturnType(sym, "string");
				} else {
					throw DaedalusVmException {"unsupported return type"};
				}
			} else {
				if (sym->has_return()) throw DaedalusIllegalExternalReturnType(sym, "void");
			}

			std::vector<DaedalusSymbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P))
				throw DaedalusIllegalExternalDefinition {sym,
				                                         "too many arguments declared for external " + sym->name() +
				                                             ": declared " + std::to_string(sizeof...(P)) +
				                                             " expected " + std::to_string(params.size())};

			if (params.size() > sizeof...(P))
				throw DaedalusIllegalExternalDefinition {sym,
				                                         "not enough arguments declared for external " + sym->name() +
				                                             ": declared " + std::to_string(sizeof...(P)) +
				                                             " expected " + std::to_string(params.size())};

			if constexpr (sizeof...(P) > 0) {
				check_external_params<0, P...>(params);
			}

			// *evil template hacking ensues*
			_m_externals[sym] = [callback](DaedalusVm& machine) {
				if constexpr (std::is_same_v<void, R>) {
					if constexpr (sizeof...(P) > 0) {
						auto v = machine.pop_values_for_external<P...>();
						std::apply(callback, v);
					} else {
						callback();
					}
				} else {
					if constexpr (sizeof...(P) > 0) {
						machine.push_value_from_external(std::apply(callback, machine.pop_values_for_external<P...>()));
					} else {
						machine.push_value_from_external(callback());
					}
				}
			};
		}

		/// \brief Registers an external function.
		///
		/// Same as #register_external(std::string_view, std::function). This method is here to bypass a template
		/// deduction failure when passing lambdas directly.
		///
		/// \tparam T The type of external function to register.
		/// \param name The name of the external to register.
		/// \param cb The callback function to register.
		/// \see #register_external(std::string_view, std::function)
		template <typename T>
		void register_external(std::string_view name, T const& cb) {
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
		/// \see #register_external(std::string_view, std::function)
		/// \todo Deduplicate source code!
		template <typename R, typename... P>
		void override_function(std::string_view name, std::function<R(P...)> const& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr) throw DaedalusVmException {"symbol not found"};
			if (sym->is_external()) throw DaedalusVmException {"symbol is already an external"};

			if constexpr (!std::is_same_v<void, R>) {
				if (!sym->has_return()) throw DaedalusIllegalExternalReturnType(sym, "<non-void>");
				if constexpr (is_instance_ptr_v<R>) {
					if (sym->rtype() != DaedalusDataType::INSTANCE)
						throw DaedalusIllegalExternalReturnType(sym, "instance");
				} else if constexpr (std::is_floating_point_v<R>) {
					if (sym->rtype() != DaedalusDataType::FLOAT) throw DaedalusIllegalExternalReturnType(sym, "float");
				} else if constexpr (std::is_convertible_v<int32_t, R>) {
					if (sym->rtype() != DaedalusDataType::INT) throw DaedalusIllegalExternalReturnType(sym, "int");
				} else if constexpr (std::is_convertible_v<std::string, R>) {
					if (sym->rtype() != DaedalusDataType::STRING)
						throw DaedalusIllegalExternalReturnType(sym, "string");
				} else {
					throw DaedalusVmException {"unsupported return type"};
				}
			} else {
				if (sym->has_return()) throw DaedalusIllegalExternalReturnType(sym, "void");
			}

			std::vector<DaedalusSymbol*> params = find_parameters_for_function(sym);
			if (params.size() < sizeof...(P))
				throw DaedalusIllegalExternalDefinition {
				    sym,
				    "too many arguments declared for function override " + sym->name() + ": declared " +
				        std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};

			if (params.size() > sizeof...(P))
				throw DaedalusIllegalExternalDefinition {
				    sym,
				    "not enough arguments declared for function override " + sym->name() + ": declared " +
				        std::to_string(sizeof...(P)) + " expected " + std::to_string(params.size())};

			if constexpr (sizeof...(P) > 0) {
				check_external_params<0, P...>(params);
			}

			// *evil template hacking ensues*
			_m_function_overrides[sym->address()] = [callback, sym](DaedalusVm& machine) {
				machine.push_call(sym);
				if constexpr (std::is_same_v<void, R>) {
					if constexpr (sizeof...(P) > 0) {
						auto v = machine.pop_values_for_external<P...>();
						std::apply(callback, v);
					} else {
						callback();
					}
				} else {
					if constexpr (sizeof...(P) > 0) {
						machine.push_value_from_external(std::apply(callback, machine.pop_values_for_external<P...>()));
					} else {
						machine.push_value_from_external(callback());
					}
				}
				machine.pop_call();
			};
		}

		/// \brief Overrides a function in Daedalus code with an external naked call.
		///
		/// Whenever the function with the given name would be called from within Daedalus code, redirect the call
		/// to the given external callback handler instead.
		///
		/// \param name The name of the function to override.
		/// \param callback The C++ function to register as the external.
		void override_function(std::string_view name, std::function<DaedalusNakedCall(DaedalusVm&)> const& callback) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr) throw DaedalusVmException {"symbol not found"};
			if (sym->is_external()) throw DaedalusVmException {"symbol is already an external"};

			_m_function_overrides[sym->address()] = [callback](DaedalusVm& machine) { callback(machine); };
		}

		/// \brief Overrides a function in Daedalus code with an external definition.
		///
		/// Same as #override_function(std::string_view, std::function). This method is here to bypass a template
		/// deduction failure when passing lambdas directly.
		///
		/// \tparam T The type of external function to register.
		/// \param name The name of the function to override.
		/// \param cb The callback function to register.
		/// \see #override_function(std::string_view, std::function)
		template <typename T>
		void override_function(std::string_view name, T const& cb) {
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
		///                 external.
		ZKAPI void register_default_external(std::function<void(std::string_view)> const& callback);

		/// \brief Registers a function to be called when the script tries to call an external which has not been
		///        registered.
		///
		/// Before the callback is invoked, the VM makes sure that all parameters for the external are popped off
		/// the stack and a default return value (if needed) is pushed onto the stack. This prevents stack
		/// underflows.
		///
		/// \param callback The function to call. The one parameter of the function is the symbol of the unresolved
		///                 external.
		ZKAPI void register_default_external(std::function<void(DaedalusSymbol const&)> const& callback);

		ZKAPI void register_default_external_custom(std::function<void(DaedalusVm&, DaedalusSymbol&)> const& callback);

		ZKAPI void register_access_trap(std::function<void(DaedalusSymbol&)> const& callback);

		/// \brief Registers a function to be called when script execution fails.
		///
		/// A variety of exceptions can occur within the VM while executing. The function passed to this handler can
		/// inspect and alter the VMs state to try to correct the error.
		///
		/// \param callback The function to call. It receives a reference to the VM and the exception that occurred.
		///                 If the function returns `true` the error is assumed to have been handled and execution will
		///                 continue as normal. If `false` is returned, the VM will re-raise the exception and thus,
		///                 halt execution.
		ZKAPI void register_exception_handler(
		    std::function<DaedalusVmExceptionStrategy(DaedalusVm&,
		                                              DaedalusScriptError const&,
		                                              DaedalusInstruction const&)> const& callback);

		/// \brief Calls the given symbol as a function.
		///
		/// Automatically pushes a call stack frame. If the function has parameters and/or a return value,
		/// the caller is required to deal with them appropriately.
		///
		/// \param sym The symbol to unsafe_call.
		ZKAPI void unsafe_call(DaedalusSymbol const* sym);
		ZKAPI void unsafe_jump(uint32_t address);
		ZKAPI std::shared_ptr<DaedalusInstance> unsafe_get_gi();
		ZKAPI void unsafe_set_gi(std::shared_ptr<DaedalusInstance> i);
		ZKAPI void unsafe_clear_stack();

		/// \return the symbol referring to the global <tt>var C_NPC self</tt>.
		[[nodiscard]] ZKAPI DaedalusSymbol* global_self() {
			return _m_self_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC other</tt>.
		[[nodiscard]] ZKAPI DaedalusSymbol* global_other() {
			return _m_other_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC victim</tt>.
		[[nodiscard]] ZKAPI DaedalusSymbol* global_victim() {
			return _m_victim_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC hero</tt>.
		[[nodiscard]] ZKAPI DaedalusSymbol* global_hero() {
			return _m_hero_sym;
		}

		/// \return the symbol referring to the global <tt>var C_NPC item</tt>.
		[[nodiscard]] ZKAPI DaedalusSymbol* global_item() {
			return _m_item_sym;
		}

		/// \brief Prints the contents of the function call stack and the VMs stack to stderr.
		ZKAPI void print_stack_trace() const;

		/// \return The current program counter (or instruction index) the VM is at.
		[[nodiscard]] ZKAPI uint32_t pc() const noexcept {
			return _m_pc;
		}

		[[nodiscard]] ZKAPI std::int32_t
		get_int(std::shared_ptr<DaedalusInstance> const& context,
		        std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>> const& value,
		        uint16_t index) const;
		[[nodiscard]] ZKAPI float
		get_float(std::shared_ptr<DaedalusInstance> const& context,
		          std::variant<int32_t, float, DaedalusSymbol*, std::shared_ptr<DaedalusInstance>> const& value,
		          uint16_t index) const;

		ZKAPI void set_int(std::shared_ptr<DaedalusInstance> const& context,
		                   DaedalusSymbol* ref,
		                   uint16_t index,
		                   std::int32_t value);
		ZKAPI void
		set_float(std::shared_ptr<DaedalusInstance> const& context, DaedalusSymbol* ref, uint16_t index, float value);
		ZKAPI void set_string(std::shared_ptr<DaedalusInstance> const& context,
		                      DaedalusSymbol* ref,
		                      uint16_t index,
		                      std::string_view value);

	protected:
		/// \brief Runs the instruction at the current program counter and advances it properly.
		/// \return false, the instruction executed was a op_return instruction, otherwise true.
		ZKINT bool exec();

		/// \brief Validates the given address and jumps to it (sets the program counter).
		/// \param address The address to jump to.
		ZKINT void jump(std::uint32_t address);

		/// \brief Pushes a call stack frame onto the call stack.
		///
		/// This method also pushes the current state of the interpreter to be restored as soon
		/// as #pop_call is invoked.
		///
		/// \param sym The symbol referring to the function called.
		ZKINT void push_call(DaedalusSymbol const* sym);

		/// \brief Pops a call stack from from the call stack.
		///
		/// This method restores the interpreter's state to before the function which the
		/// call stack entry refers to was called.
		ZKINT void pop_call();

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
		/// \throws DaedalusIllegalExternalParameter If the types don't match.
		/// \note Requires that sizeof...(Px) + 1 == defined.size().
		template <int32_t i, typename P, typename... Px>
		void check_external_params(std::vector<DaedalusSymbol*> const& defined) {
			if constexpr (is_instance_ptr_v<P> || std::is_same_v<DaedalusSymbol*, P> || is_raw_instance_ptr_v<P>) {
				if (defined[i]->type() != DaedalusDataType::INSTANCE)
					throw DaedalusIllegalExternalParameter(defined[i], "instance", i + 1);
			} else if constexpr (std::is_same_v<float, P>) {
				if (defined[i]->type() != DaedalusDataType::FLOAT)
					throw DaedalusIllegalExternalParameter(defined[i], "float", i + 1);
			} else if constexpr (std::is_same_v<int32_t, P> || std::is_same_v<bool, P> ||
			                     std::is_same_v<DaedalusFunction, P>) {
				if (defined[i]->type() != DaedalusDataType::INT && defined[i]->type() != DaedalusDataType::FUNCTION)
					throw DaedalusIllegalExternalParameter(defined[i], "int/func", i + 1);
			} else if constexpr (std::is_same_v<std::string_view, P>) {
				if (defined[i]->type() != DaedalusDataType::STRING)
					throw DaedalusIllegalExternalParameter(defined[i], "string", i + 1);
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
		/// \tparam T The type of the value to pop (one of std::shared_ptr<? extends DaedalusInstance>, float, int32_t,
		///            DaedalusSymbol*, std::string_view)
		/// \return The value popped.
		template <typename T>
		std::enable_if_t<is_instance_ptr_v<T> || std::is_same_v<T, float> || std::is_same_v<T, std::int32_t> ||
		                     std::is_same_v<T, bool> || std::is_same_v<T, std::string_view> ||
		                     std::is_same_v<T, DaedalusSymbol*> || is_raw_instance_ptr_v<T> ||
		                     std::is_same_v<T, DaedalusFunction>,
		                 T>
		pop_value_for_external() {
			if constexpr (is_instance_ptr_v<T>) {
				auto r = pop_instance();

				if (r != nullptr && !std::is_same_v<T, std::shared_ptr<DaedalusInstance>>) {
					auto& expected = typeid(typename T::element_type);

					if (!r->_m_type) {
						throw DaedalusVmException {"Popping instance of unregistered type: " +
						                           std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}

					if (*r->_m_type != expected) {
						throw DaedalusVmException {"Popping instance of wrong type: " +
						                           std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}
				}

				return std::static_pointer_cast<typename T::element_type>(r);
			} else if constexpr (is_raw_instance_ptr_v<T>) {
				auto r = pop_instance();

				if (r != nullptr && !std::is_same_v<T, DaedalusInstance*>) {
					auto& expected = typeid(std::remove_pointer_t<T>);

					if (!r->_m_type) {
						throw DaedalusVmException {"Popping instance of unregistered type: " +
						                           std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}

					if (*r->_m_type != expected) {
						throw DaedalusVmException {"Popping instance of wrong type: " +
						                           std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}
				}

				return reinterpret_cast<T>(r.get());
			} else if constexpr (std::is_same_v<float, T>) {
				return pop_float();
			} else if constexpr (std::is_same_v<std::int32_t, T>) {
				return pop_int();
			} else if constexpr (std::is_same_v<bool, T>) {
				return pop_int() != 0;
			} else if constexpr (std::is_same_v<std::string_view, T>) {
				return pop_string();
			} else if constexpr (std::is_same_v<DaedalusSymbol*, T>) {
				return std::get<0>(pop_reference());
			} else if constexpr (std::is_same_v<DaedalusFunction, T>) {
				auto symbol_id = static_cast<uint32_t>(pop_int());

				auto* sym = find_symbol_by_index(symbol_id);
				while (sym != nullptr && sym->type() == DaedalusDataType::FUNCTION && !sym->is_const()) {
					symbol_id = static_cast<uint32_t>(sym->get_int());
					sym = find_symbol_by_index(symbol_id);
				}

				if (sym != nullptr && sym->type() != DaedalusDataType::FUNCTION) {
					return DaedalusFunction {nullptr};
				}

				return DaedalusFunction {sym};
			} else {
				throw DaedalusVmException {"pop: unsupported stack frame type"};
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
		std::enable_if_t<is_instance_ptr_v<T> || std::is_convertible_v<T, float> ||
		                     std::is_convertible_v<T, std::int32_t> || std::is_same_v<T, std::string> ||
		                     std::is_same_v<T, std::string_view> || std::is_same_v<T, DaedalusFunction>,
		                 void>
		push_value_from_external(T v) { // clang-format on
			if constexpr (is_instance_ptr_v<T>) {
				push_instance(std::static_pointer_cast<DaedalusInstance>(v));
			} else if constexpr (std::is_floating_point_v<T>) {
				push_float(static_cast<float>(v));
			} else if constexpr (std::is_convertible_v<std::int32_t, T>) {
				push_int(static_cast<std::int32_t>(v));
			} else if constexpr (std::is_same_v<DaedalusFunction, T>) {
				push_int(static_cast<std::int32_t>(v.value->index()));
			} else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
				push_string(v);
			} else {
				throw DaedalusVmException {"push: unsupported stack frame type"};
			}
		}

		/// \brief The given types in reverse order using #pop_value_for_external.
		///
		/// This is used to pop values from the stack in the correct order for calling an external function
		/// since the latest value on the stack is the last parameter to the function call.
		///
		/// \tparam P A value type to pop from the stack (one of std::shared_ptr<? extends instance>, float,
		/// int32_t,
		///           DaedalusSymbol*, std::string_view)
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
		std::enable_if_t<is_instance_ptr_v<R> || std::is_same_v<R, float> || std::is_same_v<R, std::int32_t> ||
		                     std::is_same_v<R, DaedalusFunction> || std::is_same_v<R, std::string> ||
		                     std::is_same_v<R, void>,
		                 void>
		check_call_return_type(DaedalusSymbol const* sym) {
			if constexpr (is_instance_ptr_v<R>) {
				if (sym->rtype() != DaedalusDataType::INSTANCE)
					throw DaedalusVmException {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<float, R>) {
				if (sym->rtype() != DaedalusDataType::FLOAT)
					throw DaedalusVmException {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<int32_t, R> || std::is_same_v<DaedalusFunction, R>) {
				if (sym->rtype() != DaedalusDataType::INT && sym->rtype() != DaedalusDataType::FUNCTION)
					throw DaedalusVmException {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<std::string, R>) {
				if (sym->rtype() != DaedalusDataType::STRING)
					throw DaedalusVmException {"invalid return type for function " + sym->name()};
			} else if constexpr (std::is_same_v<void, R>) {
				if (sym->rtype() != DaedalusDataType::VOID)
					throw DaedalusVmException {"invalid return type for function " + sym->name()};
			}
		}

		template <int32_t i, typename P, typename... Px>
		std::enable_if_t<is_instance_ptr_v<P> || std::is_same_v<std::remove_reference_t<P>, float> ||
		                     std::is_same_v<std::remove_reference_t<P>, std::int32_t> ||
		                     std::is_same_v<std::remove_reference_t<P>, bool> ||
		                     std::is_same_v<std::remove_reference_t<P>, std::string_view> ||
		                     std::is_same_v<std::remove_reference_t<P>, DaedalusSymbol*>,
		                 void>
		push_call_parameters(std::vector<DaedalusSymbol*> const& defined, P value, Px... more) { // clang-format on
			if constexpr (is_instance_ptr_v<P> || std::is_same_v<DaedalusSymbol*, P>) {
				if (defined[i]->type() != DaedalusDataType::INSTANCE)
					throw DaedalusIllegalExternalParameter(defined[i], "instance", i + 1);

				push_instance(value);
			} else if constexpr (std::is_same_v<float, P>) {
				if (defined[i]->type() != DaedalusDataType::FLOAT)
					throw DaedalusIllegalExternalParameter(defined[i], "float", i + 1);

				push_float(value);
			} else if constexpr (std::is_same_v<int32_t, P> || std::is_same_v<bool, P>) {
				if (defined[i]->type() != DaedalusDataType::INT && defined[i]->type() != DaedalusDataType::FUNCTION)
					throw DaedalusIllegalExternalParameter(defined[i], "int", i + 1);

				push_int(value);
			} else if constexpr (std::is_same_v<std::string_view, P>) {
				if (defined[i]->type() != DaedalusDataType::STRING)
					throw DaedalusIllegalExternalParameter(defined[i], "string", i + 1);

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
						throw DaedalusVmException {"Popping instance of unregistered type: " +
						                           std::string {r->_m_type->name()} + ", expected " + expected.name()};
					}

					if (*r->_m_type != expected) {
						throw DaedalusVmException {"Popping instance of wrong type: " +
						                           std::string {r->_m_type->name()} + ", expected " + expected.name()};
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
		std::array<DaedalusStackFrame, stack_size> _m_stack;
		uint16_t _m_stack_ptr {0};

		std::stack<DaedalusCallStackFrame> _m_call_stack;
		std::unordered_map<DaedalusSymbol*, std::function<void(DaedalusVm&)>> _m_externals;
		std::unordered_map<uint32_t, std::function<void(DaedalusVm&)>> _m_function_overrides;
		std::optional<std::function<void(DaedalusVm&, DaedalusSymbol&)>> _m_default_external {std::nullopt};
		std::function<void(DaedalusSymbol&)> _m_access_trap;
		std::optional<std::function<
		    DaedalusVmExceptionStrategy(DaedalusVm&, DaedalusScriptError const&, DaedalusInstruction const&)>>
		    _m_exception_handler {std::nullopt};

		DaedalusSymbol* _m_self_sym;
		DaedalusSymbol* _m_other_sym;
		DaedalusSymbol* _m_victim_sym;
		DaedalusSymbol* _m_hero_sym;
		DaedalusSymbol* _m_item_sym;

		DaedalusSymbol* _m_temporary_strings;

		std::shared_ptr<DaedalusInstance> _m_instance;
		std::uint32_t _m_pc {0};
		std::uint8_t _m_flags {DaedalusVmExecutionFlag::NONE};
	};

	/// \brief A VM exception handler which handles some and pretends to handle other VM exceptions.
	///
	/// Some exceptions are just ignored and some are handled properly.
	///
	/// \param v The VM the exception occurred in.
	/// \param exc The exception being handled.
	/// \param instr The instruction being executed.
	/// \return DaedalusVmExceptionStrategy::continue_
	ZKAPI DaedalusVmExceptionStrategy lenient_vm_exception_handler(DaedalusVm& v,
	                                                               DaedalusScriptError const& exc,
	                                                               DaedalusInstruction const& instr);
} // namespace zenkit
