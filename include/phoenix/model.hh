// Copyright © 2022 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#pragma once
#include <string>

#include <phoenix/model_hierarchy.hh>
#include <phoenix/model_mesh.hh>

namespace phoenix {
	class model {
	public:
		/**
		 * @brief Parses a model from a file.
		 * @param path The path of the file to read from
		 * @return The model just read.
		 */
		static model parse(buffer& path);

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
