// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/daedalus/script.hh>

#include <flags.h>
#include <fmt/format.h>

#include <fstream>
#include <string>
#include <string_view>

#include "config.hh"
#include "decompiler.hh"

using namespace phoenix::daedalus;

static constexpr const auto HELP_MESSAGE =
    R"(USAGE
    zscript -v
    zscript -h
    zscript -t [-f FILE] [-I FLAGS] [-E FLAGS] [-f TERM] [-s NAME]
    zscript -d [-f FILE] [-s NAME]
    zscript -u -s NAME [-f FILE]
    zscript -k -s NAME [-f FILE]

DESCRIPTION
    Dumps symbols and bytecode from compiled Daedalus script files. Can also
    decompile function, prototype and instance code.

OPTIONS
    -v, --version               Print the version of zdump
    -h, --help                  Print this help message
    -f FILE, --input FILE       If specified, reads the given file from disk instead of stdin
                                (unless -e is specified).
    -s NAME, --symbol NAME      Dump data only for the symbol with the given NAME
    -I FLAGS, --include FLAGS   Only display symbols with any of these FLAGS
    -E FLAGS, --exclude FLAGS   Only display symbols without any of these FLAGS
    -c TERM, --find TERM        Only display symbols where the name contains TERM
    -t, --symbols               Display a list of symbols filtered by the given parameters or
                                information about the symbol passed via -s.
    -d, --disassemble           Print a disassembly of the file or the symbol passed via -s.
    -u, --usages                Print a list of all bytecode instructions referencing the symbol
                                passed via -s.
    -k, --decompile             Print a decompilation of the symbol passed via -s.


FLAGS
    c - const symbols
    r - symbols that have a return value
    m - member symbol_name
    e - extern symbols
    M - merged symbols
    g - generated symbols
    v - symbols of type void
    f - symbols of type float
    i - symbols of type int
    s - symbols of type string
    C - classes
    F - functions
    P - prototypes
    I - instances

VERSION
    phoenix zscript v{})";

#define PRINT_FLAG(cond, flag)                                                                                         \
	if (cond) {                                                                                                        \
		fmt::print("{}", flag);                                                                                        \
	} else {                                                                                                           \
		fmt::print(" ");                                                                                               \
	}

#define SV_CONTAINS(sv, chr) ((sv).find(chr) != std::string_view::npos)

void print_assembly_of_symbol(const script& scr, const symbol& sym);
void print_assembly(const script& scr);

void print_symbol_detailed(const script& scr, const symbol& sym);
void print_symbol_list(const script& scr,
                       std::string_view include_filter,
                       std::string_view exclude_filter,
                       std::string_view search);

std::string print_definition(const script& scr, const symbol& sym, const symbol* parent, std::string_view indent = "");
void find_usages(const script& scr, const symbol& sym);

