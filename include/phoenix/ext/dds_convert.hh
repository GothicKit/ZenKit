// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once

#include <phoenix/texture.hh>

namespace phoenix {
	/**
	 * @brief Converts a texture to the DDS format.
	 * @param tex The texture to convert.
	 * @return A buffer containing the DDS file.
	 */
	buffer texture_to_dds(const texture& tex);
}
