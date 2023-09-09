// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Library.hh"
#include "zenkit/addon/texcvt.hh"

#include "phoenix/buffer.hh"

namespace phoenix {
	/// \brief Converts a texture to the DDS format.
	/// \param tex The texture to convert.
	/// \return A buffer containing the DDS file.
	[[nodiscard]] ZKREM("use zenkit::to_dds") inline buffer texture_to_dds(zenkit::Texture const& tex) {
		return buffer::of(zenkit::to_dds(tex));
	}
} // namespace phoenix