int main(int argc, char** argv) {
	const flags::args args {argc, argv};
	phoenix::logging::use_default_logger();

	std::optional<std::string> input {};
	std::optional<std::string> symbol_name {};
	std::optional<std::string> include {};
	std::optional<std::string> exclude {};
	std::optional<std::string> search {};
	bool action_symbolize = false;
	bool action_disassemble = false;
	bool action_usages = false;
	bool action_decompile = false;

	action_symbolize = args.get<bool>("t") || args.get<bool>("symbols");
	action_disassemble = args.get<bool>("d") || args.get<bool>("disassemble");
	action_usages = args.get<bool>("u") || args.get<bool>("usages");
	action_decompile = args.get<bool>("k") || args.get<bool>("decompile");

	input = args.get<std::string>("f");
	if (!input)
		input = args.get<std::string>("file");

	search = args.get<std::string>("c");
	if (!search)
		search = args.get<std::string>("find");

	include = args.get<std::string>("I");
	if (!include)
		include = args.get<std::string>("include");

	exclude = args.get<std::string>("E");
	if (!exclude)
		exclude = args.get<std::string>("exclude");

	symbol_name = args.get<std::string>("s");
	if (!symbol_name)
		symbol_name = args.get<std::string>("symbol");

	if (args.get<bool>("v") || args.get<bool>("version")) {
		fmt::print("zscript v{}\n", ZSCRIPT_VERSION);
		return EXIT_SUCCESS;
	} else if (args.get<bool>("h") || args.get<bool>("help")) {
		fmt::print(HELP_MESSAGE, ZSCRIPT_VERSION);
		return EXIT_SUCCESS;
	} else {
		if (!input) {
			fmt::print(stderr, "no input file given");
			return EXIT_FAILURE;
		}

		auto scr = script::parse(*input);
		const symbol* sym = nullptr;

		if (symbol_name) {
			sym = scr.find_symbol_by_name(*symbol_name);

			if (sym == nullptr) {
				fmt::print("symbol with name {} not found\n", *symbol_name);
				return EXIT_FAILURE;
			}
		}

		if (action_symbolize) {
			if (symbol_name) {
				print_symbol_detailed(scr, *sym);
			} else {
				print_symbol_list(scr, include.value_or(""), exclude.value_or(""), search.value_or(""));
			}
		} else if (action_disassemble) {
			if (symbol_name) {
				print_assembly_of_symbol(scr, *sym);
			} else {
				print_assembly(scr);
			}
		} else if (action_usages) {
			if (!symbol_name) {
				fmt::print(stderr, "Please provide a symbol of which to find usages.");
				return EXIT_FAILURE;
			}

			find_usages(scr, *sym);
		} else if (action_decompile) {
			if (symbol_name) {
				fmt::print("{}", print_definition(scr, *sym, scr.find_symbol_by_index(sym->parent())));
				fmt::print(" {{\n{}}}\n", decompile(scr, *sym, 4));
			} else {
				std::unordered_map<uint32_t, std::string> files;
				uint32_t skip = 0;

				for (auto& s : scr.symbols()) {
					if (s.name() == "$INSTANCE_HELP" || s.is_generated())
						continue;

					if (skip > 0) {
						skip--;
						continue;
					}

					if (files.find(s.file_index()) == files.end()) {
						files[s.file_index()] = {};
					}

					std::string def = print_definition(scr, s, scr.find_symbol_by_index(s.parent()));

					if (!s.is_member() && !s.is_external() &&
					    (s.type() == phoenix::daedalus::dt_prototype ||
					     (s.type() == phoenix::daedalus::dt_function && s.is_const()) ||
					     (s.type() == phoenix::daedalus::dt_instance && s.is_const()))) {
						def += fmt::format(" {{\n{}}}\n", decompile(scr, s, 4));
					}

					if (s.type() == dt_function) {
						skip = s.count();
					}

					files[s.file_index()] += def + "\n";
				}

				for (auto& idx : files) {
					std::ofstream out {std::to_string(idx.first) + ".d"};
					out << idx.second;
					out.close();
				}
			}
		}
	}

	return 0;
}

/// \brief Tests whether the symbol has a value related to it
/// \param symbol The symbol to test on
/// \return `true` if the symbol has a value `false` otherwise.
bool has_constant_value(const symbol& symbol) {
	return symbol.is_const() && symbol.type() != dt_prototype && symbol.type() != dt_instance &&
	    symbol.type() != dt_function;
}

constexpr std::string_view get_type_name(datatype tp) {
	switch (tp) {
	case dt_void:
		return "void";
	case dt_float:
		return "float";
	case dt_integer:
		return "int";
	case dt_string:
		return "string";
	case dt_class:
		return "class";
	case dt_function:
		return "func";
	case dt_prototype:
		return "prototype";
	case dt_instance:
		return "instance";
	default:
		return "*ERR*";
	}
}

/// \brief Prints the value of a symbol
/// \param symbol The symbol to print the value of
std::string print_symbol_value(const symbol& symbol, int index = -1) {
	std::string val {};
	auto print_value = [&](uint8_t index) {
		switch (symbol.type()) {
		case dt_float:
			val += fmt::format("{}", symbol.get_float(index));
			break;
		case dt_integer:
			val += fmt::format("{}", symbol.get_int(index));
			break;
		case dt_string:
			val += fmt::format("\"{}\"", symbol.get_string(index));
			break;
		default:
			break;
		}
	};

	if (index >= 0) {
		print_value(index);
	} else {
		if (symbol.count() > 1)
			val += "{";

		for (unsigned i = 0; i < symbol.count(); ++i) {
			if (i > 0)
				val += ", ";
			print_value(i);
		}

		if (symbol.count() > 1) {
			val += "}";
		}
	}

	return val;
}

