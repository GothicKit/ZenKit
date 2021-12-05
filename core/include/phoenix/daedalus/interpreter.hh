// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/daedalus/script.hh>

#include <functional>
#include <optional>
#include <stack>
#include <tuple>
#include <variant>

namespace phoenix {
	template <typename T>
	struct is_instance_ptr : std::false_type {};

	template <typename T>// clang-format off
	requires (std::derived_from<T, instance>)
	struct is_instance_ptr<std::shared_ptr<T>> : std::true_type {// clang-format on
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
		illegal_external_param(const symbol* sym, std::string_view provided, u8 i);
	};

	struct daedalus_stack_frame {
		bool reference;
		std::variant<int32_t, float, symbol*, std::shared_ptr<instance>> value;
		uint16_t index {0};
	};

	struct daedalus_call_stack_frame {
		const symbol* function;
		u32 program_counter;
		u32 dynamic_string_index;
	};

	class daedalus_interpreter {
	public:
		/**
		 * @brief Creates a DaedalusVM instance for the given script.
		 * @param[in, out] scr The script to load into the VM.
		 */
		explicit daedalus_interpreter(script& scr);

		void call_function(const std::string& name);

		void call_function(const symbol* sym);

		template <class _instance_t>
		std::shared_ptr<_instance_t> init_instance(const std::string& name) {
			auto* sym = _m_script.find_symbol_by_name(name);
			if (sym == nullptr) { throw std::runtime_error {"Cannot call " + name + ": not found"}; }
			if (sym->type() != dt_instance) { throw std::runtime_error {"Cannot call " + sym->name() + ": not an instance"}; }

			auto inst = std::make_shared<_instance_t>(sym);

			// set the proper instances
			_m_instance = inst;
			sym->set_instance(_m_instance);

			if (_m_self_sym)
				_m_self_sym->set_instance(_m_instance);

			call(sym);
			return inst;
		}

		void push_int(s32 value);
		void push_float(f32 value);
		void push_instance(std::shared_ptr<instance> value);
		void push_reference(symbol* value, u8 index = 0);
		void push_string(const std::string& value);

		[[nodiscard]] s32 pop_int();
		[[nodiscard]] f32 pop_float();
		[[nodiscard]] std::shared_ptr<instance> pop_instance();
		[[nodiscard]] const std::string& pop_string();
		[[nodiscard]] std::tuple<symbol*, u8> pop_reference();

		template <typename R, typename... P>
		void register_external(const std::string& name, const std::function<R(P...)>& callback) {
			auto* sym = _m_script.find_symbol_by_name(name);
			if (sym == nullptr) throw std::runtime_error {"symbol not found"};
			if (!sym->is_external()) throw std::runtime_error {"symbol is not external"};

			if constexpr (!std::same_as<void, R>) {
				if (!sym->has_return()) throw illegal_external_rtype(sym, "<non-void>");
				if constexpr (is_instance_ptr<R>::value) {
					if (sym->rtype() != dt_instance) throw illegal_external_rtype(sym, "instance");
				} else if constexpr (std::floating_point<R>) {
					if (sym->rtype() != dt_float) throw illegal_external_rtype(sym, "float");
				} else if constexpr (std::convertible_to<int32_t, R>) {
					if (sym->rtype() != dt_integer) throw illegal_external_rtype(sym, "int");
				} else if constexpr (std::convertible_to<std::string, R>) {
					if (sym->rtype() != dt_string) throw illegal_external_rtype(sym, "string");
				} else {
					throw std::runtime_error {"unsupported return type"};
				}
			} else {
				if (sym->has_return()) throw illegal_external_rtype(sym, "void");
			}

			std::vector<symbol*> params = _m_script.find_parameters_for_function(sym);
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
			_m_externals[sym] = [callback](daedalus_interpreter& vm) {
				if constexpr (std::same_as<void, R>) {
					auto v = vm.pop_values_for_external<P...>();
					std::apply(callback, v);
				} else {
					vm.push_value_from_external(std::apply(callback, vm.pop_values_for_external<P...>()));
				}
			};
		}

		// Bypass for not being able to input a lambda directly :| Please don't hurt me okay?
		template <typename T>
		void register_external(const std::string& name, const T& cb) {
			register_external(name, std::function {cb});
		}

		script& loaded_script() const noexcept { return _m_script; }

	protected:
		void call(const symbol* sym);

		bool exec();

		void jump(u32 address);

		void push_call(const symbol* sym);

		void pop_call();

		template<int i, typename P, typename ... Px>
		void check_external_params(const std::vector<symbol*>& defined) {
			if constexpr (is_instance_ptr<P>::value || std::same_as<symbol*, P>) {
				if (defined[i]->type() != dt_instance) throw illegal_external_param(defined[i], "instance", i + 1);
			} else if constexpr (std::same_as<float, P>) {
				if (defined[i]->type() != dt_float) throw illegal_external_param(defined[i], "float", i + 1);
			} else if constexpr (std::same_as<int32_t, P>) {
				if (defined[i]->type() != dt_integer && defined[i]->type() != dt_function)
					throw illegal_external_param(defined[i], "int", i + 1);
			} else if constexpr (std::same_as<std::string_view, P>) {
				if (defined[i]->type() != dt_string) throw illegal_external_param(defined[i], "string", i + 1);
			}

			if constexpr (sizeof...(Px) > 0) {
				check_external_params<i + 1, Px...>(defined);
			}
		}

		template <typename T>// clang-format off
		requires (is_instance_ptr<T>::value || std::same_as<float, T> || std::same_as<s32, T> ||
		          std::same_as<std::string_view, T> || std::same_as<symbol*, T>)
		inline T pop_value_for_external() {// clang-format on
			if constexpr (is_instance_ptr<T>::value) {
				// TODO: Add check that is_instance_ptr<T>::instance_type is actually the correct type of the instance
				//  returned by pop_instance!
				return std::static_pointer_cast<typename is_instance_ptr<T>::instance_type>(pop_instance());
			} else if constexpr (std::same_as<float, T>) {
				return pop_float();
			} else if constexpr (std::same_as<s32, T>) {
				return pop_int();
			} else if constexpr (std::same_as<std::string_view, T>) {
				return pop_string();
			} else if constexpr (std::same_as<symbol*, T>) {
				return std::get<0>(pop_reference());
			} else {
				throw std::runtime_error {"pop: unsupported stack frame type "};
			}
		}

		template <typename T>// clang-format off
		requires (std::floating_point<T> || std::convertible_to<s32, T> || std::convertible_to<std::string, T> ||
		          is_instance_ptr<T>::value)
		void push_value_from_external(T v) {// clang-format on
			if constexpr (is_instance_ptr<T>::value) {
				push_instance(std::static_pointer_cast<instance>(v));
			} else if constexpr (std::floating_point<T>) {
				push_float(static_cast<float>(v));
			} else if constexpr (std::convertible_to<s32, T>) {
				push_int(static_cast<s32>(v));
			} else if constexpr (std::convertible_to<std::string, T>) {
				push_string(v);
			} else {
				throw std::runtime_error {"push: unsupported stack frame type"};
			}
		}

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
		script& _m_script;
		std::stack<daedalus_stack_frame> _m_stack;
		std::stack<daedalus_call_stack_frame> _m_call_stack;
		std::unordered_map<symbol*, std::function<void(daedalus_interpreter&)>> _m_externals;

		symbol* _m_self_sym;
		std::shared_ptr<instance> _m_instance;
		u32 _m_pc {0};
		u32 _m_dynamic_string_index {0};

		void print_stack_trace();
	};
}// namespace phoenix
