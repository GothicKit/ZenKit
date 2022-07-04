// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "decompiler.hh"

#include <fmt/format.h>
#include <vector>

using namespace phoenix::daedalus;

static uint32_t current_instance = -1;
static const symbol* current_symbol = nullptr;

struct stack_frame {
	instruction instr;
	uint32_t instance;
};

bool is_terminating_instruction(const script& script, const instruction& i) {
	switch (i.op) {
	case op_assign_int:
	case op_assign_add:
	case op_assign_subtract:
	case op_assign_multiply:
	case op_assign_divide:
	case op_return:
	case op_assign_string:
	case op_assign_stringref:
	case op_assign_func:
	case op_assign_float:
	case op_assign_instance:
	case op_jump:
	case op_jump_if_zero:
		return true;
	case op_set_instance:
		current_instance = i.symbol;
		return false;
	case op_call:
		return !script.find_symbol_by_address(i.address)->has_return();
	case op_call_external:
		return !script.find_symbol_by_index(i.symbol)->has_return();
	default:
		return false;
	}
}

stack_frame extract_statement(const script& script, uint32_t& pointer, std::vector<stack_frame>& stack) {
	instruction instr = script.instruction_at(pointer);
	pointer += instr.size;

	while (!is_terminating_instruction(script, instr)) {
		if (instr.op != op_noop && instr.op != op_set_instance) {
			stack.push_back({instr, current_instance});
		}

		instr = script.instruction_at(pointer);
		pointer += instr.size;
	}

	return {instr, current_instance};
}

static std::unordered_map<opcode, std::string> OPCODE_STR {
    {op_add, "+"},
    {op_subtract, "-"},
    {op_multiply, "*"},
    {op_divide, "/"},
    {op_modulo, "%"},
    {op_bitor, "|"},
    {op_bitand, "&"},
    {op_less, "<"},
    {op_greater, ">"},
    {op_or, "||"},
    {op_and, "&&"},
    {op_shift_left, "<<"},
    {op_shift_right, ">>"},
    {op_less_or_equal, "<="},
    {op_equal, "=="},
    {op_not_equal, "!="},
    {op_greater_or_equal, ">="},
    {op_plus, "+"},
    {op_minus, "-"},
    {op_not, "!"},
    {op_complement, "~"},
    {op_assign_add, "+="},
    {op_assign_subtract, "-="},
    {op_assign_multiply, "*="},
    {op_assign_divide, "/="},
};

