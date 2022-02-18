// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <memory>
#include <phoenix/texture.hh>

namespace phoenix {
	struct light_map {
		std::shared_ptr<texture> image;
		glm::vec3 normals[2];
		glm::vec3 origin;
	};
} // namespace phoenix
