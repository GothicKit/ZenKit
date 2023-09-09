// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/CutsceneLibrary.hh"

namespace phoenix {
	using atomic_message ZKREM("renamed to zenkit::CutsceneMessage") = zenkit::CutsceneMessage;
	using message_block ZKREM("renamed to zenkit::CutsceneBlock") = zenkit::CutsceneBlock;
	using messages ZKREM("renamed to zenkit::CutsceneLibrary") = zenkit::CutsceneLibrary;
} // namespace phoenix
