// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/script.hh>

#include <argh.h>
#include <fmt/format.h>

#include <string>
#include <string_view>

#include "config.hh"

using namespace phoenix;

static constexpr const auto HELP_MESSAGE = "Usage: pxscrdmp [--version]\n"
										   "       pxscrdmp [-h|--help]\n"
										   "       pxscrdmp <FILE> [-t|--symbolize] [-I|--include <IFLAGS>] [-E|--exclude <EFLAGS>]\n"
										   "                       [-f|--search <TERM>] [-s|--symbol <NAME>]\n"
										   "       pxscrdmp <FILE> [-d|--disassemble] [-s|--symbol <NAME>]\n"
										   "\n"
										   "phoenix pxscrdmp v{}\n"
										   "View contents of compiled Daedalus script files.\n"
										   "\n"
										   "\n"
										   "Include and exclude flags:\n"
										   "\tc - const symbols\n"
										   "\tr - symbols that have a return value\n"
										   "\tm - member symbol_name\n"
										   "\te - extern symbols\n"
										   "\tM - merged symbols\n"
										   "\tg - generated symbols\n"
										   "\tv - symbols of type void\n"
										   "\tf - symbols of type float\n"
										   "\ti - symbols of type int\n"
										   "\ts - symbols of type string\n"
										   "\tC - classes\n"
										   "\tF - functions\n"
										   "\tP - prototypes\n"
										   "\tI - instances\n";


#define PRINT_FLAG(cond, flag)  \
	if (cond) {                 \
		fmt::print("{}", flag); \
	} else {                    \
		fmt::print(" ");        \
	}

#define SV_CONTAINS(sv, chr) ((sv).find(chr) != std::string_view::npos)

void print_assembly_of_symbol(const script& scr, const symbol& sym);
void print_assembly(const script& scr);

void print_symbol_detailed(const script& scr, const symbol& sym);
void print_symbol_list(const script& scr, std::string_view include_filter, std::string_view exclude_filter, std::string_view search);

int main(int argc, char** argv) {
	argh::parser cmdl {argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION};
	std::string input {};
	std::string symbol_name {};
	std::string include {};
	std::string exclude {};
	std::string search {};
	bool action_symbolize = false;
	bool action_disassemble = false;
	bool action_help = false;

	bool specific = false;
	bool help_flags = false;

	if (cmdl["--version"]) {
		fmt::print("pxscrdmp v{}\n", PXSCRDMP_VERSION);
		return EXIT_SUCCESS;
	}

	if (!cmdl(1)) {
		fmt::print(stderr, "no input file specified.\n");
		return EXIT_FAILURE;
	}

	input = cmdl(1).str();

	action_symbolize = cmdl[{"-t", "--symbolize"}];
	action_disassemble = cmdl[{"-d", "--disassemble"}];
	action_help = cmdl[{"-h", "--help"}];

	cmdl({"-f", "--search"}) >> search;
	cmdl({"-I", "--include"}) >> include;
	cmdl({"-E", "--exclude"}) >> exclude;

	if (cmdl({"-s", "--symbol"}) >> symbol_name) {
		specific = true;
	}

	if (action_help) {
		fmt::print(HELP_MESSAGE, PXSCRDMP_VERSION);
		return EXIT_SUCCESS;
	} else {
		auto scr = script::parse(input);
		const symbol* sym = nullptr;

		if (specific) {
			sym = scr.find_symbol_by_name(symbol_name);

			if (sym == nullptr) {
				fmt::print("symbol with name {} not found\n", symbol_name);
				return EXIT_FAILURE;
			}
		}

		if (action_symbolize) {
			if (specific) {
				print_symbol_detailed(scr, *sym);
			} else {
				print_symbol_list(scr, include, exclude, search);
			}
		} else if (action_disassemble) {
			if (specific) {
				print_assembly_of_symbol(scr, *sym);
			} else {
				print_assembly(scr);
			}
		}
	}

	return 0;
}

