// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Mmap.hh"

#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace zenkit {
	Mmap::Mmap(std::filesystem::path const& path) {
		auto handle = open(path.c_str(), O_RDONLY);

		if (handle == -1) {
			throw std::runtime_error {"Failed to open " + path.string()};
		}

		struct ::stat st {};
		if (fstat(handle, &st) != 0) {
			throw std::runtime_error {"Failed to stat " + path.string()};
		}

		_m_data = (std::byte*) ::mmap(nullptr, (::size_t) st.st_size, PROT_READ, MAP_SHARED, handle, 0);
		if (_m_data == nullptr) {
			throw std::runtime_error {"Failed to mmap " + path.string()};
		}

		_m_size = static_cast<std::size_t>(st.st_size);
		_m_platform_handle = (void*) _m_data;

		close(handle);
	}

	Mmap::Mmap(Mmap&& other) noexcept {
		_m_data = other.data();
		_m_size = other.size();

		other._m_data = nullptr;
		other._m_size = 0;
	}

	Mmap::~Mmap() noexcept {
		if (_m_data != nullptr) {
			::munmap((void*) _m_data, _m_size);
			_m_data = nullptr;
			_m_platform_handle = nullptr;
		}
	}
} // namespace zenkit
