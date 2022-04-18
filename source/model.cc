// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include <phoenix/model.hh>

namespace phoenix {
	model model::parse(buffer& buf) {
		model tmp {};

		tmp._m_hierarchy = model_hierachy::parse(buf);
		tmp._m_mesh = model_mesh::parse(buf);
		return tmp;
	}
} // namespace phoenix
