// Copyright © 2021-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Error.hh"
#include "zenkit/Library.hh"
#include "zenkit/Stream.hh"

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <variant>
#include <vector>

namespace zenkit {
	class Read;

	/// \brief Daedalus data types.
	enum class DaedalusDataType : std::uint32_t {
		VOID = 0U,      ///< A datatype similar to C++'s `void`.
		FLOAT = 1U,     ///< A 32-bit floating point type similar to C++'s `float`.
		INT = 2U,       ///< A 32-bit signed integer type like std::int32_t
		STRING = 3U,    ///< A [Windows-1252](https://en.wikipedia.org/wiki/Windows-1252) encoded character array.
		CLASS = 4U,     ///< A structure definition.
		FUNCTION = 5U,  ///< A function definition or a function pointer represented as a 32-bit signed integer.
		PROTOTYPE = 6U, ///< A prototype definition.
		INSTANCE = 7U,  ///< An instance definition or instance reference represented as a 32-bit unsigned integer.

		// Deprecated entries.
		void_ ZKREM("renamed to DaedalusDataType::VOID") = VOID,
		float_ ZKREM("renamed to DaedalusDataType::FLOAT") = FLOAT,
		integer ZKREM("renamed to DaedalusDataType::INT") = INT,
		string ZKREM("renamed to DaedalusDataType::STRING") = STRING,
		class_ ZKREM("renamed to DaedalusDataType::CLASS") = CLASS,
		function ZKREM("renamed to DaedalusDataType::FUNCTION") = FUNCTION,
		prototype ZKREM("renamed to DaedalusDataType::PROTOTYPE") = PROTOTYPE,
		instance ZKREM("renamed to DaedalusDataType::INSTANCE") = INSTANCE,
	};

