// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/texture.hh>
#include <memory>

namespace phoenix {
	struct light_map {
		std::shared_ptr<texture> texture;
		glm::vec3 normals[2];
		glm::vec3 origin;
	};
}