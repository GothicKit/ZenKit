// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#pragma once
#include <filesystem>

namespace zenkit {
#ifdef _ZK_WITH_MMAP
	class Mmap {
	public:
		explicit Mmap(std::filesystem::path const& path);

		Mmap(Mmap const&) = delete;
		Mmap(Mmap&&) noexcept;

		~Mmap() noexcept;

		[[nodiscard]] inline std::byte const* data() const noexcept {
			return _m_data;
		}

		[[nodiscard]] inline std::size_t size() const noexcept {
			return _m_size;
		}

	private:
		std::byte const* _m_data;
		std::size_t _m_size;

		[[maybe_unused]] void* _m_platform_handle {nullptr};
	};
#endif
} // namespace zenkit
