// Copyright © 2023-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include <doctest/doctest.h>

#include "zenkit/daedalus/Compiler.hh"

#include <string_view>

static std::string_view SOURCE = R"(
const INT MY_CONSTANT_2 = 88 - 48;
CONST int MY_CONSTANT_1 = MY_CONSTANT_2 * 2;

Class MyClass {
	Var Int field1;
	var Float field2;
	var String field3;
	var Func field4,field5;

	var int field6_array[5];
	var int field7_array[MY_CONSTANT_1];
};

Prototype MyProto(MyClass) {};

Instance MyInstance1(MyClass) {};
instance MyInstance2(MyProto) {};

Func Void MyFunc1() {};
func void MyFunc2(var int par1) {};
func void MyFunc3(var string par1, var MyClass par2) {};

)";

TEST_CASE("DaedalusCompiler") {
	zenkit::Logger::set_default(zenkit::LogLevel::DEBUG);
	zenkit::daedalus::Compiler compiler;
	compiler.add_raw(SOURCE);
	compiler.compile();
}