/// \brief Generates a daedalus definition from the given symbol.
/// \param scr The script to reference
/// \param sym The symbol to generate a definition of
/// \param parent The parent of the symbol
/// \param indent A custom indentation to prepend
std::string print_definition(const script& scr, const symbol& sym, const symbol* parent, std::string_view indent) {
	std::string def {};
	if (sym.is_member())
		return def;
	def += indent;
	if (sym.is_external())
		def += "extern ";

	if (sym.type() == dt_instance) {
		def += fmt::format("instance {}({})", sym.name(), (parent == nullptr ? "*ERR*" : parent->name()));
	} else if (sym.type() == dt_prototype) {
		def += fmt::format("prototype {}({})", sym.name(), (parent == nullptr ? "*ERR*" : parent->name()));
	} else if (sym.type() == dt_class) {
		def += fmt::format("class {} {{\n", sym.name());

		for (const auto& member : scr.symbols()) {
			if (member.parent() == sym.index() && member.is_member()) {
				auto name = member.name().substr(member.name().find('.') + 1);
				def += fmt::format("{}\tvar {} {}", indent, get_type_name(member.type()), name);
				if (member.count() > 1)
					def += fmt::format("[{}]", member.count());
				def += ";\n";
			}
		}

		def += fmt::format("{}}};", indent);
	} else if (sym.type() == dt_function) {
		def += fmt::format("func {} {}(", get_type_name(sym.rtype()), sym.name());

		auto params = scr.find_parameters_for_function(&sym);
		unsigned count = 0;
		for (const auto& param : params) {
			if (count > 0)
				def += ", ";

			if (param->type() == dt_instance) {
				const auto* dt = scr.find_symbol_by_index(param->parent());

				if (dt == nullptr) {
					def += "var instance ";
				} else {
					def += fmt::format("var {} ", dt->name());
				}
			} else {
				def += fmt::format("var {} ", get_type_name(param->type()));
			}

			def += param->name().substr(param->name().find('.') + 1);
			count++;
		}

		def += ")";
	} else {
		def += fmt::format("const {} {}", get_type_name(sym.type()), sym.name());
		if (sym.count() > 1)
			def += fmt::format("[{}]", sym.count());
		def += " = " + print_symbol_value(sym) + ";";
	}

	return def;
}

/// \brief Print detailed information about a symbol
/// \param scr The script to reference
/// \param sym The symbol to print details of
void print_symbol_detailed(const script& scr, const symbol& sym) {
	const auto* parent = scr.find_symbol_by_index(sym.parent());

	fmt::print("{:0>8x} <{}>\n", sym.index(), sym.name());
	fmt::print("\tName: {}\n", sym.name());
	fmt::print("\tAddress: {:x}\n", sym.address());
	fmt::print("\tSize: {}\n", sym.count());
	fmt::print("\tType: {}\n", get_type_name(sym.type()));
	fmt::print("\tGenerated: {}\n", (sym.is_generated() ? "Yes" : "No"));
	fmt::print("\tFlags:\n");
	fmt::print("\t\tConst: {}\n", (sym.is_const() ? "Yes" : "No"));
	fmt::print("\t\tReturn: {}\n", (sym.has_return() ? "Yes" : "No"));
	fmt::print("\t\tMember: {}\n", (sym.is_member() ? "Yes" : "No"));
	fmt::print("\t\tExtern: {}\n", (sym.is_external() ? "Yes" : "No"));
	fmt::print("\t\tMerged: {}\n", (sym.is_merged() ? "Yes" : "No"));
	fmt::print("\tFile Index: {}\n", sym.file_index());
	fmt::print("\tLine Start: {}\n", sym.line_start());
	fmt::print("\tLine Count: {}\n", sym.line_count());
	fmt::print("\tChar Start: {}\n", sym.char_start());
	fmt::print("\tChar Count: {}\n", sym.char_count());

	if (parent != nullptr) {
		fmt::print("\tParent: {} ({:x})\n", parent->name(), parent->index());
	}

	if (sym.is_member()) {
		fmt::print("\tVariable Offset: {}\n", sym.offset_as_member());
	}

	if (sym.type() == dt_class) {
		fmt::print("\tClass Size: {}\n", sym.class_size());
	}

	if (sym.is_const() && sym.type() == dt_function) {
		fmt::print("\tReturn Type: {}\n", get_type_name(sym.rtype()));
	}

	if (!sym.is_member()) {
		fmt::print("\tDefinition:\n{}\n", print_definition(scr, sym, parent, "\t\t"));
	}
}

