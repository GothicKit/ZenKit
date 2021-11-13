// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/detail/stream.hh>

#include <fmt/format.h>

phoenix::reader phoenix::reader::from(const std::string& path) {
	try {
		auto src = std::make_shared<mio::mmap_source>(path);
		return reader {
				std::string_view {src->data(), static_cast<u64>(src->size())},
				src};
	} catch (const std::system_error& e) {
		throw io_error(e.what());
	}
}

namespace phoenix {
	reader::reader(std::string_view buffer) : _m_buffer(buffer) {
	}

	reader::reader(std::string_view buffer, std::shared_ptr<mio::mmap_source> source)
		: _m_source(std::move(source)), _m_buffer(buffer), _m_offset(0) {
	}

	void reader::seek(u64 offset) {
		if (offset > _m_buffer.size()) {
			throw io_error(fmt::format("reader: refusing to seek to {} (max. {}): out-of-range", offset, _m_buffer.size()));
		}

		_m_offset = offset;
	}

	void reader::ignore(u64 count) {
		if (_m_offset + count > _m_buffer.size()) {
			throw io_error(fmt::format("reader: refusing to ignore {} bytes starting at {} (max. {}): out-of-range", count, _m_offset, _m_buffer.size()));
		}

		_m_offset += count;
	}

	reader reader::fork(u64 size, s64 offset) {
		if (offset < 0) {
			if (_m_offset + size > _m_buffer.size()) {
				throw io_error(fmt::format("reader: refusing to fork at {} with size {} (max. {}): out-of-range", _m_offset, size, _m_buffer.size()));
			}

			auto forked = reader {_m_buffer.substr(_m_offset, size)};
			_m_offset += size;

			return forked;
		} else {
			if (offset + size > _m_buffer.size()) {
				throw io_error(fmt::format("reader: refusing to fork at {} with size {} (max. {}): out-of-range", offset, size, _m_buffer.size()));
			}

			return reader {_m_buffer.substr(offset, size)};
		}
	}

	void reader::read(void* buffer, u64 size) {
		if (_m_offset + size > _m_buffer.size()) {
			throw io_error(fmt::format("reader: refusing to read {} bytes at {} (max. {}): out-of-range", size, _m_offset, _m_buffer.size()));
		}

		_m_buffer.copy(static_cast<char*>(buffer), size, _m_offset);
		_m_offset += size;
	}

	std::string reader::read_string(u64 size) {
		std::string tmp;
		tmp.resize(size);
		read(tmp.data(), size);
		return tmp;
	}

	std::string reader::read_line(bool skipws) {
		auto end_of_line = _m_buffer.find_first_of(std::string_view {"\r\n\0", 3}, _m_offset);
		auto tmp = read_string(end_of_line - _m_offset);

		// don't forget to skip the char we originally matched :)
		_m_offset++;

		if (skipws) {
			_m_offset = _m_buffer.find_first_not_of(" \t\n\r", _m_offset);
		}

		return tmp;
	}

	writer::writer(std::ostream& out) : _m_stream(out), _m_begin(out.tellp()) {
		try {
			_m_stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
		} catch (const std::ios::failure& fail) {
			throw io_error(fail.what());
		}
	}

	void writer::write(const void* data, u64 size) {
		try {
			_m_stream.write(static_cast<const char*>(data), static_cast<s32>(size));
		} catch (const std::ios::failure& fail) {
			throw io_error(fail.what());
		}
	}

	void writer::seek(u64 offset) {
		try {
			_m_stream.seekp(static_cast<s64>(_m_begin + offset));
		} catch (const std::ios::failure& fail) {
			throw io_error(fail.what());
		}
	}
}// namespace phoenix
