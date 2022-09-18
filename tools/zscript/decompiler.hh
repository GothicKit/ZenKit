// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/script.hh>

#include <string>

std::string decompile(const phoenix::daedalus::script& script, const phoenix::daedalus::symbol& sym, int indent = 0);
