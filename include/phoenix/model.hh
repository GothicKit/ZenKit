// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <string>

#include <phoenix/model_hierarchy.hh>
#include <phoenix/model_mesh.hh>

namespace phoenix {
	class model {
	public:
		/**
		 * @brief Parses a model from a file.
		 * @param in The buffer to read from.
		 * @return The model just read.
		 */
		[[nodiscard]] static model parse(buffer& in);

		/**
		 * @brief Parses a model from a file.
		 * @param in The buffer to read from.
		 * @return The model just read.
		 */
		[[nodiscard]] inline static model parse(buffer&& in) {
			return parse(in);
		}

		[[nodiscard]] const model_hierachy& hierachy() const noexcept {
			return _m_hierarchy;
		}

		[[nodiscard]] const model_mesh& mesh() const noexcept {
			return _m_mesh;
		}

	private:
		model_hierachy _m_hierarchy;
		model_mesh _m_mesh;
	};
} // namespace phoenix
