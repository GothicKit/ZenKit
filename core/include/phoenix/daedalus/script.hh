// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <phoenix/detail/types.hh>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#define unset 0xFF'FF'FF'FFU

namespace phoenix {
	/**
	 * @brief Types of symbols
	 */
	enum datatype : u32 {
		dt_void = 0U,
		dt_float = 1U,
		dt_integer = 2U,
		dt_string = 3U,
		dt_class = 4U,
		dt_function = 5U,
		dt_prototype = 6U,
		dt_instance = 7U,
	};

	constexpr const char * const DAEDALUS_DATA_TYPE_NAMES[] = {
			"void",
			"float",
			"int",
			"string",
			"class",
			"function",
			"prototype",
			"instance"
	};

	/**
	 * @brief Flags set on symbols.
	 */
	enum flag : u32 {
		sf_const = 1U << 0U,
		sf_return = 1U << 1U,
		sf_member = 1U << 2U,
		sf_extern = 1U << 3U,
		sf_merged = 1U << 4U,
	};

	/**
	 * @brief All opcodes supported by the daedalus interpreter
	 */
	enum opcode : u8 {
		op_add = 0,              // a + b
		op_subtract = 1,         // a - b
		op_multiply = 2,         // a * b
		op_divide = 3,           // a / b
		op_modulo = 4,           // a % b
		op_bitor = 5,            // a | b
		op_bitand = 6,           // a & b
		op_less = 7,             // a < b
		op_greater = 8,          // a > b
		op_assign_int = 9,       // a = b
		op_or = 11,              // a || b
		op_and = 12,             // a && b
		op_shift_left = 13,      // a << b
		op_shift_right = 14,     // a >> b
		op_less_or_equal = 15,   // a <= b
		op_equal = 16,           // a == b
		op_not_equal = 17,       // a != b
		op_greater_or_equal = 18,// a >= b
		op_assign_add = 19,      // a += b (a = a + b)
		op_assign_subtract = 20, // a -= b (a = a - b)
		op_assign_multiply = 21, // a *= b (a = a * b)
		op_assign_divide = 22,   // a /= b (a = a / b)
		op_plus = 30,            // +a
		op_minus = 31,           // -a
		op_not = 32,             // !a
		op_complement = 33,      // ~a
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
		op_push_array_var = 245,// EParOp_PushVar + EParOp_Array
	};

	class instance;
	class symbol;

	/**
	 * @brief An exception thrown when the value of a symbol is illegally accessed.
	 */
	class illegal_access : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	/**
	 * @brief An exception thrown when the type of a symbol does not match the type expected.
	 */
	class illegal_type_access final : public illegal_access {
	public:
		illegal_type_access(const symbol& sym, datatype expected);
	};

	/**
	 * @brief An exception thrown when an out-of-bounds index is accessed.
	 */
	class illegal_index_access final : public illegal_access {
	public:
		illegal_index_access(const symbol& sym, u8 index);
	};