	constexpr char const* const DAEDALUS_DATA_TYPE_NAMES[] = {
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
	namespace DaedalusSymbolFlag {
		static constexpr auto CONST = 1U << 0U;       ///< The symbol is not mutable.
		static constexpr auto RETURN = 1U << 1U;      ///< The symbol is a function and has a return value.
		static constexpr auto MEMBER = 1U << 2U;      ///< The symbol is a class member.
		static constexpr auto EXTERNAL = 1U << 3U;    ///< The symbol refers to an external function.
		static constexpr auto MERGED = 1U << 4U;      ///< Unused.
		static constexpr auto TRAP_ACCESS = 1U << 6U; ///< VM should call trap callback, when symbol accessed.
		static constexpr auto FUNC_LOCALS = 1U << 7U; ///< VM should call trap callback, when symbol accessed.

		// Deprecated entries.
		ZKREM("renamed to DaedalusSymbolFlag::CONST") static constexpr auto const_ = CONST;
		ZKREM("renamed to DaedalusSymbolFlag::RETURN") static constexpr auto return_ = RETURN;
		ZKREM("renamed to DaedalusSymbolFlag::MEMBER") static constexpr auto member = MEMBER;
		ZKREM("renamed to DaedalusSymbolFlag::EXTERNAL") static constexpr auto external = EXTERNAL;
		ZKREM("renamed to DaedalusSymbolFlag::MERGED") static constexpr auto merged = MERGED;
		ZKREM("renamed to DaedalusSymbolFlag::TRAP_ACCESS") static constexpr auto access_trap = TRAP_ACCESS;
	} // namespace DaedalusSymbolFlag

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
	enum class DaedalusOpcode : std::uint8_t {
		/// \brief Add `a` and `b` and put the result back onto the stack.
		ADD = 0,

		/// \brief Subtract `b` from `a` and put the result back onto the stack.
		SUB = 1,

		/// \brief Multiply `a` and `b` and put the result back onto the stack.
		MUL = 2,

		/// \brief Divide `a` by `b` and put the result back onto the stack.
		DIV = 3,

		/// \brief Divide `a` by `b` and put the remainder back onto the stack.
		MOD = 4,

		/// \brief Compute the bitwise or of `a` and `b` and put the result back onto the stack.
		OR = 5,

		/// \brief Compute the bitwise and of `a` and `b` and put the result back onto the stack.
		ANDB = 6, ///< a & b

		/// \brief Test if `a` is less than `b` and put `1` or `0` onto the stack if
		///        the test is true or false respectively.
		LT = 7,

		/// \brief Test if `a` is greater than `b` and put `1` or `0` onto the stack
		///        if the test is true or false respectively.
		GT = 8,

		/// \brief Write `b` to `x` as an integer.
		MOVI = 9,

		/// \brief Test if `a == 1` or `b == 1` and put `1` or `0` onto the stack if
		/// 		the test is true or false respectively.
		ORR = 11,

		/// \brief Test if `a == 1` and `b == 1` and put `1` or `0` onto the stack if
		///        the test is true or false respectively.
		AND = 12,

		/// \brief Left shift  `a` by `b` bits and put the result back onto the stack.
		LSL = 13,

		/// \brief Right shift  `a` by `b` bits and put the result back onto the stack.
		LSR = 14,

		/// \brief Test if `a` is less than or equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		LTE = 15,

		/// \brief Test if `a` is equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		EQ = 16,

		/// \brief Test if `a` is not equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		NEQ = 17,

		/// \brief Test if `a` is greater than or equal to `b` and put `1` or `0` onto the
		///        stack if the test is true or false respectively.
		GTE = 18,

		/// \brief Add `x` and `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		ADDMOVI = 19,

		/// \brief Subtract `b` from `x` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		SUBMOVI = 20,

		/// \brief Multiply `x` from `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		MULMOVI = 21,

		/// \brief Divide `x` by `b` and assign the result back to `x`.
		/// \note `x` must be a reference to an integer.
		DIVMOVI = 22,

		/// \brief Compute `+a` and put the result back onto the stack.
		PLUS = 30,

		/// \brief Compute `-a` and put the result back onto the stack.
		NEGATE = 31,

		/// \brief Compute `!a` and put the result back onto the stack.
		NOT = 32,

		/// \brief Compute the bitwise complement `a` and put the result back onto the stack.
		CMPL = 33,

		/// \brief Do nothing.
		NOP = 45,

		/// \brief Return from the currently running function
		RSR = 60,

		/// \brief Call the function at the address provided in the instruction.
		BL = 61,

		/// \brief Call the external function at the symbol index provided in the instruction.
		BE = 62,

		/// \brief Push the immediate value provided in the instruction onto the stack as an integer.
		PUSHI = 64,

		/// \brief Push the symbol with the index provided in the instruction onto the stack as a reference.
		PUSHV = 65,

		/// \brief Push the instance with the symbol index provided in the instruction onto the stack as a reference.
		PUSHVI = 67,

		/// \brief Write `m` to `x` as a string.
		MOVS = 70,

		/// \brief Write `m` to `x` as a string reference; not implemented.
		MOVSS = 71,

		/// \brief Write `b` to `x` as a function reference.
		MOVVF = 72,

		/// \brief Write `b` to `x` as a floating point number.
		MOVF = 73,

		/// \brief Write `y` to `x` as an instance reference.
		MOVVI = 74,

		/// \brief Immediately jump to the instruction at the address provided in the instruction.
		B = 75,

		/// \brief Jump to the instruction at the address provided in the instruction if `a == 0`.
		BZ = 76,

		/// \brief Set the global instance reference to the instance with the symbol index provided in the instrucion.
		GMOVI = 80,

		/// \brief Push the element at the given index of the symbol with the index provided in the
		///        instruction onto the stack as a reference.
		PUSHVV = 245,

		add ZKREM("renamed to DaedalusDataType::ADD") = ADD,
		sub ZKREM("renamed to DaedalusDataType::SUB") = SUB,
		mul ZKREM("renamed to DaedalusDataType::MUL") = MUL,
		div ZKREM("renamed to DaedalusDataType::DIV") = DIV,
		mod ZKREM("renamed to DaedalusDataType::MOD") = MOD,
		or_ ZKREM("renamed to DaedalusDataType::OR") = OR,
		andb ZKREM("renamed to DaedalusDataType::ANDB") = ANDB,
		lt ZKREM("renamed to DaedalusDataType::LT") = LT,
		gt ZKREM("renamed to DaedalusDataType::GT") = GT,
		movi ZKREM("renamed to DaedalusDataType::MOVI") = MOVI,
		orr ZKREM("renamed to DaedalusDataType::ORR") = ORR,
		and_ ZKREM("renamed to DaedalusDataType::ADD") = AND,
		lsl ZKREM("renamed to DaedalusDataType::LSL") = LSL,
		lsr ZKREM("renamed to DaedalusDataType::LSR") = LSR,
		lte ZKREM("renamed to DaedalusDataType::LTE") = LTE,
		eq ZKREM("renamed to DaedalusDataType::EQ") = EQ,
		neq ZKREM("renamed to DaedalusDataType::NEQ") = NEQ,
		gte ZKREM("renamed to DaedalusDataType::GTE") = GTE,
		addmovi ZKREM("renamed to DaedalusDataType::ADDMOVI") = ADDMOVI,
		submovi ZKREM("renamed to DaedalusDataType::SUBMOVI") = SUBMOVI,
		mulmovi ZKREM("renamed to DaedalusDataType::MULMOVI") = MULMOVI,
		divmovi ZKREM("renamed to DaedalusDataType::DIVMOVI") = DIVMOVI,
		plus ZKREM("renamed to DaedalusDataType::PLUS") = PLUS,
		negate ZKREM("renamed to DaedalusDataType::NEGATE") = NEGATE,
		not_ ZKREM("renamed to DaedalusDataType::NOT") = NOT,
		cmpl ZKREM("renamed to DaedalusDataType::CMPL") = CMPL,
		nop ZKREM("renamed to DaedalusDataType::NOP") = NOP,
		rsr ZKREM("renamed to DaedalusDataType::RSR") = RSR,
		bl ZKREM("renamed to DaedalusDataType::BL") = BL,
		be ZKREM("renamed to DaedalusDataType::BE") = BE,
		pushi ZKREM("renamed to DaedalusDataType::PUSHI") = PUSHI,
		pushv ZKREM("renamed to DaedalusDataType::PUSHV") = PUSHV,
		pushvi ZKREM("renamed to DaedalusDataType::PUSHVI") = PUSHVI,
		movs ZKREM("renamed to DaedalusDataType::MOVS") = MOVS,
		movss ZKREM("renamed to DaedalusDataType::MOVSS") = MOVSS,
		movvf ZKREM("renamed to DaedalusDataType::MOVVF") = MOVVF,
		movf ZKREM("renamed to DaedalusDataType::MOVF") = MOVF,
		movvi ZKREM("renamed to DaedalusDataType::MOVVI") = MOVVI,
		b ZKREM("renamed to DaedalusDataType::B") = B,
		bz ZKREM("renamed to DaedalusDataType::BZ") = BZ,
		gmovi ZKREM("renamed to DaedalusDataType::GMOVI") = GMOVI,
		pushvv ZKREM("renamed to DaedalusDataType::PUSHVV") = PUSHVV,
	};

	class DaedalusSymbol;

	/// \brief Represents an object associated with an instance in the script.
	///
	/// Every class defined in C++ that can be used as an instance has to inherit from this class.
	class ZKAPI DaedalusInstance {
	public:
		virtual ~DaedalusInstance() = default;

		/// \return The index of the symbol this instance is bound to.
		[[nodiscard]] uint32_t symbol_index() const {
			return _m_symbol_index;
		}

		[[nodiscard]] std::type_info const* instance_type() const {
			return _m_type;
		}

		/// \brief A pointer which may be used by the user of this API
		void* user_ptr = nullptr;

	protected:
		virtual std::uint8_t* data() {
			return reinterpret_cast<std::uint8_t*>(this);
		}

		[[nodiscard]] virtual std::uint8_t const* data() const {
			return reinterpret_cast<std::uint8_t const*>(this);
		}

	private:
		friend class DaedalusTransientInstance;
		friend class DaedalusSymbol;
		friend class DaedalusScript;
		friend class DaedalusVm;

		uint32_t _m_symbol_index {static_cast<uint32_t>(-1)};
		std::type_info const* _m_type {nullptr};
	};

	/// \brief Represents an object associated with an instance in the script.
	///
	/// Instances allocated with init_opaque will be backed up by this class with plain memory storage
	class DaedalusOpaqueInstance final : public DaedalusInstance {
	public:
		ZKAPI DaedalusOpaqueInstance(DaedalusSymbol const& sym, std::vector<DaedalusSymbol*> const& members);
		ZKAPI ~DaedalusOpaqueInstance() override;

	protected:
		friend class symbol;

		ZKAPI std::uint8_t* data() override {
			return _m_storage.get();
		}

		[[nodiscard]] ZKAPI std::uint8_t const* data() const override {
			return _m_storage.get();
		}

	private:
		template <typename T, typename... Args>
		ZKINT T* construct_at(size_t offset, Args&&... args);

		std::unique_ptr<std::uint8_t[]> _m_storage;
		std::vector<std::string*> _m_strings;
	};

	/// \brief Represents object instance in the script with no defined backing to memory.
	///
	/// Expected to be used for DMA mods or to emulate variable-like access to engine-functions.
	class ZKAPI DaedalusTransientInstance : public DaedalusInstance {
	public:
		DaedalusTransientInstance();

	protected:
		friend class DaedalusSymbol;

		virtual void set_int(DaedalusSymbol const& sym, uint16_t index, std::int32_t value) = 0;
		[[nodiscard]] virtual std::int32_t get_int(DaedalusSymbol const& sym, uint16_t index) const = 0;

		virtual void set_float(DaedalusSymbol const& sym, uint16_t index, float value) = 0;
		[[nodiscard]] virtual float get_float(DaedalusSymbol const& sym, uint16_t index) const = 0;

		virtual void set_string(DaedalusSymbol const& sym, uint16_t index, std::string_view value) = 0;
		[[nodiscard]] virtual std::string const& get_string(DaedalusSymbol const& sym, uint16_t index) const = 0;
	};

	/// \brief The base class for all exceptions thrown by interacting with a script.
	struct DaedalusScriptError : Error {
		using Error::Error;
	};

	/// \brief An exception thrown if the symbol with a given name could not be found.
	struct DaedalusSymbolNotFound : DaedalusScriptError {
		ZKAPI explicit DaedalusSymbolNotFound(std::string&& name);

		std::string name;
	};

	/// \brief An exception thrown if registering a class member was unsuccessful.
	struct DaedalusMemberRegistrationError : DaedalusScriptError {
		ZKAPI explicit DaedalusMemberRegistrationError(DaedalusSymbol const* sym, std::string&& message);

		/// \brief The symbol being registered.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if the type of the member being registered does not match the type provided.
	struct DaedalusInvalidRegistrationDataType final : DaedalusMemberRegistrationError {
		ZKAPI explicit DaedalusInvalidRegistrationDataType(DaedalusSymbol const* sym, std::string&& given);

		std::string given;
	};

	/// \brief An exception thrown when the value of a symbol accessed in a way which is not permissible.
	struct DaedalusIllegalAccess : DaedalusScriptError {
		using DaedalusScriptError::DaedalusScriptError;
	};

	/// \brief An exception thrown when the type of a symbol does not match the type expected.
	struct DaedalusIllegalTypeAccess final : DaedalusIllegalAccess {
		ZKINT DaedalusIllegalTypeAccess(DaedalusSymbol const* sym, DaedalusDataType expected);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The datatype excepted.
		DaedalusDataType expected;
	};

	/// \brief An exception thrown when an out-of-bounds index is accessed.
	struct DaedalusIllegalIndexAccess final : DaedalusIllegalAccess {
		ZKINT DaedalusIllegalIndexAccess(DaedalusSymbol const* sym, std::size_t index);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The index being accessed
		std::size_t index;
	};

	/// \brief An exception thrown when a constant symbol is accessed as mutable
	struct DaedalusIllegalConstAccess final : DaedalusIllegalAccess {
		ZKINT explicit DaedalusIllegalConstAccess(DaedalusSymbol const* sym);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown when the parent class of a member does not match the class of an instance.
	struct DaedalusIllegalInstanceAccess final : DaedalusIllegalAccess {
		ZKINT DaedalusIllegalInstanceAccess(DaedalusSymbol const* sym, std::uint32_t expected_parent);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The parent which was expected for the member.
		std::uint32_t expected_parent;
	};

	/// \brief An exception thrown when the parent class of a member does not match the class of an instance.
	struct DaedalusUnboundMemberAccess final : DaedalusIllegalAccess {
		ZKAPI explicit DaedalusUnboundMemberAccess(DaedalusSymbol const* sym);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if a member symbol is being access without a context set.
	struct DaedalusNoContextError final : DaedalusIllegalAccess {
		ZKINT explicit DaedalusNoContextError(DaedalusSymbol const* sym);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;
	};

	/// \brief An exception thrown if a member symbol is being accessed with a context instance it is not bound to.
	struct DaedalusIllegalContextType final : DaedalusIllegalAccess {
		ZKAPI DaedalusIllegalContextType(DaedalusSymbol const* sym, std::type_info const& context_type);

		/// \brief The symbol being accessed.
		DaedalusSymbol const* sym;

		/// \brief The type of context currently set.
		std::type_info const& context_type;
	};

	/// \brief Represents a compiled daedalus symbol.
	class DaedalusSymbol final {
	public:
		ZKINT DaedalusSymbol() = default;

		ZKAPI void load(Read* in);

		/// \brief Validates that the symbol is a string and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The string associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_string.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		[[nodiscard]] ZKAPI std::string const& get_string(std::uint16_t index = 0,
		                                                  DaedalusInstance const* context = nullptr) const;

		/// \brief Validates that the symbol is a float and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The float value associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_float.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		[[nodiscard]] ZKAPI float get_float(std::uint16_t index = 0, DaedalusInstance const* context = nullptr) const;

		/// \brief Validates that the symbol is an int and retrieves it's value in the given context.
		/// \param index The index of the value to get.
		/// \param context An instance to use as context for getting member variables.
		/// \return The int value associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_int or dt_function.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		[[nodiscard]] ZKAPI std::int32_t get_int(std::uint16_t index = 0,
		                                         DaedalusInstance const* context = nullptr) const;

		/// \brief Validates that the symbol is an instance and retrieves it's value
		/// \return The instance associated with the symbol.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_instance
		[[nodiscard]] ZKAPI std::shared_ptr<DaedalusInstance> const& get_instance();

		// -=-= Value setters =-=- //

		/// \brief Validates that the symbol is a string and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_string.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		ZKAPI void set_string(std::string_view value, std::uint16_t index = 0, DaedalusInstance* context = nullptr);

		/// \brief Validates that the symbol is a float and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_float
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		ZKAPI void set_float(float value, std::uint16_t index = 0, DaedalusInstance* context = nullptr);

		/// \brief Validates that the symbol is an int and not constant and sets it's value in the given context.
		/// \param value The new value to set.
		/// \param index The index of the value to set
		/// \param context An instance to use as context for setting member variables.
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_int or dt_function.
		/// \throws DaedalusIllegalIndexAccess if \p index >= #count.
		/// \throws DaedalusNoContextError if this symbol #is_member and \p context is `nullptr`.
		/// \throws DaedalusUnboundMemberAccess if this symbol has not been registered yet
		/// \throws DaedalusIllegalContextType if this symbol #is_registered_to a different type than the type of \p
		/// context.
		ZKAPI void set_int(std::int32_t value, std::uint16_t index = 0, DaedalusInstance* context = nullptr);

		/// \brief Validates that the symbol is an instance and sets it's value
		/// \param inst The instance value to set
		/// \throws DaedalusIllegalTypeAccess if the #type of this symbol is not dt_instance.
		ZKAPI void set_instance(std::shared_ptr<DaedalusInstance> const& inst);

		/// \brief Tests whether this symbol holds an instance of the given type.
		/// \tparam T The type of instance to check for.
		/// \return <tt>true</tt> if the symbol contains an instance of the given type, <tt>false</tt> if not.
		template <typename T>
		    requires std::derived_from<T, DaedalusInstance>
		ZKAPI bool is_instance_of() {
			return this->type() == DaedalusDataType::INSTANCE && this->get_instance() != nullptr &&
			    this->get_instance()->_m_type == &typeid(T);
		}

		/// \brief Allows VM traps on access to this symbol
		/// \param enable true to enable and false to disable
		ZKAPI void set_access_trap_enable(bool enable) noexcept;

		/// \brief Allows function associated with this symbol to stash local variables on a stack during recursion.
		/// \param enable true to enable and false to disable
		ZKAPI void set_local_variables_enable(bool enable) noexcept;

		/// \brief Tests whether the symbol is a constant.
		/// \return `true` if the symbol is a constant, `false` if not.
		[[nodiscard]] ZKAPI bool is_const() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::CONST) != 0;
		}

		/// \brief Tests whether the symbol is a member variable.
		/// \return `true` if the symbol is a member, `false` if not.
		[[nodiscard]] ZKAPI bool is_member() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::MEMBER) != 0;
		}

		/// \brief Tests whether the symbol is an extern symbol.
		/// \return `true` if the symbol is an extern symbol, `false` if not.
		[[nodiscard]] ZKAPI bool is_external() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::EXTERNAL) != 0;
		}

		/// \brief Tests whether the symbol is merged.
		/// \return `true` if the symbol is merged, `false` if not.
		/// \note It is currently not known what 'merged' means.
		[[nodiscard]] ZKAPI bool is_merged() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::MERGED) != 0;
		}

		/// \brief Tests whether the symbol has access trap.
		/// \return `true` if the symbol has trap enabled, `false` if not.
		[[nodiscard]] ZKAPI bool has_access_trap() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::TRAP_ACCESS) != 0;
		}
		/// \brief brief Tests whether the symbol is a compiler-generated symbol
		/// \return return `true` if the symbol is generated, `false` if not.
		[[nodiscard]] ZKAPI bool is_generated() const noexcept {
			return _m_generated;
		}

		/// \brief brief Tests whether the symbol has a return value.
		/// \return return `true` if the symbol has a return value, `false` if not.
		[[nodiscard]] ZKAPI bool has_return() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::RETURN) != 0;
		}

		/// \brief brief Tests whether the symbol has local-variables enabled.
		/// \return return `true` if enabled, `false` if not.
		[[nodiscard]] ZKAPI bool has_local_variables_enabled() const noexcept {
			return (_m_flags & DaedalusSymbolFlag::FUNC_LOCALS) != 0;
		}

		/// \return The name of the symbol.
		[[nodiscard]] ZKAPI std::string const& name() const noexcept {
			return _m_name;
		}

		/// \return The address of the symbol.
		[[nodiscard]] ZKAPI std::uint32_t address() const noexcept {
			return static_cast<uint32_t>(_m_address);
		}

		/// \return The index of the parent symbol or unset if the symbol does not have a parent.
		[[nodiscard]] ZKAPI std::uint32_t parent() const noexcept {
			return static_cast<uint32_t>(_m_parent);
		}

		/// \return The count of values stored in the symbol.
		[[nodiscard]] ZKAPI std::uint32_t count() const noexcept {
			return _m_count;
		}

		/// \return The type of the symbol.
		[[nodiscard]] ZKAPI DaedalusDataType type() const noexcept {
			return _m_type;
		}

		/// \return The index of the symbol.
		[[nodiscard]] ZKAPI std::uint32_t index() const noexcept {
			return _m_index;
		}

		/// \return The return type of the symbol.
		[[nodiscard]] ZKAPI DaedalusDataType rtype() const noexcept {
			return _m_return_type;
		}

		/// \return The index of the file the symbol was in.
		[[nodiscard]] ZKAPI std::uint32_t file_index() const noexcept {
			return _m_file_index;
		}

		/// \return The offset in bytes of a member from the start of the instance.
		[[nodiscard]] ZKAPI std::uint32_t offset_as_member() const noexcept {
			return _m_member_offset;
		}

		[[nodiscard]] ZKAPI std::uint32_t line_start() const noexcept {
			return _m_line_start;
		}

		[[nodiscard]] ZKAPI std::uint32_t line_count() const noexcept {
			return _m_line_count;
		}

		[[nodiscard]] ZKAPI std::uint32_t char_start() const noexcept {
			return _m_char_start;
		}

		[[nodiscard]] ZKAPI std::uint32_t char_count() const noexcept {
			return _m_char_count;
		}

		[[nodiscard]] ZKAPI std::uint32_t class_size() const noexcept {
			return _m_class_size;
		}

		[[nodiscard]] ZKAPI std::type_info const& registered_to() const noexcept {
			return *_m_registered_to;
		}

		ZKAPI void grow(uint32_t n);

	protected:
		template <typename T>
		T const* get_member_ptr(std::uint16_t index, DaedalusInstance const* context) const {
			if (!_m_registered_to) throw DaedalusUnboundMemberAccess(this);
			if (*_m_registered_to != *context->_m_type) throw DaedalusIllegalContextType {this, *context->_m_type};

			auto data_ptr = context->data();
			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<T const*>(data_ptr + target_offset);
		}

		template <typename T>
		T* get_member_ptr(std::uint16_t index, DaedalusInstance* context) {
			if (!_m_registered_to) throw DaedalusUnboundMemberAccess(this);
			if (*_m_registered_to != *context->_m_type) throw DaedalusIllegalContextType {this, *context->_m_type};

			auto data_ptr = context->data();
			std::uint32_t target_offset = offset_as_member() + index * sizeof(T);
			return reinterpret_cast<T*>(data_ptr + target_offset);
		}

	private:
		friend class DaedalusScript;
		std::string _m_name;
		std::variant<std::unique_ptr<std::int32_t[]>,
		             std::unique_ptr<float[]>,
		             std::unique_ptr<std::string[]>,
		             std::shared_ptr<DaedalusInstance>>
		    _m_value;

		std::int32_t _m_address {-1};
		std::int32_t _m_parent {-1};
		std::int32_t _m_class_offset {-1};
		std::uint32_t _m_count {0};
		DaedalusDataType _m_type {0};
		std::uint32_t _m_flags {0};
		bool _m_generated {false};

		std::uint32_t _m_file_index {0};
		std::uint32_t _m_line_start {0};
		std::uint32_t _m_line_count {0};
		std::uint32_t _m_char_start {0};
		std::uint32_t _m_char_count {0};

		std::uint32_t _m_member_offset {static_cast<uint32_t>(-1)};
		std::uint32_t _m_class_size {static_cast<uint32_t>(-1)};
		DaedalusDataType _m_return_type {DaedalusDataType::VOID};
		std::uint32_t _m_index {static_cast<uint32_t>(-1)};
		std::type_info const* _m_registered_to {nullptr};
	};

	/// \brief Represents a daedalus VM instruction.
	struct DaedalusInstruction {
		DaedalusOpcode op {DaedalusOpcode::NOP};
		std::uint32_t address {0};
		std::uint32_t symbol {0};
		std::int32_t immediate {0};
		std::uint8_t index {0};
		std::uint8_t size {1};

		/// \brief Reads an instruction from a reader.
		/// \param[in,out] r The reader to read from
		/// \return The instruction read.
		ZKINT static DaedalusInstruction decode(Read* r);
	};

	template <typename T>
	concept DaedalusValue = std::same_as<T, std::string> || std::same_as<T, float> || std::same_as<T, int32_t> ||
	    (std::is_enum_v<T> && sizeof(T) == 4);

	/// \brief Represents a compiled daedalus script
	class DaedalusScript {
	public:
		ZKAPI DaedalusScript() = default;
		ZKAPI DaedalusScript(DaedalusScript const& copy) = delete;
		ZKAPI DaedalusScript(DaedalusScript&& move) = default;

		ZKAPI void load(Read* r);

		/// \brief Registers a member offset
		/// \param name The name of the member in the script
		/// \param field The field to register
		/// \throws DaedalusSymbolNotFound if there is no symbol with the given name.
		/// \throws DaedalusMemberRegistrationError if the member could not be registered
		/// \throws DaedalusInvalidRegistrationDataType If the datatype of \p _member is different than that of the
		/// symbol.
		template <typename _class, typename _member, int N>
		    requires DaedalusValue<_member>
		void register_member(std::string_view name, _member (_class::*field)[N]) {
			auto* type = &typeid(_class);
			auto* sym = _check_member<_member, N>(name, type);

			_class* base = nullptr;
			auto member = &(base->*field);
			sym->_m_member_offset = reinterpret_cast<std::uint64_t>(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/// \brief Registers a member offset
		/// \param name The name of the member in the script
		/// \param field The field to register
		/// \throws DaedalusSymbolNotFound if there is no symbol with the given name.
		/// \throws DaedalusMemberRegistrationError if the member could not be registered
		/// \throws DaedalusInvalidRegistrationDataType If the datatype of \p _member is different than that of the
		/// symbol.
		template <typename _class, typename _member>
		    requires DaedalusValue<_member>
		void register_member(std::string_view name, _member _class::* field) {
			auto* type = &typeid(_class);
			auto* sym = _check_member<_member, 1>(name, type);

			_class* base = nullptr;
			auto member = &(base->*field);
			sym->_m_member_offset = reinterpret_cast<std::uint64_t>(member) & 0xFFFFFFFF;
			sym->_m_registered_to = type;
		}

		/// \return All symbols in the script
		[[nodiscard]] ZKAPI std::vector<DaedalusSymbol> const& symbols() const noexcept {
			return _m_symbols;
		}

		/// \brief Retrieves the symbol with the given \p index
		/// \param index The index of the symbol to get
		/// \return The symbol or `nullptr` if the index was out-of-range.
		[[nodiscard]] ZKAPI DaedalusSymbol const* find_symbol_by_index(std::uint32_t index) const;

		/// \brief Looks for parameters of the given function symbol. Only works for external functions.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function parameter symbols.
		[[nodiscard]] ZKAPI std::span<DaedalusSymbol const>
		find_parameters_for_function(DaedalusSymbol const* parent) const;

		/// \brief Retrieves the symbol with the given \p address set
		/// \param address The address of the symbol to get
		/// \return The symbol or `nullptr` if no symbol with that address was found.
		[[nodiscard]] ZKAPI DaedalusSymbol const* find_symbol_by_address(std::uint32_t address) const;

		/// \brief Retrieves the symbol with the given \p name.
		/// \param name The name of the symbol to get.
		/// \return The symbol or `nullptr` if no symbol with that name was found.
		[[nodiscard]] ZKAPI DaedalusSymbol const* find_symbol_by_name(std::string_view name) const;

		/// \brief Retrieves the symbol with the given \p index
		/// \param index The index of the symbol to get
		/// \return The symbol or `nullptr` if the index was out-of-range.
		[[nodiscard]] ZKAPI DaedalusSymbol* find_symbol_by_index(std::uint32_t index);

		/// \brief Retrieves the symbol with the given \p address set
		/// \param address The address of the symbol to get
		/// \return The symbol or `nullptr` if no symbol with that address was found.
		[[nodiscard]] ZKAPI DaedalusSymbol* find_symbol_by_address(std::uint32_t address);

		/// \brief Looks for parameters of the given function symbol. Only works for external functions.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function parameter symbols.
		[[nodiscard]] ZKAPI std::span<DaedalusSymbol> find_parameters_for_function(DaedalusSymbol const* parent);

		/// \brief Looks for local-variables of the given function symbol.
		/// \param parent The function symbol to get the parameter symbols for.
		/// \return A list of function local-variable symbols.
		[[nodiscard]] ZKAPI std::span<DaedalusSymbol> find_locals_for_function(DaedalusSymbol const* parent);

		/// \brief Retrieves the symbol with the given \p name.
		/// \param name The name of the symbol to get.
		/// \return The symbol or `nullptr` if no symbol with that name was found.
		[[nodiscard]] ZKAPI DaedalusSymbol* find_symbol_by_name(std::string_view name);

		/// \brief Call the given callback function for every instance symbol which is a descendant of the class with
		///        the given name.
		/// \param name The name of the parent class.
		/// \param callback The function to call with each instance symbol.
		ZKAPI void enumerate_instances_by_class_name(std::string_view name,
		                                             std::function<void(DaedalusSymbol&)> const& callback);

		/// \brief Decodes the instruction at \p address and returns it.
		/// \param address The address of the instruction to decode
		/// \return The instruction.
		[[nodiscard]] ZKAPI DaedalusInstruction instruction_at(std::uint32_t address) const;

		/// \return The total size of the script.
		[[nodiscard]] ZKAPI std::uint32_t size() const noexcept;

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		ZKAPI DaedalusSymbol const* find_symbol_by_instance(DaedalusInstance const& inst) const {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		ZKAPI DaedalusSymbol* find_symbol_by_instance(DaedalusInstance const& inst) {
			return find_symbol_by_index(inst._m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		template <typename T>
		    requires std::derived_from<T, DaedalusInstance>
		ZKAPI DaedalusSymbol const* find_symbol_by_instance(std::shared_ptr<T> const& inst) const {
			return find_symbol_by_index(inst->_m_symbol_index);
		}

		/// \brief Finds the symbol the given instance is currently bound to.
		/// \param inst The instance to get the symbol for.
		/// \return The symbol associated with that instance or <tt>nullptr</tt> if the symbol is not associated
		///         with any instance.
		template <typename T>
		    requires std::derived_from<T, DaedalusInstance>
		ZKAPI DaedalusSymbol* find_symbol_by_instance(std::shared_ptr<T> const& inst) {
			return find_symbol_by_index(inst->_m_symbol_index);
		}

		[[nodiscard]] ZKAPI std::vector<DaedalusSymbol*> find_class_members(DaedalusSymbol const& cls);

		ZKAPI void register_as_opaque(std::string_view class_name) {
			return register_as_opaque(find_symbol_by_name(class_name));
		}

		void register_as_opaque(DaedalusSymbol* sym);

	protected:
		template <typename _member, int N>
		DaedalusSymbol* _check_member(std::string_view name, std::type_info const* type) {
			auto* sym = find_symbol_by_name(name);

			if (sym == nullptr) throw DaedalusSymbolNotFound {std::string {name}};
			if (!sym->is_member()) throw DaedalusMemberRegistrationError {sym, "not a member"};
			if (sym->count() > N)
				throw DaedalusMemberRegistrationError {sym,
				                                       "incorrect number of elements: given " + std::to_string(N) +
				                                           " expected " + std::to_string(sym->count())};

			// check class registration
			auto* parent = find_symbol_by_index(sym->parent());
			if (parent == nullptr) throw DaedalusMemberRegistrationError {sym, "no parent found"};

			if (parent->_m_registered_to == nullptr) {
				parent->_m_registered_to = type;
			} else if (parent->_m_registered_to != type) {
				throw DaedalusMemberRegistrationError {sym,
				                                       "parent class is already registered with a different type (" +
				                                           std::string {parent->_m_registered_to->name()} + ")"};
			}

			// check type matches
			if constexpr (std::same_as<std::string, _member>) {
				if (sym->type() != DaedalusDataType::STRING) throw DaedalusInvalidRegistrationDataType {sym, "string"};
			} else if constexpr (std::same_as<float, _member>) {
				if (sym->type() != DaedalusDataType::FLOAT) throw DaedalusInvalidRegistrationDataType {sym, "float"};
			} else if constexpr (std::same_as<int32_t, _member> || std::is_enum_v<_member>) {
				if (sym->type() != DaedalusDataType::INT && sym->type() != DaedalusDataType::FUNCTION)
					throw DaedalusInvalidRegistrationDataType {sym, "int"};
			} else {
				throw DaedalusInvalidRegistrationDataType {sym, "<unknown>"};
			}

			return sym;
		}

		ZKAPI DaedalusSymbol* add_temporary_strings_symbol();

	private:
		std::vector<DaedalusSymbol> _m_symbols;
		std::unordered_map<std::string, uint32_t> _m_symbols_by_name;
		std::unordered_map<std::uint32_t, uint32_t> _m_symbols_by_address;

		mutable std::unique_ptr<Read> _m_text;
		std::uint8_t _m_version {0};
	};
} // namespace zenkit
