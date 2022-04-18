// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/proto_mesh.hh>
#include <phoenix/softskin_mesh.hh>

namespace phoenix {

	class model_mesh {
	public:
		static model_mesh parse(buffer& in);

		const std::vector<softskin_mesh>& meshes() const noexcept {
			return _m_meshes;
		}

		const std::unordered_map<std::string, proto_mesh>& attachments() const noexcept {
			return _m_attachments;
		}

	private:
		std::vector<softskin_mesh> _m_meshes;
		std::unordered_map<std::string, proto_mesh> _m_attachments;
	};

} // namespace phoenix
