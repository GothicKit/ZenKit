// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Archive.hh"

namespace phoenix {
	using archive_format ZKREM("renamed to zenkit::ArchiveFormat") = zenkit::ArchiveFormat;
	using archive_header ZKREM("renamed to zenkit::ArchiveHeader") = zenkit::ArchiveHeader;
	using archive_object ZKREM("renamed to zenkit::ArchiveObject") = zenkit::ArchiveObject;
	using archive_reader ZKREM("renamed to zenkit::ArchiveReader") = zenkit::ReadArchive;
} // namespace phoenix
