// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

#include <glm/mat3x3.hpp>

#include <memory>


namespace phoenix {
	class vob {
	public:
		static vob read_tree(archive_reader_ref& in, u16 root_version) {
			in->skip_object(false);
			return {};
		}

	private:
		std::vector<std::unique_ptr<vob>> _m_children;

		glm::vec3 _m_bbox[2];
		glm::vec3 _m_position;
		glm::mat3x3 _m_rotation;
		bool _m_show_visual;
		u8 _m_camera_alignment;
		bool _m_cd_static;
		bool _m_cd_dynamic;
		bool _m_vob_static;
		bool _m_dynamic_shadows;
		bool _m_physics_enabled;
		u8 _m_animation_mode;
		s32 _m_bias;
		bool _m_ambient;
		float _m_animation_strength;
		float _m_far_clip_scale;

		std::string _m_preset_name;
		std::string _m_vob_name;
		std::string _m_visual_name;
	};

}// namespace phoenix
