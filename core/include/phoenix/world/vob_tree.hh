// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/archive.hh>

#include <glm/mat3x3.hpp>

#include <memory>


namespace phoenix {
	/**
	 * @brief Represents ZenGin's VOB tree.
	 * @note This is not currently fully finished.
	 */
	class vob_tree {
	public:
		/**
		 * @brief Parses a VOB tree from the given reader.
		 * @param in The reader to read from.
		 * @param version The version of Gothic being used.
		 * @return The tree parsed.
		 */
		[[nodiscard]] static vob_tree parse(archive_reader_ref& in, game_version version);

		/**
		 * @return A list of child VOBs.
		 */
		[[nodiscard]] inline const std::vector<vob_tree>& children() const noexcept { return _m_children; }

		/**
		 * @return The bounding box of the VOB as a (min, max) tuple.
		 */
		[[nodiscard]] inline std::tuple<glm::vec3, glm::vec3> bbox() const noexcept { return std::make_tuple(_m_bbox[0], _m_bbox[1]); }

		/**
		 * @return The position of the VOB
		 */
		[[nodiscard]] inline glm::vec3 position() const noexcept { return _m_position; }

		/**
		 * @return The rotation of the VOB
		 */
		[[nodiscard]] inline glm::mat3x3 rotation() const noexcept { return _m_rotation; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool show_visual() const noexcept { return _m_show_visual;}

		/**
		 * @return ???
		 */
		[[nodiscard]] inline u8 camera_alignment() const noexcept { return _m_camera_alignment; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline u8 animation_mode() const noexcept { return _m_animation_mode; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline s32 bias() const noexcept { return _m_bias; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline f32 animation_strength() const noexcept { return _m_animation_strength; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline f32 far_clip_scale() const noexcept { return _m_far_clip_scale; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline u8 dynamic_shadows() const noexcept { return _m_dynamic_shadows; }

		/**
		 * @return The name of the preset being used
		 */
		[[nodiscard]] inline const std::string& preset_name() const noexcept { return _m_preset_name; }

		/**
		 * @return The name of the VOB
		 */
		[[nodiscard]] inline const std::string& vob_name() const noexcept { return _m_vob_name; }

		/**
		 * @return The name of the attached visual
		 */
		[[nodiscard]] inline const std::string& visual_name() const noexcept { return _m_visual_name; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_cd_static() const noexcept { return _m_cd_static; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_cd_dynamic() const noexcept { return _m_cd_dynamic; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_vob_static() const noexcept { return _m_vob_static; }

		/**
		 * @return `true` if physics should be activated for the VOB
		 */
		[[nodiscard]] inline bool is_physics_enabled() const noexcept { return _m_physics_enabled; }

		/**
		 * @return ???
		 */
		[[nodiscard]] inline bool is_ambient() const noexcept { return _m_ambient; }

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