/// \brief Gets the one-char abbreviation of a datatype
/// \param type The type to get the abbreviation of
/// \return The abbreviation
constexpr char get_type_abbrev(datatype tp) {
	switch (tp) {
	case dt_void:
		return 'v';
	case dt_float:
		return 'f';
	case dt_integer:
		return 'i';
	case dt_string:
		return 's';
	case dt_class:
		return 'C';
	case dt_function:
		return 'F';
	case dt_prototype:
		return 'P';
	case dt_instance:
		return 'I';
	}

	return '?';
}

/// \brief Print a string containing all the given symbols flags
/// \param sym The symbol to print the flags of
void print_symbol_flags(const symbol& sym) {
	fmt::print("{}", get_type_abbrev(sym.type()));

	PRINT_FLAG(sym.is_const(), 'c');
	PRINT_FLAG(sym.has_return(), 'r');
	PRINT_FLAG(sym.is_member(), 'm');
	PRINT_FLAG(sym.is_external(), 'e');
	PRINT_FLAG(sym.is_merged(), 'M');
	PRINT_FLAG(sym.is_generated(), 'g');
}

/// \brief Prints the name of the symbol's parent symbol if it exists
/// \param sym The symbol to print the parent of
/// \param scr The script to look up the parent in.
void print_symbol_parent(const symbol& sym, const script& scr) {
	if (sym.parent() != unset) {
		const auto* parent = scr.find_symbol_by_index(sym.parent());

		if (parent == nullptr) {
			fmt::print("{: <25}", "*ERR*");
		} else {
			fmt::print("{: <25}", parent->name());
		}
	} else {
		fmt::print("{: <25}", "*UND*");
	}
}

/// \brief Tests if a symbol matches the given filter
/// \param sym the symbol to test
/// \param include_filter Filter of flags to include
/// \param exclude_filter Filter of flags to exclude
/// \return `true` if the filters match, `false` otherwise.
bool symbol_matches_filter(const symbol& sym, std::string_view include_filter, std::string_view exclude_filter) {
	bool matches_include = true;
	bool matches_exclude = false;

	auto filter_matches = [&](std::string_view filter) {
		if ((sym.is_const() && SV_CONTAINS(filter, 'c')) || (sym.has_return() && SV_CONTAINS(filter, 'r')) ||
		    (sym.is_member() && SV_CONTAINS(filter, 'm')) || (sym.is_external() && SV_CONTAINS(filter, 'e')) ||
		    (sym.is_merged() && SV_CONTAINS(filter, 'M')) || (sym.is_generated() && SV_CONTAINS(filter, 'g'))) {
			return true;
		}

		if ((sym.type() == dt_void && SV_CONTAINS(filter, 'v')) ||
		    (sym.type() == dt_float && SV_CONTAINS(filter, 'f')) ||
		    (sym.type() == dt_integer && SV_CONTAINS(filter, 'i')) ||
		    (sym.type() == dt_string && SV_CONTAINS(filter, 's')) ||
		    (sym.type() == dt_class && SV_CONTAINS(filter, 'C')) ||
		    (sym.type() == dt_function && SV_CONTAINS(filter, 'F')) ||
		    (sym.type() == dt_prototype && SV_CONTAINS(filter, 'P')) ||
		    (sym.type() == dt_instance && SV_CONTAINS(filter, 'I'))) {
			return true;
		}
		return false;
	};

	if (!include_filter.empty()) {
		matches_include = filter_matches(include_filter);
	}

	if (!exclude_filter.empty()) {
		matches_exclude = filter_matches(exclude_filter);
	}

	return matches_include && !matches_exclude;
}

