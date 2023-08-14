// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include "Api.hh"
#include <phoenix/buffer.hh>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#define unset 0xFF'FF'FF'FFU

namespace phoenix {
	/// \brief Daedalus data types.
	enum class datatype : std::uint32_t {
		void_ = 0U,     ///< A datatype similar to C++'s `void`.
		float_ = 1U,    ///< A 32-bit floating point type similar to C++'s `float`.
		integer = 2U,   ///< A 32-bit signed integer type like std::int32_t
		string = 3U,    ///< A [Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) encoded character array.
		class_ = 4U,    ///< A structure definition.
		function = 5U,  ///< A function definition or a function pointer represented as a 32-bit signed integer.
		prototype = 6U, ///< A prototype definition.
		instance = 7U,  ///< An instance definition or instance reference represented as a 32-bit unsigned integer.
	};

	constexpr const char* const DAEDALUS_DATA_TYPE_NAMES[] = {
	    "void",
	    "float",
	    "int",
	    "string",
	    "class",
	    "function",
	    "prototype",
	    "instance",
	};

	/// \brief Flags of symbols.
	namespace symbol_flag {
		static constexpr auto const_ = 1U << 0U;      ///< The symbol is not mutable.
		static constexpr auto return_ = 1U << 1U;     ///< The symbol is a function and has a return value.
		static constexpr auto member = 1U << 2U;      ///< The symbol is a class member.
		static constexpr auto external = 1U << 3U;    ///< The symbol refers to an external function.
		static constexpr auto merged = 1U << 4U;      ///< Unused.
		static constexpr auto access_trap = 1U << 6U; ///< VM should call trap callback, when symbol accessed.
	}                                                 // namespace symbol_flag

	/// \brief All opcodes supported by the daedalus interpreter.
	///
	/// <p>In the documentation for each opcode, the following conventions apply.</p>
	/// <ul>
	///   <li>`a` refers to the first value on the stack and is required to be an integer.</li>
	///   <li>`b` refers to the second value on the stack and is required to be an integer.</li>
	///   <li>`m` refers to the first value on the stack which may be of any non-reference type.</li>
	///   <li>`n` refers to the second value on the stack which may be of any non-reference type.</li>
	///   <li>`x` refers to the first value on the stack and is required to be variable reference.</li>
	///	  <li>`y` refers to the second value on the stack and is required to be variable reference.</li>
	/// </ul>
	enum class opcode : std::uint8_t {
		/// \brief Add `a` and `b` and put the result back onto the stack.
		add = 0,

		/// \brief Subtract `b` from `a` and put the result back onto the stack.
		sub = 1,

		/// \brief Multiply `a` and `b` and put the result back onto the stack.
		mul = 2,

		/// \brief Divide `a` by `b` and put the result back onto the stack.
		div = 3,

		/// \brief Divide `a` by `b` and put the remainder back onto the stack.
		mod = 4,

		/// \brief Compute the bitwise or of `a` and `b` and put the result back onto the stack.
		or_ = 5,

		/// \brief Compute the bitwise and of `a` and `b` and put the result back onto the stack.
		andb = 6, ///< a & b

		/// \brief Test if `a` is less than `b` and put `1` or `0` onto the stack if
		///        the test is true or false respectively.
		lt = 7,

		/// \brief Test if `a` is greater than `b` and put `1` or `0` onto the stack
		///        if the test is true or false respectively.
		gt = 8,

		/// \brief Write `b` to `x` as an integer.
		movi = 9,

		/// \brief Test if `a == 1` or `b == 1` and put `1` or `0` onto the stack if
		/// 		the test is true or false respectively.
		orr = 11,

		/// \brief Test if `a == 1` and `b == 1` and put `1` or `0` onto the stack if
		///        the test is true or false respectively.
		and_ = 12,

		/// \brief Left shift  `a` by `b` bits and put the result back onto the stack.
		lsl = 13,

		/// \brief Right shift  `a` by `b` bits and put the result back onto the stack.
		lsr = 14,

		/// \brief Test if `a` is less than or equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		lte = 15,

		/// \brief Test if `a` is equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		eq = 16,

		/// \brief Test if `a` is not equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		neq = 17,

		/// \brief Test if `a` is greater than or equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		gte = 18,

		/// \brief Add `x` and `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		addmovi = 19,

		/// \brief Subtract `b` from `x` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		submovi = 20,

		/// \brief Multiply `x` from `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		mulmovi = 21,

