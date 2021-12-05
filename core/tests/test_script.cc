// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include "catch2.hh"
#include <phoenix/daedalus/script.hh>

TEST_CASE("compiled daedalus script symbols are read correctly", "[script][proprietary]") {
	auto scr = phoenix::script::parse("./samples/menu.proprietary.dat");

	auto& syms = scr.symbols();
	REQUIRE(syms.size() == 1094);

	auto* class_symbol = scr.find_symbol_by_index(118);
	auto* member_symbol = scr.find_symbol_by_name("C_MENU.BACKPIC");
	auto* prototype_symbol = scr.find_symbol_by_index(133);
	auto* instance_symbol = scr.find_symbol_by_name("MENU_MAIN");
	auto* function_symbol = scr.find_symbol_by_address(1877);
	auto* external_symbol = scr.find_symbol_by_index(1);

	auto* nonexistent_symbol1 = scr.find_symbol_by_index(syms.size() + 100);
	auto* nonexistent_symbol2 = scr.find_symbol_by_name("nonexistent_lol");
	auto* nonexistent_symbol3 = scr.find_symbol_by_address(0xffffffaa);

	REQUIRE(class_symbol != nullptr);
	REQUIRE(member_symbol != nullptr);
	REQUIRE(prototype_symbol != nullptr);
	REQUIRE(instance_symbol != nullptr);
	REQUIRE(function_symbol != nullptr);
	REQUIRE(external_symbol != nullptr);
	REQUIRE(nonexistent_symbol1 == nullptr);
	REQUIRE(nonexistent_symbol2 == nullptr);
	REQUIRE(nonexistent_symbol3 == nullptr);

	REQUIRE(class_symbol->name() == "C_MENU");
	REQUIRE(class_symbol->count() == 13);
	REQUIRE(class_symbol->type() == phoenix::dt_class);
	REQUIRE(!class_symbol->has_return());
	REQUIRE(class_symbol->class_size() == 3096);
	// REQUIRE(class_symbol->parent() == -1); FIXME

	REQUIRE(member_symbol->name() == "C_MENU.BACKPIC");
	REQUIRE(member_symbol->count() == 1);
	REQUIRE(member_symbol->type() == phoenix::dt_string);
	REQUIRE(!member_symbol->has_return());
	REQUIRE(member_symbol->parent() == 118);

	REQUIRE(prototype_symbol->name() == "C_MENU_DEF");
	REQUIRE(prototype_symbol->count() == 0);
	REQUIRE(prototype_symbol->address() == 236);
	REQUIRE(prototype_symbol->type() == phoenix::dt_prototype);
	REQUIRE(!prototype_symbol->has_return());
	REQUIRE(prototype_symbol->parent() == 118);

	REQUIRE(instance_symbol->name() == "MENU_MAIN");
	REQUIRE(instance_symbol->count() == 0);
	REQUIRE(instance_symbol->address() == 372);
	REQUIRE(instance_symbol->type() == phoenix::dt_instance);
	REQUIRE(!instance_symbol->has_return());
	REQUIRE(instance_symbol->parent() == 133);

	REQUIRE(function_symbol->name() == "SHOWINTRO");
	REQUIRE(function_symbol->count() == 0);
	REQUIRE(function_symbol->address() == 1877);
	REQUIRE(function_symbol->type() == phoenix::dt_function);
	REQUIRE(function_symbol->has_return());
	REQUIRE(function_symbol->rtype() == phoenix::dt_integer);
	// REQUIRE(function_symbol->parent() == -1); FIXME

	REQUIRE(external_symbol->name() == "UPDATE_CHOICEBOX");
	REQUIRE(external_symbol->count() == 1);
	REQUIRE(external_symbol->type() == phoenix::dt_function);
	REQUIRE(external_symbol->is_extern());
	REQUIRE(external_symbol->is_const());
	REQUIRE(!external_symbol->has_return());
}

static bool compare_instruction(phoenix::instruction a, phoenix::instruction b) {
	return a.op == b.op &&
		   a.index == b.index &&
		   a.immediate == b.immediate &&
		   a.address == b.address &&
		   a.symbol == b.symbol;
}

TEST_CASE("compiled daedalus script instructions are read correctly", "[script][proprietary]") {
	auto scr = phoenix::script::parse("./samples/menu.proprietary.dat");
	auto* instance_symbol = scr.find_symbol_by_name("MENU_MAIN");
	REQUIRE(instance_symbol != nullptr);

	auto pc_begin = instance_symbol->address();
	REQUIRE(pc_begin == 372);

	// TODO: This only covers a very small amount of instructions. Improve!
	phoenix::instruction ops[10] = {
			phoenix::instruction {.op = phoenix::op_call, .address = 236},
			phoenix::instruction {.op = phoenix::op_push_var, .symbol = 10},
			phoenix::instruction {.op = phoenix::op_push_var, .symbol = 119},
			phoenix::instruction {.op = phoenix::op_assign_string},
			phoenix::instruction {.op = phoenix::op_push_var, .symbol = 426},
			phoenix::instruction {.op = phoenix::op_push_var, .symbol = 126},
			phoenix::instruction {.op = phoenix::op_assign_string},
			phoenix::instruction {.op = phoenix::op_push_var, .symbol = 427},
			phoenix::instruction {.op = phoenix::op_push_var, .symbol = 128},
			phoenix::instruction {.op = phoenix::op_assign_string, .address = 0},
	};

	phoenix::instruction op;

	for (int i = 0; i < 10; ++i) {
		op = scr.instruction_at(pc_begin);
		REQUIRE(compare_instruction(op, ops[i]));
		pc_begin += op.size;
	}
}