/// \brief Print a list of symbols and their contents
/// \param scr The script to print the symbols of
/// \param include_filter Filter for symbols to include
/// \param exclude_filter Filter for symbols to exclude
void print_symbol_list(const script& scr,
                       std::string_view include_filter,
                       std::string_view exclude_filter,
                       std::string_view search) {
	fmt::print("Index    Flags   Parent                    Address  R Name\n");

	for (const auto& sym : scr.symbols()) {
		if (!symbol_matches_filter(sym, include_filter, exclude_filter) ||
		    (!search.empty() && sym.name().find(search) == std::string_view::npos)) {
			continue;
		}

		// Index
		fmt::print("{:0>8x} ", sym.index());

		// Flags
		print_symbol_flags(sym);
		fmt::print(" ");

		// Parent
		print_symbol_parent(sym, scr);
		fmt::print(" ");

		// Address
		fmt::print("{:0>8x} ", sym.address());

		// Return type
		if (sym.is_const() && sym.type() == dt_function) {
			if (sym.has_return()) {
				fmt::print("{} ", get_type_abbrev(sym.rtype()));
			} else {
				fmt::print("v ");
			}
		} else {
			fmt::print("  ");
		}

		// Symbol name
		fmt::print("{}\n", sym.name());
	}
}

constexpr std::string_view get_opcode_name(opcode code) {
	switch (code) {
	case op_noop:
		return "nop";

		// Arithmetic operations

	case op_add:
		return "add";
	case op_subtract:
		return "sub";
	case op_multiply:
		return "mul";
	case op_divide:
		return "div";
	case op_modulo:
		return "mod";
	case op_bitor:
		return "bor";
	case op_bitand:
		return "band";
	case op_shift_left:
		return "shl";
	case op_shift_right:
		return "shr";
	case op_plus:
		return "pos";
	case op_minus:
		return "neg";

		// Logical operations

	case op_less:
		return "lt";
	case op_greater:
		return "gt";
	case op_or:
		return "or";
	case op_and:
		return "and";
	case op_less_or_equal:
		return "leq";
	case op_equal:
		return "eq";
	case op_not_equal:
		return "neq";
	case op_greater_or_equal:
		return "geq";
	case op_not:
		return "not";
	case op_complement:
		return "compl";

		// Flow control operations

	case op_return:
		return "ret";
	case op_call:
		return "call";
	case op_call_external:
		return "callext";
	case op_jump:
		return "br";
	case op_jump_if_zero:
		return "brz";

		// variable manipulation

	case op_assign_add:
		return "addass";
	case op_assign_subtract:
		return "subass";
	case op_assign_multiply:
		return "mulass";
	case op_assign_divide:
		return "divass";
	case op_assign_string:
		return "asss";
	case op_assign_stringref:
		return "asssr";
	case op_assign_func:
		return "assfn";
	case op_assign_float:
		return "assf";
	case op_assign_instance:
		return "assinst";
	case op_assign_int:
		return "assi";

		// stack manipulation

	case op_push_int:
		return "pushi";
	case op_push_var:
		return "pushv";
	case op_push_instance:
		return "pushinst";
	case op_push_array_var:
		return "pushva";

		// special

	case op_set_instance:
		return "setinst";
	}

	return "???";
}

/// \brief Prints the given instruction as raw bytes
/// \param i The instruction to print
void print_instruction_bytes(const instruction& i) {
	fmt::print("{:0>2x} ", i.op);

	switch (i.op) {
	case op_call:
	case op_jump_if_zero:
	case op_jump:
		fmt::print("{:0>2x} {:0>2x} {:0>2x} {:0>2x}    ",
		           i.address & 0xFFU,
		           (i.address >> 8U) & 0xFFU,
		           (i.address >> 16U) & 0xFFU,
		           (i.address >> 24U) & 0xFFU);
		break;
	case op_push_int:
		fmt::print("{:0>2x} {:0>2x} {:0>2x} {:0>2x}    ",
		           i.immediate & 0xFFU,
		           (i.immediate >> 8U) & 0xFFU,
		           (i.immediate >> 16U) & 0xFFU,
		           (i.immediate >> 24U) & 0xFFU);
		break;
	case op_call_external:
	case op_push_var:
	case op_push_instance:
	case op_set_instance:
		fmt::print("{:0>2x} {:0>2x} {:0>2x} {:0>2x}    ",
		           i.symbol & 0xFFU,
		           (i.symbol >> 8U) & 0xFFU,
		           (i.symbol >> 16U) & 0xFFU,
		           (i.symbol >> 24U) & 0xFFU);
		break;
	case op_push_array_var:
		fmt::print("{:0>2x} {:0>2x} {:0>2x} {:0>2x} {:0>2x} ",
		           i.symbol & 0xFFU,
		           (i.symbol >> 8U) & 0xFFU,
		           (i.symbol >> 16U) & 0xFFU,
		           (i.symbol >> 24U) & 0xFFU,
		           i.index);
		break;
	default:
		fmt::print("               ");
		break;
	}
}