/// \brief Tests whether the symbol has a value related to it
/// \param symbol The symbol to test on
/// \return `true` if the symbol has a value `false` otherwise.
bool has_constant_value(const symbol& symbol) {
	return symbol.is_const() && symbol.type() != dt_prototype && symbol.type() != dt_instance && symbol.type() != dt_function;
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
void print_symbol_value(const symbol& symbol, int index = -1) {
	auto print_value = [&](uint8_t index) {
		switch (symbol.type()) {
			case dt_float:
				fmt::print("{}", symbol.get_float(index));
				break;
			case dt_integer:
				fmt::print("{}", symbol.get_int(index));
				break;
			case dt_string:
				fmt::print("\"{}\"", symbol.get_string(index));
				break;
			default:
				break;
		}
	};

	if (index >= 0) {
		print_value(index);
	} else {
		if (symbol.count() > 1) { fmt::print("{"); }

		for (unsigned i = 0; i < symbol.count(); ++i) {
			if (i > 0) { fmt::print(", "); }
			print_value(i);
		}

		if (symbol.count() > 1) { fmt::print("}"); }
	}
}

/// \brief Generates a daedalus definition from the given symbol.
/// \param scr The script to reference
/// \param sym The symbol to generate a definition of
/// \param parent The parent of the symbol
/// \param indent A custom indentation to prepend
void print_definition(const script& scr, const symbol& sym, const symbol* parent, std::string_view indent = "") {
	if (sym.is_member()) { return; }
	fmt::print(indent);
	if (sym.is_extern()) { fmt::print("extern "); }

	if (sym.type() == dt_instance) {
		fmt::print("instance {}({}) {{ /* ... */}}", sym.name(), (parent == nullptr ? "*ERR*" : parent->name()));
	} else if (sym.type() == dt_prototype) {
		fmt::print("prototype {}({}) {{ /* ... */ }}", sym.name(), (parent == nullptr ? "*ERR*" : parent->name()));
	} else if (sym.type() == dt_class) {
		fmt::print("class {} {{\n", sym.name());

		for (const auto& member : scr.symbols()) {
			if (member.parent() == sym.index() && member.is_member()) {
				auto name = member.name().substr(member.name().find('.') + 1);
				fmt::print("{}\tvar {} {}", indent, get_type_name(member.type()), name);
				if (member.count() > 1) { fmt::print("[{}]", member.count()); }
				fmt::print(";\n");
			}
		}

		fmt::print("{}}};", indent);
	} else if (sym.type() == dt_function) {
		fmt::print("func {} {}(", get_type_name(sym.rtype()), sym.name());

		unsigned count = 0;
		for (const auto& param : scr.symbols()) {
			if (!param.is_member() && param.name().find(sym.name() + ".") == 0 && param.index() != sym.index()) {
				if (count > 0) { fmt::print(", "); }
				auto name = param.name().substr(param.name().find('.') + 1);

				if (param.type() == dt_instance) {
					const auto* dt = scr.find_symbol_by_index(param.parent());

					if (dt == nullptr) {
						fmt::print("var instance ");
					} else {
						fmt::print("var {} ", dt->name());
					}
				} else {
					fmt::print("var {} ", get_type_name(param.type()));
				}

				fmt::print(name);
				count++;
			}
		}

		fmt::print(") {{ /* ... */ }};");
	} else {
		fmt::print("const {} {}", get_type_name(sym.type()), sym.name());
		if (sym.count() > 1) { fmt::print("[{}]", sym.count()); }

		fmt::print(" = ");
		print_symbol_value(sym);
		fmt::print(";");
	}
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
	fmt::print("\t\tExtern: {}\n", (sym.is_extern() ? "Yes" : "No"));
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
		fmt::print("\tDefinition:\n");
		print_definition(scr, sym, parent, "\t\t");
		fmt::print("\n");
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
}

/// \brief Print a string containing all the given symbols flags
/// \param sym The symbol to print the flags of
void print_symbol_flags(const symbol& sym) {
	fmt::print("{}", get_type_abbrev(sym.type()));

	PRINT_FLAG(sym.is_const(), 'c');
	PRINT_FLAG(sym.has_return(), 'r');
	PRINT_FLAG(sym.is_member(), 'm');
	PRINT_FLAG(sym.is_extern(), 'e');
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
		if ((sym.is_const() && SV_CONTAINS(filter, 'c')) ||
			(sym.has_return() && SV_CONTAINS(filter, 'r')) ||
			(sym.is_member() && SV_CONTAINS(filter, 'm')) ||
			(sym.is_extern() && SV_CONTAINS(filter, 'e')) ||
			(sym.is_merged() && SV_CONTAINS(filter, 'M')) ||
			(sym.is_generated() && SV_CONTAINS(filter, 'g'))) {
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
void print_symbol_list(const script& scr, std::string_view include_filter, std::string_view exclude_filter, std::string_view search) {
	fmt::print("Index    Flags   Parent                    Address  R Name\n");

	for (const auto& sym : scr.symbols()) {
		if (!symbol_matches_filter(sym, include_filter, exclude_filter) || (!search.empty() && sym.name().find(search) == std::string_view::npos)) {
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
				fmt::print("{}", get_type_abbrev(sym.rtype()));
			} else {
				fmt::print("v");
			}
		} else {
			fmt::print(" ");
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
}

/// \brief Prints the given instruction as raw bytes
/// \param i The instruction to print
void print_instruction_bytes(const instruction& i) {
	fmt::print("{:0>2x} ", i.op);

	if (i.size > 1) {
		fmt::print("{:0>2x} {:0>2x} {:0>2x} {:0>2x} ", (i.address >> 24U) & 0xFFU, (i.address >> 16U) & 0xFFU, (i.address >> 8U) & 0xFFU, i.address & 0xFFU);
	} else {
		fmt::print("            ");
	}

	if (i.size > 5) {
		fmt::print("{:0>2x} ", i.index);
	} else {
		fmt::print("   ");
	}
}

/// \brief Prints the given instruction
/// \param scr The script to reference
/// \param i The instruction to print
void print_instruction(const script& scr, const instruction& i) {
	print_instruction_bytes(i);

	fmt::print(" {}", get_opcode_name(i.op));

	switch (i.op) {
		case opcode::op_call:
			fmt::print(" {:0>8x}", i.address);
			if (const auto* s = scr.find_symbol_by_address(i.address); s != nullptr) {
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

			if (const auto* s = scr.find_symbol_by_index(i.symbol); s != nullptr) {
				fmt::print(" ; <{}>", s->name());

				if (has_constant_value(*s)) {
					fmt::print(" = ");
					print_symbol_value(*s);
				}
			}
			break;
		case opcode::op_push_int:
			fmt::print(" {}", i.immediate);
			break;
		case opcode::op_push_array_var:
			fmt::print(" {:0>8x} + {}", i.address, i.index);

			if (const auto* s = scr.find_symbol_by_index(i.symbol); s != nullptr) {
				fmt::print(" ; <{}+{}>", s->name(), i.index);

				if (has_constant_value(*s)) {
					fmt::print(" = ");
					print_symbol_value(*s, i.index);
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
		if (sym.type() == dt_prototype || sym.type() == dt_instance || (sym.type() == dt_function && !sym.is_extern() && sym.is_const())) {
			print_assembly_of_symbol(scr, sym);
		}
	}
}
