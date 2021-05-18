// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

#include <glm/mat3x3.hpp>

#include <memory>


namespace phoenix {
	class vob_tree {
	public:
		static vob_tree read_tree(archive_reader_ref& in, game_version version);

		inline const std::vector<vob_tree>& children() const noexcept { return _m_children; }

		inline std::tuple<glm::vec3, glm::vec3> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }
		inline glm::vec3 position() const noexcept { return _m_position; }
		inline glm::mat3x3 rotation() const noexcept { return _m_rotation; }
		inline bool show_visual() const noexcept { return _m_show_visual;}
		inline u8 camera_alignment() const noexcept { return _m_camera_alignment; }
		inline u8 animation_mode() const noexcept { return _m_animation_mode; }
		inline s32 bias() const noexcept { return _m_bias; }
		inline f32 animation_strength() const noexcept { return _m_animation_strength; }
		inline f32 far_clip_scale() const noexcept { return _m_far_clip_scale; }
		inline u8 dynamic_shadows() const noexcept { return _m_dynamic_shadows; }

		inline const std::string& preset_name() const noexcept { return _m_preset_name; }
		inline const std::string& vob_name() const noexcept { return _m_vob_name; }

		inline const std::string& visual_name() const noexcept { return _m_visual_name; }
		inline bool is_cd_static() const noexcept { return _m_cd_static; }
		inline bool is_cd_dynamic() const noexcept { return _m_cd_dynamic; }
		inline bool is_vob_static() const noexcept { return _m_vob_static; }
		inline bool is_physics_enabled() const noexcept { return _m_physics_enabled; }
		inline bool is_ambient() const noexcept { return _m_ambient; }

	private:
		std::vector<vob_tree> _m_children;

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
