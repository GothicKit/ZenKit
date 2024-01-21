// Copyright © 2022-2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/Texture.hh"

#include <vector>

namespace zenkit {
	/// \brief Converts a texture to the DDS format.
	/// \param tex The texture to convert.
	/// \return A buffer containing the DDS file.
	[[nodiscard]] ZKAPI std::vector<std::byte> to_dds(Texture const& tex);
} // namespace zenkit