		/// \brief Divide `x` by `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		divmovi = 22,

		/// \brief Compute `+a` and put the result back onto the stack.
		plus = 30,

		/// \brief Compute `-a` and put the result back onto the stack.
		negate = 31,

		/// \brief Compute `!a` and put the result back onto the stack.
		not_ = 32,

		/// \brief Compute the bitwise complement `a` and put the result back onto the stack.
		cmpl = 33,

		/// \brief Do nothing.
		nop = 45,

		/// \brief Return from the currently running function
		rsr = 60,

		/// \brief Call the function at the address provided in the instruction.
		bl = 61,

		/// \brief Call the external function at the symbol index provided in the instruction.
		be = 62,

		/// \brief Push the immediate value provided in the instruction onto the stack as an integer.
		pushi = 64,

		/// \brief Push the symbol with the index provided in the instruction onto the stack as a reference.
		pushv = 65,

		/// \brief Push the instance with the symbol index provided in the instruction onto the stack as a reference.
		pushvi = 67,

		/// \brief Write `m` to `x` as a string.
		movs = 70,

		/// \brief Write `m` to `x` as a string reference; not implemented.
		movss = 71,

		/// \brief Write `b` to `x` as a function reference.
		movvf = 72,

		/// \brief Write `b` to `x` as a floating point number.
		movf = 73,

		/// \brief Write `y` to `x` as an instance reference.
		movvi = 74,

		/// \brief Immediately jump to the instruction at the address provided in the instruction.
		b = 75,

		/// \brief Jump to the instruction at the address provided in the instruction if `a == 0`.
		bz = 76,

		/// \brief Set the global instance reference to the instance with the symbol index provided in the instrucion.
		gmovi = 80,

		/// \brief Push the element at the given index of the symbol with the index provided in the
		///        instruction onto the stack as a reference.
		pushvv = 245,
	};

	class symbol;

	/// \brief Represents an object associated with an instance in the script.
	///
	/// Every class defined in C++ that can be used as an instance has to inherit from this class.
	class instance {
	public:
		PHOENIX_API virtual ~instance() = default;

		/// \return The index of the symbol this instance is bound to.
		[[nodiscard]] PHOENIX_API inline uint32_t symbol_index() const {
			return _m_symbol_index;
		}

		/// \brief A pointer which may be used by the user of this API
		void* user_ptr = nullptr;

	protected:
		PHOENIX_INTERNAL virtual std::uint8_t* data() {
			return reinterpret_cast<std::uint8_t*>(this);
		}

		PHOENIX_INTERNAL virtual const std::uint8_t* data() const {
			return reinterpret_cast<const std::uint8_t*>(this);
		}

	private:
		friend class transient_instance;
		friend class symbol;
		friend class script;
		friend class vm;

		uint32_t _m_symbol_index {unset};
		const std::type_info* _m_type {nullptr};
	};

	/// \brief Represents an object associated with an instance in the script.
	///
	/// Instances allocated with init_opaque will be backed up by this class with plain memory storage
	class opaque_instance final : public instance {
	public:
		PHOENIX_INTERNAL opaque_instance(symbol const& sym, std::vector<symbol*> const& members);
		PHOENIX_INTERNAL ~opaque_instance();

	protected:
		friend class symbol;

		PHOENIX_INTERNAL std::uint8_t* data() override {
			return _m_storage.get();
		}

		PHOENIX_INTERNAL const std::uint8_t* data() const override {
			return _m_storage.get();
		}

	private:
		template <typename T, typename... Args>
		PHOENIX_INTERNAL T* construct_at(size_t offset, Args&&... args);

		std::unique_ptr<std::uint8_t[]> _m_storage;
		std::vector<std::string*> _m_strings;
	};

	/// \brief Represents object instance in the script with no defined backing to memory.
	///
	/// Expected to be used for DMA mods or to emulate variable-like access to engine-functions.
	class transient_instance : public instance {
	public:
		transient_instance();
		~transient_instance();

	protected:
		friend class symbol;

		virtual void read32(void* data32, symbol const& sym, size_t index) = 0;
		virtual void write32(const void* data32, symbol const& sym, size_t index) = 0;

		virtual void write_string(std::string_view str, symbol const& sym, size_t index) = 0;
		virtual const std::string& read_string(symbol const& sym, size_t index) = 0;
	};

