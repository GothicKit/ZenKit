// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <phoenix/script.hh>

namespace px = phoenix;

static bool compare_instruction(phoenix::daedalus::instruction a, phoenix::daedalus::instruction b) {
	return a.op == b.op && a.index == b.index && a.immediate == b.immediate && a.address == b.address &&
	    a.symbol == b.symbol;
}

TEST_SUITE("script") {
	TEST_CASE("compiled daedalus script symbols are read correctly") {
		auto scr = phoenix::daedalus::script::parse("./samples/menu.proprietary.dat");

		auto& syms = scr.symbols();
		CHECK(syms.size() == 1094);

		auto* class_symbol = scr.find_symbol_by_index(118);
		auto* member_symbol = scr.find_symbol_by_name("C_MENU.BACKPIC");
		auto* prototype_symbol = scr.find_symbol_by_index(133);
		auto* instance_symbol = scr.find_symbol_by_name("MENU_MAIN");
		auto* function_symbol = scr.find_symbol_by_address(1877);
		auto* external_symbol = scr.find_symbol_by_index(1);

		auto* nonexistent_symbol1 = scr.find_symbol_by_index(syms.size() + 100);
		auto* nonexistent_symbol2 = scr.find_symbol_by_name("nonexistent_lol");
		auto* nonexistent_symbol3 = scr.find_symbol_by_address(0xffffffaa);

		CHECK(class_symbol != nullptr);
		CHECK(member_symbol != nullptr);
		CHECK(prototype_symbol != nullptr);
		CHECK(instance_symbol != nullptr);
		CHECK(function_symbol != nullptr);
		CHECK(external_symbol != nullptr);
		CHECK(nonexistent_symbol1 == nullptr);
		CHECK(nonexistent_symbol2 == nullptr);
		CHECK(nonexistent_symbol3 == nullptr);

		CHECK(class_symbol->name() == "C_MENU");
		CHECK(class_symbol->count() == 13);
		CHECK(class_symbol->type() == px::daedalus::datatype::class_);
		CHECK(!class_symbol->has_return());
		CHECK(class_symbol->class_size() == 3096);
		// CHECK(class_symbol->parent() == -1); FIXME

		CHECK(member_symbol->name() == "C_MENU.BACKPIC");
		CHECK(member_symbol->count() == 1);
		CHECK(member_symbol->type() == px::daedalus::datatype::string);
		CHECK(!member_symbol->has_return());
		CHECK(member_symbol->parent() == 118);

		CHECK(prototype_symbol->name() == "C_MENU_DEF");
		CHECK(prototype_symbol->count() == 0);
		CHECK(prototype_symbol->address() == 236);
		CHECK(prototype_symbol->type() == px::daedalus::datatype::prototype);
		CHECK(!prototype_symbol->has_return());
		CHECK(prototype_symbol->parent() == 118);

		CHECK(instance_symbol->name() == "MENU_MAIN");
		CHECK(instance_symbol->count() == 0);
		CHECK(instance_symbol->address() == 372);
		CHECK(instance_symbol->type() == px::daedalus::datatype::instance);
		CHECK(!instance_symbol->has_return());
		CHECK(instance_symbol->parent() == 133);

		CHECK(function_symbol->name() == "SHOWINTRO");
		CHECK(function_symbol->count() == 0);
		CHECK(function_symbol->address() == 1877);
		CHECK(function_symbol->type() == px::daedalus::datatype::function);
		CHECK(function_symbol->has_return());
		CHECK(function_symbol->rtype() == px::daedalus::datatype::integer);
		// CHECK(function_symbol->parent() == -1); FIXME

		CHECK(external_symbol->name() == "UPDATE_CHOICEBOX");
		CHECK(external_symbol->count() == 1);
		CHECK(external_symbol->type() == px::daedalus::datatype::function);
		CHECK(external_symbol->is_external());
		CHECK(external_symbol->is_const());
		CHECK(!external_symbol->has_return());
	}

	TEST_CASE("compiled daedalus script instructions are read correctly") {
		auto scr = phoenix::daedalus::script::parse("./samples/menu.proprietary.dat");
		auto* instance_symbol = scr.find_symbol_by_name("MENU_MAIN");
		CHECK(instance_symbol != nullptr);

		auto pc_begin = instance_symbol->address();
		CHECK(pc_begin == 372);

		// TODO: This only covers a very small amount of instructions. Improve!
		phoenix::daedalus::instruction ops[10] = {
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_call, .address = 236},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_push_var, .symbol = 10},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_push_var, .symbol = 119},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_assign_string},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_push_var, .symbol = 426},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_push_var, .symbol = 126},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_assign_string},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_push_var, .symbol = 427},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_push_var, .symbol = 128},
		    phoenix::daedalus::instruction {.op = px::daedalus::opcode::op_assign_string, .address = 0},
		};

		phoenix::daedalus::instruction op;

		for (int i = 0; i < 10; ++i) {
			op = scr.instruction_at(pc_begin);
			CHECK(compare_instruction(op, ops[i]));
			pc_begin += op.size;
		}
	}
}
