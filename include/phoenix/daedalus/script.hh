// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/detail/buffer.hh>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#define unset 0xFF'FF'FF'FFU

namespace phoenix::daedalus {
	/**
	 * @brief Types of symbols
	 */
	enum datatype : std::uint32_t {
		dt_void = 0U,
		dt_float = 1U,
		dt_integer = 2U,
		dt_string = 3U,
		dt_class = 4U,
		dt_function = 5U,
		dt_prototype = 6U,
		dt_instance = 7U,
	};

	constexpr const char* const DAEDALUS_DATA_TYPE_NAMES[] =
	    {"void", "float", "int", "string", "class", "function", "prototype", "instance"};

	/**
	 * @brief Flags set on symbols.
	 */
	enum flag : std::uint32_t {
		sf_const = 1U << 0U,
		sf_return = 1U << 1U,
		sf_member = 1U << 2U,
		sf_external = 1U << 3U,
		sf_merged = 1U << 4U,
	};

	/**
	 * @brief All opcodes supported by the daedalus interpreter
	 */
	enum opcode : std::uint8_t {
		op_add = 0,               // a + b
		op_subtract = 1,          // a - b
		op_multiply = 2,          // a * b
		op_divide = 3,            // a / b
		op_modulo = 4,            // a % b
		op_bitor = 5,             // a | b
		op_bitand = 6,            // a & b
		op_less = 7,              // a < b
		op_greater = 8,           // a > b
		op_assign_int = 9,        // a = b
		op_or = 11,               // a || b
		op_and = 12,              // a && b
		op_shift_left = 13,       // a << b
		op_shift_right = 14,      // a >> b
		op_less_or_equal = 15,    // a <= b
		op_equal = 16,            // a == b
		op_not_equal = 17,        // a != b
		op_greater_or_equal = 18, // a >= b
		op_assign_add = 19,       // a += b (a = a + b)
		op_assign_subtract = 20,  // a -= b (a = a - b)
		op_assign_multiply = 21,  // a *= b (a = a * b)
		op_assign_divide = 22,    // a /= b (a = a / b)
		op_plus = 30,             // +a
		op_minus = 31,            // -a
		op_not = 32,              // !a
		op_complement = 33,       // ~a
		op_noop = 45,
		op_return = 60,
		op_call = 61,
		op_call_external = 62,
		op_push_int = 64,
		op_push_var = 65,
		op_push_instance = 67,
		op_assign_string = 70,
		op_assign_stringref = 71,
		op_assign_func = 72,
		op_assign_float = 73,
		op_assign_instance = 74,
		op_jump = 75,
		op_jump_if_zero = 76,
		op_set_instance = 80,
		op_push_array_var = 245, // EParOp_PushVar + EParOp_Array
	};

	class symbol;

	/**
	 * @brief Represents an object associated with an instance in the script.
	 *
	 * Every class defined in C++ that can be used as an instance has to inherit from this class.
	 */
	class instance {
	public:
		virtual ~instance() = default;

	private:
		friend class symbol;
		friend class script;
		friend class vm;

		uint32_t _m_symbol_index {unset};
		const std::type_info* _m_type {nullptr};
	};

	struct symbol_not_found : public std::runtime_error {
		explicit symbol_not_found(const std::string& name);
	};

	struct member_registration_error : public std::runtime_error {
		explicit member_registration_error(const symbol& sym, std::string_view message);
	};

	struct invalid_registration_datatype final : member_registration_error {
		explicit invalid_registration_datatype(const symbol& sym, const std::string& given);
	};

	/**
	 * @brief An exception thrown when the value of a symbol is illegally accessed.
	 */
	struct illegal_access : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	/**
	 * @brief An exception thrown when the type of a symbol does not match the type expected.
	 */
	struct illegal_type_access final : public illegal_access {
		illegal_type_access(const symbol& sym, datatype expected);
	};

	/**
	 * @brief An exception thrown when an out-of-bounds index is accessed.
	 */
	struct illegal_index_access final : public illegal_access {
		illegal_index_access(const symbol& sym, std::uint8_t index);
	};

