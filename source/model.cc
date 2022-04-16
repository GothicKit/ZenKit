// Copyright © 2022 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/model.hh>

namespace phoenix {
	model model::parse(const std::string& path) {
		model tmp {};
		auto in = buffer::open(path);

		tmp._m_hierarchy = model_hierachy::parse(in);
		tmp._m_mesh = model_mesh::parse(in);

		return tmp;
	}
} // namespace phoenix
