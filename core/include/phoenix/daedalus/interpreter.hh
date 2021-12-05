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
			// TODO: this interface can be better
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
			if (!sym->is_extern()) throw std::runtime_error {"symbol is not external"};

			// Todo: This can probably be better ...
			if constexpr (std::convertible_to<std::shared_ptr<instance>, R>) {
				if (!sym->has_return() || sym->rtype() != dt_instance)
					throw std::runtime_error {"external " + std::string {name} + " has incorrect return type instance - expected: " + std::to_string(sym->rtype())};
			} else if constexpr (std::same_as<float, R>) {
				if (!sym->has_return() || sym->rtype() != dt_float)
					throw std::runtime_error {"external " + std::string {name} + " has incorrect return type float - expected: " + std::to_string(sym->rtype())};
			} else if constexpr (std::convertible_to<int32_t, R>) {
				if (!sym->has_return() || sym->rtype() != dt_integer)
					throw std::runtime_error {"external " + std::string {name} + " has incorrect return type int - expected: " + std::to_string(sym->rtype())};
			} else if constexpr (std::convertible_to<std::string, R>) {
				if (!sym->has_return() || sym->rtype() != dt_string)
					throw std::runtime_error {"external " + std::string {name} + " has incorrect return type string - expected: " + std::to_string(sym->rtype())};
			} else if constexpr (std::same_as<void, R>) {
				if (sym->has_return())
					throw std::runtime_error {"external " + std::string {name} + " has incorrect return type void - expected: " + std::to_string(sym->rtype())};
			} else {
				throw std::runtime_error {"unsupported return type"};
			}

			// TODO: Check parameter types!

			// *evil template hacking ensues*
			_m_externals[sym] = [callback](daedalus_interpreter& vm) {
				if constexpr (std::same_as<void, R>) {
					auto v = vm.pop_values_reverse<P...>();

					std::apply(callback, v);
				} else {
					vm.push_value(std::apply(callback, vm.pop_values_reverse<P...>()));
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

		template <typename T>// clang-format off
		requires (std::same_as<std::shared_ptr<instance>, T> || std::same_as<float, T> || std::same_as<s32, T> ||
		          std::same_as<std::string_view, T>)
		inline T pop_value() {// clang-format on
			if constexpr (std::same_as<std::shared_ptr<instance>, T>) {
				return pop_instance();
			} else if constexpr (std::same_as<float, T>) {
				return pop_float();
			} else if constexpr (std::same_as<s32, T>) {
				return pop_int();
			} else if constexpr (std::same_as<std::string_view, T>) {
				return pop_string();
			} else {
				throw std::runtime_error {"pop: unsupported stack frame type "};
			}
		}

		template <typename T>// clang-format off
		requires (std::floating_point<T> || std::convertible_to<s32, T> || std::convertible_to<std::string, T>)
		void push_value(T v) {// clang-format on
			if constexpr (std::floating_point<T>) {
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
		std::tuple<P, Px...> pop_values_reverse() {
			if constexpr (sizeof...(Px) > 0) {
				auto v = pop_values_reverse<Px...>();

				if constexpr (std::same_as<P, s32> || std::same_as<P, f32> || std::same_as<P, std::string_view>) {
					return std::tuple_cat(std::make_tuple(pop_value<P>()), std::move(v));
				} else {
					auto k = pop_value<std::shared_ptr<instance>>();
					return std::tuple_cat(std::make_tuple(reinterpret_cast<P&>(k)), std::move(v));
				}
			} else {
				if constexpr (std::same_as<P, s32> || std::same_as<P, f32> || std::same_as<P, std::string_view>) {
					return {pop_value<P>()};
				} else {
					auto k = pop_value<std::shared_ptr<instance>>();
					return {reinterpret_cast<P&>(k)};
				}
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