	/**
	 * @brief An exception thrown when a constant symbol is accessed as mutable
	 */
	struct illegal_const_access final : public illegal_access {
		explicit illegal_const_access(const symbol& sym);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	struct illegal_instance_access final : public illegal_access {
		illegal_instance_access(const symbol& sym, std::uint32_t expected_parent);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	struct unbound_member_access final : public illegal_access {
		explicit unbound_member_access(const symbol& sym);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	struct no_context final : public illegal_access {
		explicit no_context(const symbol& sym);
	};

	struct illegal_context_type final : public illegal_access {
		illegal_context_type(const symbol& sym, const std::type_info& context_type);
	};

	/**
	 * @brief Represents a compiled daedalus symbol.
	 */
	class symbol final {
	public:
		/**
		 * @brief Parses a symbol from the given reader.
		 * @param[in,out] in The reader to read the symbol from.
		 * @return The symbol parsed.
		 */
		[[nodiscard]] static symbol parse(buffer& in);

		/**
		 * @brief Validates that the symbol is a string and retrieves it's value in the given context.
		 * @param index The index of the value to get.
		 * @param context An instance to use as context for getting member variables.
		 * @return The string associated with the symbol.
		 */
		[[nodiscard]] const std::string& get_string(std::uint8_t index = 0,
		                                            const std::shared_ptr<instance>& context = nullptr) const;

		/**
		 * @brief Validates that the symbol is a float and retrieves it's value in the given context.
		 * @param index The index of the value to get.
		 * @param context An instance to use as context for getting member variables.
		 * @return The float value associated with the symbol.
		 */
		[[nodiscard]] float get_float(std::uint8_t index = 0, const std::shared_ptr<instance>& context = nullptr) const;

		/**
		 * @brief Validates that the symbol is an int and retrieves it's value in the given context.
		 * @param index The index of the value to get.
		 * @param context An instance to use as context for getting member variables.
		 * @return The int value associated with the symbol.
		 */
		[[nodiscard]] std::int32_t get_int(std::uint8_t index = 0,
		                                   const std::shared_ptr<instance>& context = nullptr) const;

		/**
		 * @brief Validates that the symbol is an instance and retrieves it's value
		 * @return The instance associated with the symbol.
		 */
		[[nodiscard]] const std::shared_ptr<instance>& get_instance();

		// -=-= Value setters =-=- //

		/**
		 * @brief Validates that the symbol is a string and not constant and sets it's value in the given context.
		 * @param value The new value to set.
		 * @param index The index of the value to set
		 * @param context An instance to use as context for setting member variables.
		 */
		void set_string(const std::string& value,
		                std::uint8_t index = 0,
		                const std::shared_ptr<instance>& context = nullptr);

		/**
		 * @brief Validates that the symbol is a float and not constant and sets it's value in the given context.
		 * @param value The new value to set.
		 * @param index The index of the value to set
		 * @param context An instance to use as context for setting member variables.
		 */
		void set_float(float value, std::uint8_t index = 0, const std::shared_ptr<instance>& context = nullptr);

		/**
		 * @brief Validates that the symbol is an int and not constant and sets it's value in the given context.
		 * @param value The new value to set.
		 * @param index The index of the value to set
		 * @param context An instance to use as context for setting member variables.
		 */
		void set_int(std::int32_t value, std::uint8_t index = 0, const std::shared_ptr<instance>& context = nullptr);

		/**
		 * @brief Validates that the symbol is an instance and sets it's value
		 * @param inst The instance value to set
		 */
		void set_instance(const std::shared_ptr<instance>& inst);

		/**
		 * @brief Tests whether this symbol holds an instance of the given type.
		 * @tparam T The type of instance to check for.
		 * @return <tt>true</tt> if the symbol contains an instance of the given type, <tt>false</tt> if not.
		 */
		template <typename T> // clang-format off
		requires (std::derived_from<T, instance>)
		bool is_instance_of() { // clang-format on
			return type() == dt_instance && get_instance() != nullptr && get_instance()->_m_type == &typeid(T);
		}

		/**
		 * @brief Tests whether the symbol is a constant.
		 * @return `true` if the symbol is a constant, `false` if not.
		 */
		[[nodiscard]] inline bool is_const() const noexcept {
			return (_m_flags & sf_const) != 0;
		}
		/**
		 * @brief Tests whether the symbol is a member variable.
		 * @return `true` if the symbol is a member, `false` if not.
		 */
		[[nodiscard]] inline bool is_member() const noexcept {
			return (_m_flags & sf_member) != 0;
		}

		/**
		 * @brief Tests whether the symbol is an extern symbol.
		 * @return `true` if the symbol is an extern symbol, `false` if not.
		 */
		[[nodiscard]] inline bool is_external() const noexcept {
			return (_m_flags & sf_external) != 0;
		}

		/**
		 * @brief Tests whether the symbol is merged.
		 * @return `true` if the symbol is merged, `false` if not.
		 * @note It is currently not known what 'merged' means.
		 */
		[[nodiscard]] inline bool is_merged() const noexcept {
			return (_m_flags & sf_merged) != 0;
		}

		/**
		 * @brief brief Tests whether the symbol is a compiler-generated symbol
		 * @return return `true` if the symbol is generated, `false` if not.
		 */
		[[nodiscard]] inline bool is_generated() const noexcept {
			return _m_generated;
		}

		/**
		 * @brief brief Tests whether the symbol has a return value.
		 * @return return `true` if the symbol has a return value, `false` if not.
		 */
		[[nodiscard]] inline bool has_return() const noexcept {
			return (_m_flags & sf_return) != 0;
		}

		/**
		 * @return The name of the symbol.
		 */
		[[nodiscard]] inline const std::string& name() const noexcept {
			return _m_name;
		}

		/**
		 * @return The address of the symbol.
		 */
		[[nodiscard]] inline std::uint32_t address() const noexcept {
			return _m_address;
		}

		/**
		 * @return The index of the parent symbol or unset if the symbol does not have a parent.
		 */
		[[nodiscard]] inline std::uint32_t parent() const noexcept {
			return _m_parent;
		}

		/**
		 * @return The count of values stored in the symbol.
		 */
		[[nodiscard]] inline std::uint32_t count() const noexcept {
			return _m_count;
		}

		/**
		 * @return The type of the symbol.
		 */
		[[nodiscard]] inline datatype type() const noexcept {
			return _m_type;
		}

		/**
		 * @return The index of the symbol.
		 */
		[[nodiscard]] inline std::uint32_t index() const noexcept {
			return _m_index;
		}

		/**
		 * @return The return type of the symbol.
		 */
		[[nodiscard]] inline datatype rtype() const noexcept {
			return _m_return_type;
		}

		/**
		 * @return The index of the file the symbol was in.
		 */
		[[nodiscard]] inline std::uint32_t file_index() const noexcept {
			return _m_file_index;
		}

		/**
		 * @return The offset in bytes of a member from the start of the instance.
		 */
		[[nodiscard]] inline std::uint32_t offset_as_member() const noexcept {
			return _m_member_offset;
		}

		[[nodiscard]] inline std::uint32_t line_start() const noexcept {
			return _m_line_start;
		}
		[[nodiscard]] inline std::uint32_t line_count() const noexcept {
			return _m_line_count;
		}
		[[nodiscard]] inline std::uint32_t char_start() const noexcept {
			return _m_char_start;
		}
		[[nodiscard]] inline std::uint32_t char_count() const noexcept {
			return _m_char_count;
		}
		[[nodiscard]] inline std::uint32_t class_size() const noexcept {
			return _m_class_size;
		}

		[[nodiscard]] inline const std::type_info& registered_to() const noexcept {
			return *_m_registered_to;
		};

	protected:
		symbol() = default;

		template <typename T>
		const T* get_member_ptr(std::uint8_t index, const std::shared_ptr<instance>& context) const {
			if (!_m_registered_to)
				throw unbound_member_access(*this);
			if (*_m_registered_to != *context->_m_type)
				throw illegal_context_type {*this, *context->_m_type};

			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<const T*>(reinterpret_cast<const char*>(context.get()) + target_offset);
		}

		template <typename T>
		T* get_member_ptr(std::uint8_t index, const std::shared_ptr<instance>& context) {
			if (!_m_registered_to)
				throw unbound_member_access(*this);
			if (*_m_registered_to != *context->_m_type)
				throw illegal_context_type {*this, *context->_m_type};

			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<T*>(reinterpret_cast<char*>(context.get()) + target_offset);
		}

	private:
		friend class script;
		std::string _m_name;
		std::variant<std::unique_ptr<std::int32_t[]>,
		             std::unique_ptr<float[]>,
		             std::unique_ptr<std::string[]>,
		             std::shared_ptr<instance>>
		    _m_value;

		std::uint32_t _m_address {unset};
		std::uint32_t _m_parent {unset};
		std::uint32_t _m_class_offset {unset};
		std::uint32_t _m_count {0};
		datatype _m_type {0};
		flag _m_flags {0};
		bool _m_generated {false};

		std::uint32_t _m_file_index {0};
		std::uint32_t _m_line_start {0};
		std::uint32_t _m_line_count {0};
		std::uint32_t _m_char_start {0};
		std::uint32_t _m_char_count {0};

		std::uint32_t _m_member_offset {unset};
		std::uint32_t _m_class_size {unset};
		datatype _m_return_type {dt_void};
		std::uint32_t _m_index {unset};
		const std::type_info* _m_registered_to {nullptr};
	};

	/**
	 * @brief Represents a daedalus VM instruction.
	 */
	struct instruction {
		opcode op {op_noop};
		std::uint32_t address {0};
		std::uint32_t symbol {0};
		std::int32_t immediate {0};
		std::uint8_t index {0};
		std::uint8_t size {1};

		/**
		 * @brief Reads an instruction from a reader.
		 * @param[in,out] in The reader to read from
		 * @return The instruction read.
		 */
		static instruction decode(buffer& in);
	};

	/**
	 * @brief Represents a compiled daedalus script
	 */
	class script {
	public:
		/**
		 * @brief Parses in a compiled daedalus script.
		 * @param path The path of the script file.
		 * @return The script parsed
		 */
		[[nodiscard]] static script parse(const std::string& path);

		/**
		 * @brief Parses in a compiled daedalus script.
		 * @param buf A buffer containing the script data.
		 * @return The script parsed
		 */
		[[nodiscard]] static script parse(phoenix::buffer& buf);

		/**
		 * @brief Registers a member offset
		 * @param name The name of the member in the script
		 * @param field The field to register
		 */
		template <typename _class, typename _member> // clang-format off
		requires (std::same_as<std::string, _member> || std::same_as<float, _member> || std::same_as<int32_t, _member>)
		void register_member(const std::string& name, _member _class::*field) { // clang-format on
			auto* type = &typeid(_class);
			auto* sym = _check_member<_class, _member, 1>(name, type);

			_class* base = 0;
			auto member = &(base->*field);
			sym->_m_member_offset = std::uint64_t(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/**
		 * @brief Registers a member offset
		 * @param name The name of the member in the script
		 * @param field The field to register
		 */
		template <typename _class, typename _member, int N> // clang-format off
		requires (std::same_as<std::string, _member> || std::same_as<int32_t, _member> || std::same_as<float, _member>)
		void register_member(const std::string& name, _member (_class::*field)[N]) { // clang-format on
			auto* type = &typeid(_class);
			auto* sym = _check_member<_class, _member, N>(name, type);

			_class* base = 0;
			auto member = &(base->*field);
			sym->_m_member_offset = std::uint64_t(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/**
		 * @return All symbols in the script
		 */
		[[nodiscard]] inline const std::vector<symbol>& symbols() const noexcept {
			return _m_symbols;
		}

		/**
		 * @brief Retrieves the symbol with the given \p index
		 * @param index The index of the symbol to get
		 * @return The symbol or `nullptr` if the index was out-of-range.
		 */
		[[nodiscard]] const symbol* find_symbol_by_index(std::uint32_t index) const;

		/**
		 * @brief Looks for parameters of the given function symbol. Only works for external functions.
		 * @param parent The function symbol to get the parameter symbols for.
		 * @return A list of function parameter symbols.
		 */
		[[nodiscard]] std::vector<const symbol*> find_parameters_for_function(const symbol* parent) const;

		/**
		 * @brief Retrieves the symbol with the given \p address set
		 * @param index The address of the symbol to get
		 * @return The symbol or `nullptr` if no symbol with that address was found.
		 */
		[[nodiscard]] const symbol* find_symbol_by_address(std::uint32_t address) const;

		/**
		 * @brief Retrieves the symbol with the given \p name.
		 * @param name The name of the symbol to get.
		 * @return The symbol or `nullptr` if no symbol with that name was found.
		 */
		[[nodiscard]] const symbol* find_symbol_by_name(const std::string& name) const;

		/**
		 * @brief Retrieves the symbol with the given \p index
		 * @param index The index of the symbol to get
		 * @return The symbol or `nullptr` if the index was out-of-range.
		 */
		[[nodiscard]] symbol* find_symbol_by_index(std::uint32_t index);

		/**
		 * @brief Retrieves the symbol with the given \p address set
		 * @param index The address of the symbol to get
		 * @return The symbol or `nullptr` if no symbol with that address was found.
		 */
		[[nodiscard]] symbol* find_symbol_by_address(std::uint32_t address);

		/**
		 * @brief Looks for parameters of the given function symbol. Only works for external functions.
		 * @param parent The function symbol to get the parameter symbols for.
		 * @return A list of function parameter symbols.
		 */
		[[nodiscard]] std::vector<symbol*> find_parameters_for_function(const symbol* parent);

		/**
		 * @brief Retrieves the symbol with the given \p name.
		 * @param name The name of the symbol to get.
		 * @return The symbol or `nullptr` if no symbol with that name was found.
		 */
		[[nodiscard]] symbol* find_symbol_by_name(const std::string& name);

		/**
		 * @brief Call the given callback function for every instance symbol which is a descendant of the class with
		 *        the given name.
		 * @param name The name of the parent class.
		 * @param callback The function to call with each instance symbol.
		 */
		void enumerate_instances_by_class_name(const std::string& name, const std::function<void(symbol&)>& callback);

		/**
		 * @brief Decodes the instruction at \p address and returns it.
		 * @param address The address of the instruction to decode
		 * @return The instruction.
		 */
		[[nodiscard]] instruction instruction_at(std::uint32_t address) const;

		/**
		 * @return The total size of the script.
		 */
		[[nodiscard]] std::uint32_t size() const noexcept {
			return _m_text.limit() & 0xFFFFFF;
		}

		/**
		 * @brief Finds the symbol the given instance is currently bound to.
		 * @param inst The instance to get the symbol for.
		 * @return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		 *         with any instance.
		 */
		inline const symbol* find_symbol_by_instance(const instance& inst) const {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/**
		 * @brief Finds the symbol the given instance is currently bound to.
		 * @param inst The instance to get the symbol for.
		 * @return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		 *         with any instance.
		 */
		inline symbol* find_symbol_by_instance(const instance& inst) {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/**
		 * @brief Finds the symbol the given instance is currently bound to.
		 * @param inst The instance to get the symbol for.
		 * @return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		 *         with any instance.
		 */
		template <typename T> // clang-format off
		requires(std::derived_from<T, instance>)
		inline const symbol* find_symbol_by_instance(const std::shared_ptr<T>& inst) const { // clang-format on
			return find_symbol_by_index(inst->_m_symbol_index);
		}

		/**
		 * @brief Finds the symbol the given instance is currently bound to.
		 * @param inst The instance to get the symbol for.
		 * @return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		 *         with any instance.
		 */
		template <typename T> // clang-format off
	    requires(std::derived_from<T, instance>)
		inline symbol* find_symbol_by_instance(const std::shared_ptr<T>& inst) {
			return find_symbol_by_index(inst->_m_symbol_index);
		}

	protected:
		script() = default;
		script(const script& copy) = default;
		script(script&& move) = default;

		// TODO: When the script is destroyed, clear all references to symbols inside it from all instances already
		//       initialized!

		template <typename _class, typename _member, int N>
		symbol* _check_member(const std::string& name, const std::type_info* type) {
			auto* sym = find_symbol_by_name(name);

			if (sym == nullptr)
				throw symbol_not_found {name};
			if (!sym->is_member())
				throw member_registration_error {*sym, "not a member"};
			if (sym->count() > N)
				throw member_registration_error {*sym,
				                                 "incorrect number of elements: given " + std::to_string(N) +
				                                     " expected " + std::to_string(sym->count())};

			// check class registration
			auto* parent = find_symbol_by_index(sym->parent());
			if (parent == nullptr)
				throw member_registration_error {*sym, "no parent found"};

			if (parent->_m_registered_to == nullptr) {
				parent->_m_registered_to = type;
			} else if (parent->_m_registered_to != type) {
				throw member_registration_error {*sym,
				                                 "parent class is already registered with a different type (" +
				                                     std::string {parent->_m_registered_to->name()} + ")"};
			}

			// check type matches
			if constexpr (std::same_as<std::string, _member>) {
				if (sym->type() != dt_string)
					throw invalid_registration_datatype {*sym, "string"};
			} else if constexpr (std::same_as<float, _member>) {
				if (sym->type() != dt_float)
					throw invalid_registration_datatype {*sym, "float"};
			} else if constexpr (std::same_as<int32_t, _member>) {
				if (sym->type() != dt_integer && sym->type() != dt_function)
					throw invalid_registration_datatype {*sym, "int"};
			} else {
				throw std::runtime_error("illegal type");
			}

			return sym;
		}

		symbol* add_temporary_strings_symbol() {
			symbol sym {};
			sym._m_name = "$PHOENIX_FAKE_STRINGS";
			sym._m_generated = true;
			sym._m_type = dt_string;
			sym._m_count = 1;
			sym._m_value = std::unique_ptr<std::string[]> {new std::string[sym._m_count]};
			sym._m_index = _m_symbols.size();

			return &_m_symbols.emplace_back(std::move(sym));
		}

	private:
		std::vector<symbol> _m_symbols;
		std::unordered_map<std::string, symbol*> _m_symbols_by_name;
		std::unordered_map<std::uint32_t, symbol*> _m_symbols_by_address;

		mutable buffer _m_text = buffer::empty();
		std::uint8_t _m_version {0};
	};
} // namespace phoenix::daedalus