	/**
	 * @brief An exception thrown when a constant symbol is accessed as mutable
	 */
	class illegal_const_access final : public illegal_access {
	public:
		illegal_const_access(const symbol& sym);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	class illegal_instance_access final : public illegal_access {
	public:
		illegal_instance_access(const symbol& sym, u32 expected_parent);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	class unbound_member_access final : public illegal_access {
	public:
		explicit unbound_member_access(const symbol& sym);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	class no_context final : public illegal_access {
	public:
		explicit no_context(const symbol& sym);
	};

	/**
	 * @brief An exception thrown when the parent class of a member does not match the class of an instance.
	 */
	class illegal_member_offset final : public illegal_access {
	public:
		illegal_member_offset(const symbol& sym, u32 max, u32 actual, u32 value_size);
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
		static symbol parse(reader& in);

		/**
		 * @brief Validates that the symbol is a string and retrieves it's value in the given context.
		 * @param index The index of the value to get.
		 * @param context An instance to use as context for getting member variables.
		 * @return The string associated with the symbol.
		 */
		const std::string& get_string(u8 index = 0, const std::shared_ptr<instance>& context = nullptr) const;

		/**
		 * @brief Validates that the symbol is a float and retrieves it's value in the given context.
		 * @param index The index of the value to get.
		 * @param context An instance to use as context for getting member variables.
		 * @return The float value associated with the symbol.
		 */
		float get_float(u8 index = 0, const std::shared_ptr<instance>& context = nullptr) const;

		/**
		 * @brief Validates that the symbol is an int and retrieves it's value in the given context.
		 * @param index The index of the value to get.
		 * @param context An instance to use as context for getting member variables.
		 * @return The int value associated with the symbol.
		 */
		s32 get_int(u8 index = 0, const std::shared_ptr<instance>& context = nullptr) const;

		/**
		 * @brief Validates that the symbol is an instance and retrieves it's value
		 * @return The instance associated with the symbol.
		 */
		std::shared_ptr<instance> get_instance();

		// -=-= Value setters =-=- //

		/**
		 * @brief Validates that the symbol is a string and not constant and sets it's value in the given context.
		 * @param value The new value to set.
		 * @param index The index of the value to set
		 * @param context An instance to use as context for setting member variables.
		 */
		void set_string(const std::string& value, u8 index = 0, const std::shared_ptr<instance>& context = nullptr);

		/**
		 * @brief Validates that the symbol is a float and not constant and sets it's value in the given context.
		 * @param value The new value to set.
		 * @param index The index of the value to set
		 * @param context An instance to use as context for setting member variables.
		 */
		void set_float(float value, u8 index = 0, const std::shared_ptr<instance>& context = nullptr);

		/**
		 * @brief Validates that the symbol is an int and not constant and sets it's value in the given context.
		 * @param value The new value to set.
		 * @param index The index of the value to set
		 * @param context An instance to use as context for setting member variables.
		 */
		void set_int(s32 value, u8 index = 0, const std::shared_ptr<instance>& context = nullptr);

		/**
		 * @brief Validates that the symbol is an instance and sets it's value
		 * @param inst The instance value to set
		 */
		void set_instance(std::shared_ptr<instance> inst);

		/**
		 * @brief Tests whether the symbol is a constant.
		 * @return `true` if the symbol is a constant, `false` if not.
		 */
		[[nodiscard]] inline bool is_const() const noexcept { return (_m_flags & sf_const) != 0; }
		/**
		 * @brief Tests whether the symbol is a member variable.
		 * @return `true` if the symbol is a member, `false` if not.
		 */
		[[nodiscard]] inline bool is_member() const noexcept { return (_m_flags & sf_member) != 0; }

		/**
		 * @brief Tests whether the symbol is an extern symbol.
		 * @return `true` if the symbol is an extern symbol, `false` if not.
		 */
		[[nodiscard]] inline bool is_extern() const noexcept { return (_m_flags & sf_extern) != 0; }

		/**
		 * @brief Tests whether the symbol is merged.
		 * @return `true` if the symbol is merged, `false` if not.
		 * @note It is currently not known what 'merged' means.
		 */
		[[nodiscard]] inline bool is_merged() const noexcept { return (_m_flags & sf_merged) != 0; }

		/**
		 * @brief brief Tests whether the symbol is a compiler-generated symbol
		 * @return return `true` if the symbol is generated, `false` if not.
		 */
		[[nodiscard]] inline bool is_generated() const noexcept { return _m_generated; }

		/**
		 * @brief brief Tests whether the symbol has a return value.
		 * @return return `true` if the symbol has a return value, `false` if not.
		 */
		[[nodiscard]] inline bool has_return() const noexcept { return (_m_flags & sf_return) != 0; }

		/**
		 * @return The name of the symbol.
		 */
		[[nodiscard]] inline const std::string& name() const noexcept { return _m_name; }

		/**
		 * @return The address of the symbol.
		 */
		[[nodiscard]] inline u32 address() const noexcept { return _m_address; }

		/**
		 * @return The index of the parent symbol or unset if the symbol does not have a parent.
		 */
		[[nodiscard]] inline u32 parent() const noexcept { return _m_parent; }

		/**
		 * @return The count of values stored in the symbol.
		 */
		[[nodiscard]] inline u32 count() const noexcept { return _m_count; }

		/**
		 * @return The type of the symbol.
		 */
		[[nodiscard]] inline datatype type() const noexcept { return _m_type; }

		/**
		 * @return The index of the symbol.
		 */
		[[nodiscard]] inline u32 index() const noexcept { return _m_index; }

		/**
		 * @return The return type of the symbol.
		 */
		[[nodiscard]] inline datatype rtype() const noexcept { return _m_return_type; }

		/**
		 * @return The index of the file the symbol was in.
		 */
		[[nodiscard]] inline u32 file_index() const noexcept { return _m_file_index; }

		/**
		 * @return The offset in bytes of a member from the start of the instance.
		 */
		[[nodiscard]] inline u32 offset_as_member() const noexcept { return _m_member_offset; }


		[[nodiscard]] inline u32 line_start() const noexcept { return _m_line_start; }
		[[nodiscard]] inline u32 line_count() const noexcept { return _m_line_count; }
		[[nodiscard]] inline u32 char_start() const noexcept { return _m_char_start; }
		[[nodiscard]] inline u32 char_count() const noexcept { return _m_char_count; }
		[[nodiscard]] inline u32 class_size() const noexcept { return _m_class_size; }

	protected:
		symbol() = default;

		template <typename T>
		const T* get_member_ptr(u8 index, const std::shared_ptr<instance>& context) const {
			// if (sym.parent() != _m_data.klass) { throw illegal_instance_access(sym, _m_data.klass); }  // TODO: fix this check - Inheritance chains don't work:  C_NPC -> NPC_DEFAULT -> STT_309_WHISTLER
			if (offset_as_member() == unset) { throw unbound_member_access(*this); }

			u32 target_offset = offset_as_member() + index * sizeof(T);
			// if (target_offset + sizeof(T) > _m_data.size) { throw illegal_member_offset(sym, _m_data.size, target_offset, sizeof(T)); } // TODO: fix this check!

			return reinterpret_cast<const T*>(reinterpret_cast<const char*>(context.get()) + target_offset);
		}

		template <typename T>
		T* get_member_ptr(u8 index, const std::shared_ptr<instance>& context) {
			// if (sym.parent() != _m_data.klass) { throw illegal_instance_access(sym, _m_data.klass); }  // TODO: fix this check - Inheritance chains don't work:  C_NPC -> NPC_DEFAULT -> STT_309_WHISTLER
			if (offset_as_member() == unset) { throw unbound_member_access(*this); }

			u32 target_offset = offset_as_member() + index * sizeof(T);
			// if (target_offset + sizeof(T) > _m_data.size) { throw illegal_member_offset(sym, _m_data.size, target_offset, sizeof(T)); } // TODO: fix this check!

			return reinterpret_cast<T*>(reinterpret_cast<char*>(context.get()) + target_offset);
		}

	private:
		friend class script;
		std::string _m_name;
		std::variant<std::unique_ptr<s32[]>, std::unique_ptr<float[]>, std::unique_ptr<std::string[]>, std::shared_ptr<instance>> _m_value;

		u32 _m_address {unset};
		u32 _m_parent {unset};
		u32 _m_class_offset {unset};
		u32 _m_count {0};
		datatype _m_type {0};
		flag _m_flags {0};
		bool _m_generated {false};

		u32 _m_file_index {0};
		u32 _m_line_start {0};
		u32 _m_line_count {0};
		u32 _m_char_start {0};
		u32 _m_char_count {0};

		u32 _m_member_offset {unset};
		u32 _m_class_size {unset};
		datatype _m_return_type {dt_void};
		u32 _m_index {unset};

		// TODO: type safety using std::type_info and typeid
	};

	/**
	 * @brief Represents an object associated with an instance in the script.
	 */
	class instance {
	public:
		explicit instance(symbol* sym) : sym(sym) {}

		virtual ~instance() = default;

		symbol* sym;
	};

	/**
	 * @brief Represents a daedalus VM instruction.
	 */
	struct instruction {
		opcode op {op_noop};
		u32 address {0};
		u32 symbol {0};
		s32 immediate {0};
		u8 index {0};
		u8 size {1};

		/**
		 * @brief Reads an instruction from a reader.
		 * @param[in,out] in The reader to read from
		 * @return The instruction read.
		 */
		static instruction decode(reader& in);
	};

	/**
	 * @brief Represents a compiled daedalus script
	 * @todo Add a way to register external symbols as functions
	 */
	class script {
	public:
		/**
		 * @brief Parses in a compiled daedalus script.
		 * @param path The path of the script file.
		 * @return The script parsed
		 */
		static script parse(const std::string& path);

		/**
		 * @brief Registers a member offset
		 * @param name The name of the member in the script
		 * @param field The field to register
		 */
		template <typename _class, typename _member>
		void register_member(const std::string& name, _member _class::*field) {
			auto* sym = find_symbol_by_name(name);
			if (sym == nullptr) { throw std::runtime_error("cannot register member " + name + ": not found"); }
			if (!sym->is_member()) { throw std::runtime_error("cannot register member " + name + ": not a member"); }

			_class* base = 0;
			_member* member = &(base->*field);

			// TODO: check type of _member and make sure it is one of: s32, f32, s32[], f32[], std::string, std::string[]
			//       depending on sym.type()

			sym->_m_member_offset = (u64) member;
		}

		/**
		 * @return All symbols in the script
		 */
		[[nodiscard]] inline const std::vector<symbol>& symbols() const noexcept { return _m_symbols; }

		/**
		 * @brief Retrieves the symbol with the given \p index
		 * @param index The index of the symbol to get
		 * @return The symbol or `nullptr` if the index was out-of-range.
		 */
		[[nodiscard]] const symbol* find_symbol_by_index(u32 index) const;

		/**
		 * @brief Retrieves the symbol with the given \p address set
		 * @param index The address of the symbol to get
		 * @return The symbol or `nullptr` if no symbol with that address was found.
		 */
		[[nodiscard]] const symbol* find_symbol_by_address(u32 address) const;

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
		[[nodiscard]] symbol* find_symbol_by_index(u32 index);

		/**
		 * @brief Retrieves the symbol with the given \p address set
		 * @param index The address of the symbol to get
		 * @return The symbol or `nullptr` if no symbol with that address was found.
		 */
		[[nodiscard]] symbol* find_symbol_by_address(u32 address);

		/**
		 * @brief Retrieves the symbol with the given \p name.
		 * @param name The name of the symbol to get.
		 * @return The symbol or `nullptr` if no symbol with that name was found.
		 */
		[[nodiscard]] symbol* find_symbol_by_name(const std::string& name);

		/**
		 * @brief Decodes the instruction at \p address and returns it.
		 * @param address The address of the instruction to decode
		 * @return The instruction.
		 */
		instruction instruction_at(u32 address) const;

		/**
		 * @return The total size of the script.
		 */
		[[nodiscard]] u32 size() const noexcept { return _m_text.size(); }

		symbol& dynamic_strings() const noexcept { return *_m_dynamic_strings; }

	protected:
		script() = default;

	private:
		std::vector<symbol> _m_symbols;
		std::unordered_map<std::string, symbol*> _m_symbols_by_name;
		std::unordered_map<u32, symbol*> _m_symbols_by_address;

		symbol* _m_dynamic_strings;

		mutable reader _m_text {};
		u8 _m_version {0};
	};

}// namespace phoenix
