// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include "zenkit/Vfs.hh"

namespace phoenix {
	using VfsBrokenDiskError ZKREM("renamed to zenkit::VfsBrokenDiskError") = zenkit::VfsBrokenDiskError;
	using VfsFileExistsError ZKREM("renamed to zenkit::VfsFileExistsError") = zenkit::VfsFileExistsError;
	using VfsNotFoundError ZKREM("renamed to zenkit::VfsNotFoundError") = zenkit::VfsNotFoundError;
	using VfsNodeType ZKREM("renamed to zenkit::VfsNodeType") = zenkit::VfsNodeType;
	using VfsFileDescriptor ZKREM("renamed to zenkit::VfsFileDescriptor") = zenkit::VfsFileDescriptor;
	using VfsNode ZKREM("renamed to zenkit::VfsNode") = zenkit::VfsNode;
	using VfsNodeComparator ZKREM("renamed to zenkit::VfsNodeComparator") = zenkit::VfsNodeComparator;
	using VfsNode ZKREM("renamed to zenkit::VfsNode") = zenkit::VfsNode;
	using VfsOverwriteBehavior ZKREM("renamed to zenkit::VfsOverwriteBehavior") = zenkit::VfsOverwriteBehavior;
	using Vfs ZKREM("renamed to zenkit::Vfs") = zenkit::Vfs;
} // namespace phoenix