	/// \brief The base class for all exceptions thrown by interacting with a script.
	struct script_error : public error {
		using error::error;
	};

	/// \brief An exception thrown if the symbol with a given name could not be found.
	struct symbol_not_found : public script_error {
	public:
		PHOENIX_API explicit symbol_not_found(std::string&& name);

	public:
		std::string name;
	};

	/// \brief An exception thrown if registering a class member was unsuccessful.
	struct member_registration_error : public script_error {
	public:
		PHOENIX_API explicit member_registration_error(const symbol* sym, std::string&& message);

	public:
		/// \brief The symbol being registered.
		const symbol* sym;
	};

	/// \brief An exception thrown if the type of the member being registered does not match the type provided.
	struct invalid_registration_datatype final : public member_registration_error {
	public:
		PHOENIX_API explicit invalid_registration_datatype(const symbol* sym, std::string&& given);

	public:
		std::string given;
	};

	/// \brief An exception thrown when the value of a symbol accessed in a way which is not permissible.
	struct illegal_access : public script_error {
		using script_error::script_error;
	};

	/// \brief An exception thrown when the type of a symbol does not match the type expected.
	struct illegal_type_access final : public illegal_access {
	public:
		PHOENIX_INTERNAL illegal_type_access(const symbol* sym, datatype expected);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;

		/// \brief The datatype excepted.
		datatype expected;
	};

	/// \brief An exception thrown when an out-of-bounds index is accessed.
	struct illegal_index_access final : public illegal_access {
	public:
		PHOENIX_INTERNAL illegal_index_access(const symbol* sym, std::uint8_t index);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;

		/// \brief The index being accessed
		std::uint8_t index;
	};

	/// \brief An exception thrown when a constant symbol is accessed as mutable
	struct illegal_const_access final : public illegal_access {
	public:
		PHOENIX_INTERNAL explicit illegal_const_access(const symbol* sym);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;
	};

	/// \brief An exception thrown when the parent class of a member does not match the class of an instance.
	struct illegal_instance_access final : public illegal_access {
	public:
		PHOENIX_INTERNAL illegal_instance_access(const symbol* sym, std::uint32_t expected_parent);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;

		/// \brief The parent which was expected for the member.
		std::uint32_t expected_parent;
	};

	/// \brief An exception thrown when the parent class of a member does not match the class of an instance.
	struct unbound_member_access final : public illegal_access {
	public:
		PHOENIX_API explicit unbound_member_access(const symbol* sym);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;
	};

	/// \brief An exception thrown if a member symbol is being access without a context set.
	struct no_context final : public illegal_access {
	public:
		PHOENIX_INTERNAL explicit no_context(const symbol* sym);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;
	};

	/// \brief An excpetion thrown if a member symbol is being accessed with a context instance it is not bound to.
	struct illegal_context_type final : public illegal_access {
	public:
		PHOENIX_API illegal_context_type(const symbol* sym, const std::type_info& context_type);

	public:
		/// \brief The symbol being accessed.
		const symbol* sym;

		/// \brief The type of context currently set.
		const std::type_info& context_type;
	};

	/// \brief Represents a compiled daedalus symbol.
	class symbol final {
	public:
		/// \brief Parses a symbol from the given reader.
		/// \param[in,out] in The reader to read the symbol from.
		/// \return The symbol parsed.
		[[nodiscard]] PHOENIX_API static symbol parse(buffer& in);

		/// \brief Validates that the symbol is a string and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The string associated with the symbol.
		/// \throws illegal_type_access if the #type of this symbol is not dt_string.
		/// \throws illegal_index_access if \p index >= #count.
		/// \throws no_context if this symbol #is_member and \p context is `nullptr`.
		/// \throws unbound_member_access if this symbol has not been registered yet
		/// \throws illegal_context_type if this symbol #is_registered_to a different type than the type of \p context.
		[[nodiscard]] PHOENIX_API const std::string&
		get_string(std::size_t index = 0, const std::shared_ptr<instance>& context = nullptr) const;

		/// \brief Validates that the symbol is a float and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The float value associated with the symbol.
		/// \throws illegal_type_access if the #type of this symbol is not dt_float.
		/// \throws illegal_index_access if \p index >= #count.
		/// \throws no_context if this symbol #is_member and \p context is `nullptr`.
		/// \throws unbound_member_access if this symbol has not been registered yet
		/// \throws illegal_context_type if this symbol #is_registered_to a different type than the type of \p context.
		[[nodiscard]] PHOENIX_API float get_float(std::size_t index = 0,
		                                          const std::shared_ptr<instance>& context = nullptr) const;

