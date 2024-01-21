// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "phoenix.hh"
#include "zenkit/ModelScript.hh"

namespace phoenix {
	namespace mds {
		ZKREM("use zenkit::AnimationFlags::NONE") static constexpr auto af_none = zenkit::AnimationFlags::NONE;
		ZKREM("use zenkit::AnimationFlags::MOVE") static constexpr auto af_move = zenkit::AnimationFlags::MOVE;
		ZKREM("use zenkit::AnimationFlags::ROTATE") static constexpr auto af_rotate = zenkit::AnimationFlags::ROTATE;
		ZKREM("use zenkit::AnimationFlags::QUEUE") static constexpr auto af_queue = zenkit::AnimationFlags::QUEUE;
		ZKREM("use zenkit::AnimationFlags::FLY") static constexpr auto af_fly = zenkit::AnimationFlags::FLY;
		ZKREM("use zenkit::AnimationFlags::IDLE") static constexpr auto af_idle = zenkit::AnimationFlags::IDLE;
		ZKREM("use zenkit::AnimationFlags::INPLACE") static constexpr auto af_inplace = zenkit::AnimationFlags::INPLACE;

		using animation_direction ZKREM("renamed to zenkit::AnimationDirection") = zenkit::AnimationDirection;
		using animation_flags ZKREM("use zenkit::AnimationFlags instead") = zenkit::AnimationFlags;
		using event_fight_mode ZKREM("renamed to zenkit::MdsFightMode") = zenkit::MdsFightMode;
		using event_tag_type ZKREM("renamed to zenkit::MdsEventType") = zenkit::MdsEventType;
		using skeleton ZKREM("renamed to zenkit::MdsSkeleton") = zenkit::MdsSkeleton;
		using model_tag ZKREM("renamed to zenkit::MdsModelTag") = zenkit::MdsModelTag;
		using event_tag ZKREM("renamed to zenkit::MdsEventTag") = zenkit::MdsEventTag;
		using event_pfx ZKREM("renamed to zenkit::MdsParticleEffect") = zenkit::MdsParticleEffect;
		using event_camera_tremor ZKREM("renamed to zenkit::MdsCameraTremor") = zenkit::MdsCameraTremor;
		using event_pfx_stop ZKREM("renamed to zenkit::MdsParticleEffectStop") = zenkit::MdsParticleEffectStop;
		using event_sfx ZKREM("renamed to zenkit::MdsSoundEffect") = zenkit::MdsSoundEffect;
		using event_sfx_ground ZKREM("renamed to zenkit::MdsSoundEffectGround") = zenkit::MdsSoundEffectGround;
		using event_morph_animate ZKREM("renamed to zenkit::MdsMorphAnimation") = zenkit::MdsMorphAnimation;
		using animation ZKREM("renamed to zenkit::MdsAnimation") = zenkit::MdsAnimation;
		using animation_alias ZKREM("renamed to zenkit::MdsAnimationAlias") = zenkit::MdsAnimationAlias;
		using animation_blending ZKREM("renamed to zenkit::MdsAnimationBlend") = zenkit::MdsAnimationBlend;
		using animation_combination ZKREM("renamed to zenkit::MdsAnimationCombine") = zenkit::MdsAnimationCombine;
	} // namespace mds

	struct script_sytax_error final : zenkit::ParserError {
		ZKINT script_sytax_error(std::string&&, std::string&&) : ParserError("") {}
	};

	using model_script ZKREM("renamed to zenkit::ModelScript") = zenkit::ModelScript;
} // namespace phoenix
