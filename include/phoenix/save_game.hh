// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/SaveGame.hh"

namespace phoenix::unstable {
	using save_info ZKREM("renamed to zenkit::unstable::SaveInfo") = zenkit::unstable::SaveInfo;
	using topic_section ZKREM("renamed to zenkit::unstable::SaveTopicSection") = zenkit::unstable::SaveTopicSection;
	using topic_status ZKREM("renamed to zenkit::unstable::SaveTopicStatus") = zenkit::unstable::SaveTopicStatus;
	using log_topic ZKREM("renamed to zenkit::unstable::SaveLogTopic") = zenkit::unstable::SaveLogTopic;
	using info_state ZKREM("renamed to zenkit::unstable::SaveInfoState") = zenkit::unstable::SaveInfoState;
	using symbol_state ZKREM("renamed to zenkit::unstable::SaveSymbolState") = zenkit::unstable::SaveSymbolState;
	using script_state ZKREM("renamed to zenkit::unstable::SaveScriptState") = zenkit::unstable::SaveScriptState;
	using save_game ZKREM("renamed to zenkit::unstable::SaveGame") = zenkit::unstable::SaveGame;
} // namespace phoenix::unstable