		/// \brief Validates that the symbol is an int and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The int value associated with the symbol.
		/// \throws illegal_type_access if the #type of this symbol is not dt_int or dt_function.
		/// \throws illegal_index_access if \p index >= #count.
		/// \throws no_context if this symbol #is_member and \p context is `nullptr`.
		/// \throws unbound_member_access if this symbol has not been registered yet
		/// \throws illegal_context_type if this symbol #is_registered_to a different type than the type of \p context.
		[[nodiscard]] PHOENIX_API std::int32_t get_int(std::size_t index = 0,
		                                               const std::shared_ptr<instance>& context = nullptr) const;

		/// \brief Validates that the symbol is an instance and retrieves it's value
		/// \return The instance associated with the symbol.
		/// \throws illegal_type_access if the #type of this symbol is not dt_instance
		[[nodiscard]] PHOENIX_API const std::shared_ptr<instance>& get_instance();

		// -=-= Value setters =-=- //

		/// \brief Validates that the symbol is a string and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws illegal_type_access if the #type of this symbol is not dt_string.
		/// \throws illegal_index_access if \p index >= #count.
		/// \throws no_context if this symbol #is_member and \p context is `nullptr`.
		/// \throws unbound_member_access if this symbol has not been registered yet
		/// \throws illegal_context_type if this symbol #is_registered_to a different type than the type of \p context.
		PHOENIX_API void
		set_string(std::string_view value, std::size_t index = 0, const std::shared_ptr<instance>& context = nullptr);

		/// \brief Validates that the symbol is a float and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws illegal_type_access if the #type of this symbol is not dt_float
		/// \throws illegal_index_access if \p index >= #count.
		/// \throws no_context if this symbol #is_member and \p context is `nullptr`.
		/// \throws unbound_member_access if this symbol has not been registered yet
		/// \throws illegal_context_type if this symbol #is_registered_to a different type than the type of \p context.
		PHOENIX_API void
		set_float(float value, std::size_t index = 0, const std::shared_ptr<instance>& context = nullptr);

		/// \brief Validates that the symbol is an int and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws illegal_type_access if the #type of this symbol is not dt_int or dt_function.
		/// \throws illegal_index_access if \p index >= #count.
		/// \throws no_context if this symbol #is_member and \p context is `nullptr`.
		/// \throws unbound_member_access if this symbol has not been registered yet
		/// \throws illegal_context_type if this symbol #is_registered_to a different type than the type of \p context.
		PHOENIX_API void
		set_int(std::int32_t value, std::size_t index = 0, const std::shared_ptr<instance>& context = nullptr);

		/// \brief Validates that the symbol is an instance and sets it's value
		/// \param inst The instance value to set
		/// \throws illegal_type_access if the #type of this symbol is not dt_instance.
		PHOENIX_API void set_instance(const std::shared_ptr<instance>& inst);

		/// \brief Tests whether this symbol holds an instance of the given type.
		/// \tparam T The type of instance to check for.
		/// \return <tt>true</tt> if the symbol contains an instance of the given type, <tt>false</tt> if not.
		template <typename T>
		PHOENIX_API typename std::enable_if<std::is_base_of_v<instance, T>,
		                                    bool>::type inline is_instance_of() { // clang-format on
			return this->type() == datatype::instance && this->get_instance() != nullptr &&
			    this->get_instance()->_m_type == &typeid(T);
		}

		/// \brief Allows VM traps on access to this symbol
		/// \param enable true to enable and false to disable
		PHOENIX_API void set_access_trap_enable(bool enable) noexcept;

		/// \brief Tests whether the symbol is a constant.
		/// \return `true` if the symbol is a constant, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_const() const noexcept {
			return (_m_flags & symbol_flag::const_) != 0;
		}