/// \brief Prints the given instruction
/// \param scr The script to reference
/// \param i The instruction to print
void print_instruction(const script& scr, const instruction& i, bool instruction_only = false) {
	if (!instruction_only) {
		print_instruction_bytes(i);
		fmt::print(" ");
	}

	fmt::print("{}", get_opcode_name(i.op));

	switch (i.op) {
	case opcode::op_call:
		fmt::print(" {:0>8x}", i.address);
		if (const auto* s = scr.find_symbol_by_address(i.address); s != nullptr && !instruction_only) {
			fmt::print(" ; <{}>", s->name());
		}
		break;
	case opcode::op_jump:
	case opcode::op_jump_if_zero:
		fmt::print(" {:0>8x}", i.address);
		break;
	case opcode::op_push_var:
	case opcode::op_push_instance:
	case opcode::op_call_external:
	case opcode::op_set_instance:
		fmt::print(" {:0>8x}", i.symbol);

		if (const auto* s = scr.find_symbol_by_index(i.symbol); s != nullptr && !instruction_only) {
			fmt::print(" ; <{}>", s->name());

			if (has_constant_value(*s)) {
				fmt::print(" = {}", print_symbol_value(*s));
			}
		}
		break;
	case opcode::op_push_int:
		fmt::print(" {}", i.immediate);
		break;
	case opcode::op_push_array_var:
		fmt::print(" {:0>8x} + {}", i.address, i.index);

		if (const auto* s = scr.find_symbol_by_index(i.symbol); s != nullptr && !instruction_only) {
			fmt::print(" ; <{}+{}>", s->name(), i.index);

			if (has_constant_value(*s)) {
				fmt::print(" = {}", print_symbol_value(*s, i.index));
			}
		}
		break;
	default:
		break;
	}
}

/// \brief Prints the disassembly of one symbol
/// \param scr The script to reference
/// \param sym The symbol to print the disassembly of
void print_assembly_of_symbol(const script& scr, const symbol& sym) {
	fmt::print("{:0>8x} <{}>:\n", sym.address(), sym.name());

	uint32_t pc = sym.address();
	uint32_t return_after = pc;
	instruction i {};

	do {
		i = scr.instruction_at(pc);

		fmt::print("{: >8x}:\t", pc);
		print_instruction(scr, i);
		fmt::print("\n");

		if (i.op == opcode::op_jump_if_zero || i.op == opcode::op_jump) {
			return_after = return_after > i.address ? return_after : i.address;
		}

		pc += i.size;
	} while (i.op != opcode::op_return || pc <= return_after);
}

/// \brief Prints the disassembly of every symbol found in the script
/// \param scr The script to disassemble
void print_assembly(const script& scr) {
	for (const auto& sym : scr.symbols()) {
		if (sym.type() == dt_prototype || sym.type() == dt_instance ||
		    (sym.type() == dt_function && !sym.is_external() && sym.is_const())) {
			print_assembly_of_symbol(scr, sym);
		}
	}
}

void find_usages(const script& scr, const symbol& sym) {
	uint32_t pc = 0;
	const symbol* current_function;

	while (pc < scr.size()) {
		auto inst = scr.instruction_at(pc);
		auto func_sym = scr.find_symbol_by_address(pc);

		if (func_sym != nullptr) {
			fmt::print("\r{: <100}\r", func_sym->name());
			current_function = func_sym;
		}

		switch (inst.op) {
		case op_assign_add:
		case op_assign_subtract:
		case op_assign_multiply:
		case op_assign_divide:
		case op_call:
		case op_call_external:
		case op_push_var:
		case op_push_instance:
		case op_assign_int:
		case op_assign_string:
		case op_assign_func:
		case op_assign_float:
		case op_assign_instance:
		case op_push_array_var:
			if (inst.symbol == sym.index() || (inst.op == op_call && sym.address() == inst.address)) {
				fmt::print("\r[");
				print_instruction(scr, inst, true);
				fmt::print("] at {:0>8x} in {}\n", pc, current_function->name());
			}
			break;
		default:
			break;
		}

		pc += inst.size;
	}

	fmt::print("\r{: <100}\r", "");
}