std::string decompile_statement(const script& script, const stack_frame& stmt, std::vector<stack_frame>& stack) {
	switch (stmt.instr.op) {
	case op_add:
	case op_subtract:
	case op_multiply:
	case op_divide:
	case op_modulo:
	case op_bitor:
	case op_bitand:
	case op_less:
	case op_greater:
	case op_or:
	case op_and:
	case op_shift_left:
	case op_shift_right:
	case op_less_or_equal:
	case op_equal:
	case op_not_equal:
	case op_greater_or_equal: {
		stack_frame a_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
		if (!stack.empty()) {
			a_instr = stack.back();
			stack.pop_back();
		}

		auto a = decompile_statement(script, a_instr, stack);

		stack_frame b_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
		if (!stack.empty()) {
			b_instr = stack.back();
			stack.pop_back();
		}

		auto b = decompile_statement(script, b_instr, stack);
		return fmt::format("({}) {} ({})", a, OPCODE_STR[stmt.instr.op], b);
	}
	case op_assign_add:
	case op_assign_subtract:
	case op_assign_multiply:
	case op_assign_divide: {
		stack_frame a_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
		if (!stack.empty()) {
			a_instr = stack.back();
			stack.pop_back();
		}
		auto a = decompile_statement(script, a_instr, stack);

		auto sym = script.find_symbol_by_index(stmt.instr.symbol);
		std::string sym_name;

		if (sym->is_member() &&
		    !(current_symbol->type() == phoenix::daedalus::dt_instance &&
		      current_instance == current_symbol->index())) {
			auto inst_sym = script.find_symbol_by_index(stmt.instance);
			if (inst_sym == nullptr) {
				sym_name = "???" + sym->name().substr(sym->name().find('.'));
			} else {
				sym_name = inst_sym->name() + sym->name().substr(sym->name().find('.'));
			}
		} else {
			auto dot = sym->name().find('.');
			sym_name = dot == std::string::npos ? sym->name() : sym_name.substr(dot + 1);
		}

		return fmt::format("{} {} {}", sym_name, OPCODE_STR[stmt.instr.op], a);
	}
	case op_plus:
	case op_minus:
	case op_not:
	case op_complement: {
		stack_frame a_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
		if (!stack.empty()) {
			a_instr = stack.back();
			stack.pop_back();
		}
		auto a = decompile_statement(script, a_instr, stack);
		return fmt::format("{}({})", OPCODE_STR[stmt.instr.op], a);
	}
	case op_noop:
		return "";
	case op_return: {
		if (!current_symbol->has_return()) {
			return "return";
		}

		if (stack.empty()) {
			return "return 0 /* !broken stack! */";
		}

		auto a_instr = stack.back();
		stack.pop_back();
		auto a = decompile_statement(script, a_instr, stack);
		return fmt::format("return {}", a);
	}
	case op_call: {
		auto* sym = script.find_symbol_by_address(stmt.instr.address);
		auto params = script.find_parameters_for_function(sym);
		std::string call = "";

		for (unsigned i = params.size(); i > 0; --i) {
			stack_frame a_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
			if (!stack.empty()) {
				a_instr = stack.back();
				stack.pop_back();
			}

			if (i == 0 || call.size() == 0) {
				call = fmt::format("{}{}", decompile_statement(script, a_instr, stack), call);
			} else {
				call = fmt::format("{}, {}", decompile_statement(script, a_instr, stack), call);
			}
		}

		return fmt::format("{}({})", sym->name(), call);
	}
	case op_call_external: {
		auto* sym = script.find_symbol_by_index(stmt.instr.symbol);
		auto params = script.find_parameters_for_function(sym);
		std::string call = "";

		for (unsigned i = params.size(); i > 0; --i) {
			stack_frame a_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
			if (!stack.empty()) {
				a_instr = stack.back();
				stack.pop_back();
			}

			if (i == 0 || call.size() == 0) {
				call = fmt::format("{}{}", decompile_statement(script, a_instr, stack), call);
			} else {
				call = fmt::format("{}, {}", decompile_statement(script, a_instr, stack), call);
			}
		}

		return fmt::format("{}({})", sym->name(), call);
	}
	case op_push_int: {
		if (stmt.instr.immediate > 1000) {
			return fmt::format("0x{:x}", stmt.instr.immediate);
		}
		return std::to_string(stmt.instr.immediate);
	}
	case op_push_var:
	case op_push_instance: {
		auto sym = script.find_symbol_by_index(stmt.instr.symbol);

		if (sym->is_generated() && sym->type() == dt_string) {
			return fmt::format("\"{}\"", sym->get_string());
		}

		std::string sym_name;
		if (sym->is_member() &&
		    !(current_symbol->type() == phoenix::daedalus::dt_instance &&
		      current_instance == current_symbol->index())) {
			auto inst_sym = script.find_symbol_by_index(stmt.instance);
			if (inst_sym == nullptr) {
				sym_name = "???" + sym->name().substr(sym->name().find('.'));
			} else {
				sym_name = inst_sym->name() + sym->name().substr(sym->name().find('.'));
			}
		} else {
			auto dot = sym->name().find('.');
			sym_name = dot == std::string::npos ? sym->name() : sym->name().substr(dot + 1);
		}

		if (sym->count() > 1) {
			return fmt::format("{}[0]", sym_name);
		}

		return sym_name;
	}
	case op_push_array_var: {
		auto sym = script.find_symbol_by_index(stmt.instr.symbol);
		std::string sym_name;

		if (sym->is_member() &&
		    !(current_symbol->type() == phoenix::daedalus::dt_instance &&
		      current_instance == current_symbol->index())) {
			auto inst_sym = script.find_symbol_by_index(stmt.instance);
			if (inst_sym == nullptr) {
				sym_name = "???" + sym->name().substr(sym->name().find('.'));
			} else {
				sym_name = inst_sym->name() + sym->name().substr(sym->name().find('.'));
			}
		} else {
			auto dot = sym->name().find('.');
			sym_name = dot == std::string::npos ? sym->name() : sym_name.substr(dot + 1);
		}

		return fmt::format("{}[{}]", sym_name, stmt.instr.index);
	}
	case op_assign_int:
	case op_assign_string:
	case op_assign_stringref:
	case op_assign_func:
	case op_assign_float:
	case op_assign_instance: {
		stack_frame a_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
		if (!stack.empty()) {
			a_instr = stack.back();
			stack.pop_back();
		}

		auto a = decompile_statement(script, a_instr, stack);
		auto sym = script.find_symbol_by_index(a_instr.instr.symbol);

		stack_frame b_instr {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
		if (!stack.empty()) {
			b_instr = stack.back();
			stack.pop_back();
		}
		auto b = decompile_statement(script, b_instr, stack);

		if (sym->type() == dt_float) {
			return fmt::format("{} = {}", a, std::bit_cast<float>(b_instr.instr.immediate));
		} else if (sym->type() == dt_function) {
			return fmt::format("{} = {}", a, script.find_symbol_by_index(b_instr.instr.immediate)->name());
		}

		return fmt::format("{} = {}", a, b);
	}
	default:
		throw;
	}
}

std::pair<std::string, std::uint32_t> decompile_block(const script& script,
                                                      int indent,
                                                      uint32_t pointer,
                                                      int64_t end_ptr = -1,
                                                      std::vector<stack_frame> stack = {}) {
	std::string code {};
	stack_frame stmt {};

	do {
		stmt = extract_statement(script, pointer, stack);

		if (stmt.instr.op == op_jump_if_zero) {
			stack_frame real_stmt {phoenix::daedalus::op_push_int, 0, 0, 0, 0, 0};
			if (!stack.empty()) {
				real_stmt = stack.back();
				stack.pop_back();
			}

			code += fmt::format("{: >{}}if ({}) {{\n", "", indent, decompile_statement(script, real_stmt, stack));
			auto [if_block, next_branch] = decompile_block(script, indent + 4, pointer, stmt.instr.address);
			pointer = stmt.instr.address;

			code += if_block + fmt::format("{: >{}}}}", "", indent);

			while (next_branch > stmt.instr.address && pointer <= end_ptr) {
				auto new_stmt = extract_statement(script, pointer, stack);

				if (new_stmt.instr.op == op_jump_if_zero) {
					// else-if block
					if (!stack.empty()) {
						real_stmt = stack.back();
						stack.pop_back();
					}

					code += fmt::format(" else if ({}) {{\n", decompile_statement(script, real_stmt, stack));
					auto [else_if_block, else_if_next_branch] =
					    decompile_block(script, indent + 4, pointer, new_stmt.instr.address);
					next_branch = else_if_next_branch;
					pointer = new_stmt.instr.address;

					code += else_if_block + fmt::format("{: >{}}}}", "", indent);
					stmt = new_stmt;
				} else {
					// else block
					code += fmt::format(" else {{\n");
					auto [else_block, else_next_branch] = decompile_block(script, indent + 4, pointer, -1);
					next_branch = else_next_branch;
					pointer = else_next_branch;

					code += else_block + fmt::format("{: >{}}}}", "", indent);
				}
			}

			code += ";\n";
		} else if (stmt.instr.op == op_jump) {
			return {code, stmt.instr.address};
		} else {
			auto s = decompile_statement(script, stmt, stack);
			if (!s.empty()) {
				code += fmt::format("{: >{}}{};\n", "", indent, s);
			}
		}
	} while (stmt.instr.op != op_return && pointer != end_ptr);

	return {code, pointer};
}

std::string decompile(const phoenix::daedalus::script& script, const phoenix::daedalus::symbol& sym, int indent) {
	if (sym.is_external()) {
		throw;
	}

	current_symbol = &sym;
	current_instance = sym.type() == dt_instance ? sym.index() : unset;
	std::vector<stack_frame> stack {};
	auto params = script.find_parameters_for_function(&sym);

	auto result = decompile_block(script, indent, sym.address() + 6 * params.size(), -1, stack).first;

	// If the function does not return anything, remove the last return
	if (!sym.has_return()) {
		result.erase(result.rfind("return")).erase(result.rfind('\n') + 1);
	}

	return result;
}
