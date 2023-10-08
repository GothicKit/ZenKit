// Copyright © 2023 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Mmap.hh"

#include <windows.h>

namespace zenkit {
	struct Platform {
		HANDLE hFile;
		HANDLE hFileMapping;
	};

	Mmap::Mmap(std::filesystem::path const& path) {
		HANDLE hFile;

		hFile = CreateFileW(path.c_str(),
		                    GENERIC_READ,
		                    FILE_SHARE_READ,
		                    nullptr,
		                    OPEN_EXISTING,
		                    FILE_ATTRIBUTE_NORMAL,
		                    nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			throw std::runtime_error {"Failed to open " + path.string()};
		}

		_m_size = GetFileSize(hFile, nullptr);

		HANDLE hFileMapping = CreateFileMappingA(hFile, nullptr, PAGE_READONLY, 0, _m_size, nullptr);
		if (hFileMapping == nullptr) {
			CloseHandle(hFile);
			throw std::runtime_error {"Failed to memory-map " + path.string()};
		}

		_m_data = static_cast<std::byte const*>(MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0));
		_m_platform_handle = new Platform {hFile, hFileMapping};
	}

	Mmap::Mmap(Mmap&& other) noexcept {
		_m_data = other._m_data;
		_m_size = other._m_size;
		_m_platform_handle = other._m_platform_handle;

		other._m_data = nullptr;
		other._m_size = 0;
		other._m_platform_handle = nullptr;
	}

	Mmap::~Mmap() noexcept {
		if (_m_platform_handle != nullptr) {
			UnmapViewOfFile(_m_data);

			auto* platform = reinterpret_cast<Platform*>(_m_platform_handle);
			CloseHandle(platform->hFileMapping);
			CloseHandle(platform->hFile);
			delete platform;

			_m_data = nullptr;
			_m_size = 0;
			_m_platform_handle = nullptr;
		}
	}
} // namespace zenkit
