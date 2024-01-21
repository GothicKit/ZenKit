// Copyright © 2021-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>
#include <zenkit/DaedalusScript.hh>
#include <zenkit/Stream.hh>

[[maybe_unused]] static bool compare_instruction(zenkit::DaedalusInstruction const& a,
                                                 zenkit::DaedalusInstruction const& b) {
	return a.op == b.op && a.index == b.index && a.immediate == b.immediate && a.address == b.address &&
	    a.symbol == b.symbol;
}

TEST_SUITE("DaedalusScript") {
	TEST_CASE("DaedalusScript.load") {
		auto r = zenkit::Read::from("./samples/menu.proprietary.dat");
		zenkit::DaedalusScript scr {};
		scr.load(r.get());

		auto& syms = scr.symbols();
		CHECK_EQ(syms.size(), 1093);

		auto* class_symbol = scr.find_symbol_by_index(118);
		auto* member_symbol = scr.find_symbol_by_name("C_MENU.BACKPIC");
		auto* prototype_symbol = scr.find_symbol_by_index(133);
		auto* instance_symbol = scr.find_symbol_by_name("MENU_MAIN");
		auto* function_symbol = scr.find_symbol_by_address(1877);
		auto* external_symbol = scr.find_symbol_by_index(1);

		auto* nonexistent_symbol1 = scr.find_symbol_by_index(static_cast<std::uint32_t>(syms.size() + 100));
		auto* nonexistent_symbol2 = scr.find_symbol_by_name("nonexistent_lol");
		auto* nonexistent_symbol3 = scr.find_symbol_by_address(0xffffffaa);

		CHECK_NE(class_symbol, nullptr);
		CHECK_NE(member_symbol, nullptr);
		CHECK_NE(prototype_symbol, nullptr);
		CHECK_NE(instance_symbol, nullptr);
		CHECK_NE(function_symbol, nullptr);
		CHECK_NE(external_symbol, nullptr);
		CHECK_EQ(nonexistent_symbol1, nullptr);
		CHECK_EQ(nonexistent_symbol2, nullptr);
		CHECK_EQ(nonexistent_symbol3, nullptr);

		CHECK_EQ(class_symbol->name(), "C_MENU");
		CHECK_EQ(class_symbol->count(), 13);
		CHECK_EQ(class_symbol->type(), zenkit::DaedalusDataType::CLASS);
		CHECK_FALSE(class_symbol->has_return());
		CHECK_EQ(class_symbol->class_size(), 3096);
		// CHECK_EQ(class_symbol->parent(), -1); FIXME

		CHECK_EQ(member_symbol->name(), "C_MENU.BACKPIC");
		CHECK_EQ(member_symbol->count(), 1);
		CHECK_EQ(member_symbol->type(), zenkit::DaedalusDataType::STRING);
		CHECK_FALSE(member_symbol->has_return());
		CHECK_EQ(member_symbol->parent(), 118);

		CHECK_EQ(prototype_symbol->name(), "C_MENU_DEF");
		CHECK_EQ(prototype_symbol->count(), 0);
		CHECK_EQ(prototype_symbol->address(), 236);
		CHECK_EQ(prototype_symbol->type(), zenkit::DaedalusDataType::PROTOTYPE);
		CHECK_FALSE(prototype_symbol->has_return());
		CHECK_EQ(prototype_symbol->parent(), 118);

		CHECK_EQ(instance_symbol->name(), "MENU_MAIN");
		CHECK_EQ(instance_symbol->count(), 0);
		CHECK_EQ(instance_symbol->address(), 372);
		CHECK_EQ(instance_symbol->type(), zenkit::DaedalusDataType::INSTANCE);
		CHECK_FALSE(instance_symbol->has_return());
		CHECK_EQ(instance_symbol->parent(), 133);

		CHECK_EQ(function_symbol->name(), "SHOWINTRO");
		CHECK_EQ(function_symbol->count(), 0);
		CHECK_EQ(function_symbol->address(), 1877);
		CHECK_EQ(function_symbol->type(), zenkit::DaedalusDataType::FUNCTION);
		CHECK(function_symbol->has_return());
		CHECK_EQ(function_symbol->rtype(), zenkit::DaedalusDataType::INT);
		// CHECK(function_symbol->parent(), -1); FIXME

		CHECK_EQ(external_symbol->name(), "UPDATE_CHOICEBOX");
		CHECK_EQ(external_symbol->count(), 1);
		CHECK_EQ(external_symbol->type(), zenkit::DaedalusDataType::FUNCTION);
		CHECK(external_symbol->is_external());
		CHECK(external_symbol->is_const());
		CHECK_FALSE(external_symbol->has_return());

		CHECK_NE(instance_symbol, nullptr);

		auto pc_begin = instance_symbol->address();
		CHECK_EQ(pc_begin, 372);

		// TODO: This only covers a very small amount of instructions. Improve!
		// TODO: Rework this without using designated initializers

		/*
		phoenix::instruction ops[10] = {
		    phoenix::instruction {.op = px::opcode::bl, .address = 236},
		    phoenix::instruction {.op = px::opcode::pushv, .symbol = 10},
		    phoenix::instruction {.op = px::opcode::pushv, .symbol = 119},
		    phoenix::instruction {.op = px::opcode::movs},
		    phoenix::instruction {.op = px::opcode::pushv, .symbol = 426},
		    phoenix::instruction {.op = px::opcode::pushv, .symbol = 126},
		    phoenix::instruction {.op = px::opcode::movs},
		    phoenix::instruction {.op = px::opcode::pushv, .symbol = 427},
		    phoenix::instruction {.op = px::opcode::pushv, .symbol = 128},
		    phoenix::instruction {.op = px::opcode::movs, .address = 0},
		};

		phoenix::instruction op;

		for (int32_t i = 0; i < 10; ++i) {
		    op = scr.instruction_at(pc_begin);
		    CHECK(compare_instruction(op, ops[i]));
		    pc_begin += op.size;
		}
		*/
	}
}
