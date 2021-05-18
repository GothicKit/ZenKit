// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <phoenix/detail/stream.hh>
#include <vector>

namespace phoenix {
	enum class bsp_tree_mode : u32 {
		indoor = 0,
		outdoor = 1,
	};

	class bsp_tree {
	public:
		static bsp_tree read(reader& in);

		[[nodiscard]] bsp_tree_mode mode() const noexcept { return _m_mode; }
		[[nodiscard]] const std::vector<u32>& polygon_indices() const noexcept { return _m_polygon_indices; }

	private:
		bsp_tree_mode _m_mode;
		std::vector<u32> _m_polygon_indices;
	};
}