		/// \brief Tests whether the symbol is a member variable.
		/// \return `true` if the symbol is a member, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_member() const noexcept {
			return (_m_flags & symbol_flag::member) != 0;
		}

		/// \brief Tests whether the symbol is an extern symbol.
		/// \return `true` if the symbol is an extern symbol, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_external() const noexcept {
			return (_m_flags & symbol_flag::external) != 0;
		}

		/// \brief Tests whether the symbol is merged.
		/// \return `true` if the symbol is merged, `false` if not.
		/// \note It is currently not known what 'merged' means.
		[[nodiscard]] PHOENIX_API inline bool is_merged() const noexcept {
			return (_m_flags & symbol_flag::merged) != 0;
		}

		/// \brief Tests whether the symbol has access trap.
		/// \return `true` if the symbol has trap enabled, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool has_access_trap() const noexcept {
			return (_m_flags & symbol_flag::access_trap) != 0;
		}
		/// \brief brief Tests whether the symbol is a compiler-generated symbol
		/// \return return `true` if the symbol is generated, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool is_generated() const noexcept {
			return _m_generated;
		}

		/// \brief brief Tests whether the symbol has a return value.
		/// \return return `true` if the symbol has a return value, `false` if not.
		[[nodiscard]] PHOENIX_API inline bool has_return() const noexcept {
			return (_m_flags & symbol_flag::return_) != 0;
		}

		/// \return The name of the symbol.
		[[nodiscard]] PHOENIX_API inline const std::string& name() const noexcept {
			return _m_name;
		}

		/// \return The address of the symbol.
		[[nodiscard]] PHOENIX_API inline std::uint32_t address() const noexcept {
			return _m_address;
		}

		/// \return The index of the parent symbol or unset if the symbol does not have a parent.
		[[nodiscard]] PHOENIX_API inline std::uint32_t parent() const noexcept {
			return _m_parent;
		}

		/// \return The count of values stored in the symbol.
		[[nodiscard]] PHOENIX_API inline std::uint32_t count() const noexcept {
			return _m_count;
		}

		/// \return The type of the symbol.
		[[nodiscard]] PHOENIX_API inline datatype type() const noexcept {
			return _m_type;
		}

		/// \return The index of the symbol.
		[[nodiscard]] PHOENIX_API inline std::uint32_t index() const noexcept {
			return _m_index;
		}

		/// \return The return type of the symbol.
		[[nodiscard]] PHOENIX_API inline datatype rtype() const noexcept {
			return _m_return_type;
		}

		/// \return The index of the file the symbol was in.
		[[nodiscard]] PHOENIX_API inline std::uint32_t file_index() const noexcept {
			return _m_file_index;
		}

		/// \return The offset in bytes of a member from the start of the instance.
		[[nodiscard]] PHOENIX_API inline std::uint32_t offset_as_member() const noexcept {
			return _m_member_offset;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t line_start() const noexcept {
			return _m_line_start;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t line_count() const noexcept {
			return _m_line_count;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t char_start() const noexcept {
			return _m_char_start;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t char_count() const noexcept {
			return _m_char_count;
		}

		[[nodiscard]] PHOENIX_API inline std::uint32_t class_size() const noexcept {
			return _m_class_size;
		}

		[[nodiscard]] PHOENIX_API inline const std::type_info& registered_to() const noexcept {
			return *_m_registered_to;
		};

	protected:
		PHOENIX_INTERNAL symbol() = default;

		template <typename T>
		inline const T* get_member_ptr(std::uint8_t index, const std::shared_ptr<instance>& context) const {
			if (!_m_registered_to)
				throw unbound_member_access(this);
			if (*_m_registered_to != *context->_m_type)
				throw illegal_context_type {this, *context->_m_type};

			auto data_ptr = context->data();
			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<const T*>(data_ptr + target_offset);
		}

		template <typename T>
		inline T* get_member_ptr(std::uint8_t index, const std::shared_ptr<instance>& context) {
			if (!_m_registered_to)
				throw unbound_member_access(this);
			if (*_m_registered_to != *context->_m_type)
				throw illegal_context_type {this, *context->_m_type};

			auto data_ptr = context->data();
			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<T*>(data_ptr + target_offset);
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
		std::uint32_t _m_flags {0};
		bool _m_generated {false};

		std::uint32_t _m_file_index {0};
		std::uint32_t _m_line_start {0};
		std::uint32_t _m_line_count {0};
		std::uint32_t _m_char_start {0};
		std::uint32_t _m_char_count {0};

		std::uint32_t _m_member_offset {unset};
		std::uint32_t _m_class_size {unset};
		datatype _m_return_type {datatype::void_};
		std::uint32_t _m_index {unset};
		const std::type_info* _m_registered_to {nullptr};
	};

	/// \brief Represents a daedalus VM instruction.
	struct instruction {
		opcode op {opcode::nop};
		std::uint32_t address {0};
		std::uint32_t symbol {0};
		std::int32_t immediate {0};
		std::uint8_t index {0};
		std::uint8_t size {1};

		/// \brief Reads an instruction from a reader.
		/// \param[in,out] in The reader to read from
		/// \return The instruction read.
		PHOENIX_INTERNAL static instruction decode(buffer& in);
	};

	/// \brief Represents a compiled daedalus script

	class script {
	public:
		PHOENIX_API script(const script& copy) = default;
		PHOENIX_API script(script&& move) = default;

		/// \brief Parses in a compiled daedalus script.
		/// \param path The path of the script file.
		/// \return The script parsed
		[[nodiscard]] PHOENIX_API static script parse(const std::string& path);

		/// \brief Parses in a compiled daedalus script.
		/// \param buf A buffer containing the script data.
		/// \return The script parsed
		[[nodiscard]] PHOENIX_API static script parse(phoenix::buffer& buf);

		/// \brief Registers a member offset
		/// \param name The name of the member in the script
		/// \param field The field to register
		/// \throws symbol_not_found if there is no symbol with the given name.
		/// \throws member_registration_error if the member could not be registered
		/// \throws invalid_registration_datatype If the datatype of \p _member is different than that of the symbol.
		template <typename _class, typename _member, int N>
		typename std::enable_if<std::is_same_v<_member, std::string> || std::is_same_v<_member, float> ||
		                            std::is_same_v<_member, std::int32_t> ||
		                            (std::is_enum_v<_member> && sizeof(_member) == 4),
		                        void>::type
		register_member(std::string_view name, _member (_class::*field)[N]) { // clang-format on
			auto* type = &typeid(_class);
			auto* sym = _check_member<_class, _member, N>(name, type);

			_class* base = 0;
			auto member = &(base->*field);
			sym->_m_member_offset = std::uint64_t(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/// \brief Registers a member offset
		/// \param name The name of the member in the script
		/// \param field The field to register
		/// \throws symbol_not_found if there is no symbol with the given name.
		/// \throws member_registration_error if the member could not be registered
		/// \throws invalid_registration_datatype If the datatype of \p _member is different than that of the symbol.
		template <typename _class, typename _member>
		typename std::enable_if<std::is_same_v<_member, std::string> || std::is_same_v<_member, float> ||
		                            std::is_same_v<_member, std::int32_t> ||
		                            (std::is_enum_v<_member> && sizeof(_member) == 4),
		                        void>::type
		register_member(std::string_view name, _member _class::*field) {
			auto* type = &typeid(_class);
			auto* sym = _check_member<_class, _member, 1>(name, type);

			_class* base = 0;
			auto member = &(base->*field);
			sym->_m_member_offset = std::uint64_t(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/// \return All symbols in the script
		[[nodiscard]] PHOENIX_API inline const std::vector<symbol>& symbols() const noexcept {
			return _m_symbols;
		}

		/// \brief Retrieves the symbol with the given \p index
		/// \param index The index of the symbol to get
		/// \return The symbol or `nullptr` if the index was out-of-range.
		[[nodiscard]] PHOENIX_API const symbol* find_symbol_by_index(std::uint32_t index) const;

		/// \brief Looks for parameters of the given function symbol. Only works for external functions.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function parameter symbols.
		[[nodiscard]] PHOENIX_API std::vector<const symbol*> find_parameters_for_function(const symbol* parent) const;

		/// \brief Retrieves the symbol with the given \p address set
		/// \param index The address of the symbol to get
		/// \return The symbol or `nullptr` if no symbol with that address was found.
		[[nodiscard]] PHOENIX_API const symbol* find_symbol_by_address(std::uint32_t address) const;

		/// \brief Retrieves the symbol with the given \p name.
		/// \param name The name of the symbol to get.
		/// \return The symbol or `nullptr` if no symbol with that name was found.
		[[nodiscard]] PHOENIX_API const symbol* find_symbol_by_name(std::string_view name) const;

		/// \brief Retrieves the symbol with the given \p index
		/// \param index The index of the symbol to get
		/// \return The symbol or `nullptr` if the index was out-of-range.
		[[nodiscard]] PHOENIX_API symbol* find_symbol_by_index(std::uint32_t index);

		/// \brief Retrieves the symbol with the given \p address set
		/// \param index The address of the symbol to get
		/// \return The symbol or `nullptr` if no symbol with that address was found.
		[[nodiscard]] PHOENIX_API symbol* find_symbol_by_address(std::uint32_t address);

		/// \brief Looks for parameters of the given function symbol. Only works for external functions.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function parameter symbols.
		[[nodiscard]] PHOENIX_API std::vector<symbol*> find_parameters_for_function(const symbol* parent);

		/// \brief Retrieves the symbol with the given \p name.
		/// \param name The name of the symbol to get.
		/// \return The symbol or `nullptr` if no symbol with that name was found.
		[[nodiscard]] PHOENIX_API symbol* find_symbol_by_name(std::string_view name);

		/// \brief Call the given callback function for every instance symbol which is a descendant of the class with
		///        the given name.
		/// \param name The name of the parent class.
		/// \param callback The function to call with each instance symbol.
		PHOENIX_API void enumerate_instances_by_class_name(std::string_view name,
		                                                   const std::function<void(symbol&)>& callback);

		/// \brief Decodes the instruction at \p address and returns it.
		/// \param address The address of the instruction to decode
		/// \return The instruction.
		[[nodiscard]] PHOENIX_API instruction instruction_at(std::uint32_t address) const;

		/// \return The total size of the script.
		[[nodiscard]] PHOENIX_API std::uint32_t size() const noexcept {
			return _m_text.limit() & 0xFFFFFF;
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		PHOENIX_API inline const symbol* find_symbol_by_instance(const instance& inst) const {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		PHOENIX_API inline symbol* find_symbol_by_instance(const instance& inst) {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		template <typename T>
		PHOENIX_API
		    typename std::enable_if<std::is_base_of_v<instance, T>, const symbol*>::type inline find_symbol_by_instance(
		        const std::shared_ptr<T>& inst) const { // clang-format on
			return find_symbol_by_index(inst->_m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		template <typename T>
		PHOENIX_API
		    typename std::enable_if<std::is_base_of_v<instance, T>, symbol*>::type inline find_symbol_by_instance(
		        const std::shared_ptr<T>& inst) {
			return find_symbol_by_index(inst->_m_symbol_index);
		}

		[[nodiscard]] PHOENIX_API std::vector<symbol*> find_class_members(symbol const& cls);

		inline void register_as_opaque(std::string_view class_name) {
			return register_as_opaque(find_symbol_by_name(class_name));
		}

		void register_as_opaque(symbol* sym);

	protected:
		PHOENIX_INTERNAL script() = default;

		template <typename _class, typename _member, int N>
		symbol* _check_member(std::string_view name, const std::type_info* type) {
			auto* sym = find_symbol_by_name(name);

			if (sym == nullptr)
				throw symbol_not_found {std::string {name}};
			if (!sym->is_member())
				throw member_registration_error {sym, "not a member"};
			if (sym->count() > N)
				throw member_registration_error {sym,
				                                 "incorrect number of elements: given " + std::to_string(N) +
				                                     " expected " + std::to_string(sym->count())};

			// check class registration
			auto* parent = find_symbol_by_index(sym->parent());
			if (parent == nullptr)
				throw member_registration_error {sym, "no parent found"};

			if (parent->_m_registered_to == nullptr) {
				parent->_m_registered_to = type;
			} else if (parent->_m_registered_to != type) {
				throw member_registration_error {sym,
				                                 "parent class is already registered with a different type (" +
				                                     std::string {parent->_m_registered_to->name()} + ")"};
			}

			// check type matches
			if constexpr (std::is_same_v<std::string, _member>) {
				if (sym->type() != datatype::string)
					throw invalid_registration_datatype {sym, "string"};
			} else if constexpr (std::is_same_v<float, _member>) {
				if (sym->type() != datatype::float_)
					throw invalid_registration_datatype {sym, "float"};
			} else if constexpr (std::is_same_v<int32_t, _member> || std::is_enum_v<_member>) {
				if (sym->type() != datatype::integer && sym->type() != datatype::function)
					throw invalid_registration_datatype {sym, "int"};
			} else {
				throw invalid_registration_datatype {sym, "<unknown>"};
			}

			return sym;
		}

		PHOENIX_API symbol* add_temporary_strings_symbol();

	private:
		std::vector<symbol> _m_symbols;
		std::unordered_map<std::string, uint32_t> _m_symbols_by_name;
		std::unordered_map<std::uint32_t, uint32_t> _m_symbols_by_address;

		mutable buffer _m_text = buffer::empty();
		std::uint8_t _m_version {0};
	};
} // namespace phoenix
