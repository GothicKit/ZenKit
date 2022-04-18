// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
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
