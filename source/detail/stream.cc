// Copyright © 2021 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <phoenix/detail/error.hh>
#include <phoenix/detail/stream.hh>

#include <fmt/format.h>

namespace phoenix {
	template <typename SELF>
	std::string base_reader<SELF>::read_string(u64 size) {
		std::string tmp;
		tmp.resize(size);
		read(tmp.data(), size);
		return tmp;
	}

	// TODO: Clean this up. Use another base class for reading from iterators?

	reader::reader(std::shared_ptr<mio::mmap_source> source)
	    : _m_source(std::move(source)), _m_begin(_m_source->begin()), _m_current(_m_begin), _m_end(_m_source->end()) {}

	reader::reader(std::shared_ptr<mio::mmap_source> source,
	               mio::mmap_source::const_iterator begin,
	               mio::mmap_source::const_iterator end)
	    : _m_source(std::move(source)), _m_begin(begin), _m_current(begin), _m_end(end) {}

	reader reader::from(const std::string& path) {
		return reader {std::make_shared<mio::mmap_source>(path)};
	}

	void reader::seek(u64 offset) {
		if (_m_begin + offset > _m_end) {
			throw io_error(fmt::format("reader: refusing to seek to {} (max. {}): out-of-range", offset, size()));
		}

		_m_current = _m_begin + offset;
	}

	void reader::ignore(u64 count) {
		if (_m_current + count > _m_end) {
			throw io_error(fmt::format("reader: refusing to ignore {} bytes starting at {} (max. {}): out-of-range",
			                           count,
			                           tell(),
			                           size()));
		}

		_m_current += count;
	}

	reader reader::fork(u64 size, s64 offset) {
		if (offset < 0) {
			if (_m_current + size > _m_end) {
				throw io_error(fmt::format("reader: refusing to fork at {} with size {} (max. {}): out-of-range",
				                           tell(),
				                           size,
				                           this->size()));
			}

			auto forked = reader {_m_source, _m_current, _m_current + size};
			_m_current += size;

			return forked;
		} else {
			if (_m_begin + offset + size > _m_end) {
				throw io_error(fmt::format("reader: refusing to fork at {} with size {} (max. {}): out-of-range",
				                           offset,
				                           size,
				                           this->size()));
			}

			return reader {_m_source, _m_begin + offset, _m_begin + offset + size};
		}
	}

	void reader::read(void* buffer, u64 size) {
		if (_m_current + size > _m_end) {
			throw io_error(fmt::format("reader: refusing to read {} bytes at {} (max. {}): out-of-range",
			                           size,
			                           tell(),
			                           this->size()));
		}

		std::copy_n(_m_current, size, static_cast<char*>(buffer));
		_m_current += size;
	}

	std::string reader::read_line(bool skipws) {
		auto end_of_line =
		    std::find_if(_m_current, _m_end, [](char chr) { return chr == '\r' || chr == '\n' || chr == '\0'; });
		auto tmp = read_string(end_of_line - _m_current);

		// don't forget to skip the char we originally matched :)
		_m_current++;

		if (skipws) {
			_m_current = std::find_if_not(_m_current, _m_end, [](char chr) { return std::isspace(chr); });
		}

		return tmp;
	}

	vector_reader::vector_reader(std::vector<u8>&& source)
	    : _m_source(std::move(source)), _m_begin(_m_source.begin()), _m_current(_m_begin), _m_end(_m_source.end()) {}

	void vector_reader::seek(u64 offset) {
		if (_m_begin + offset > _m_end) {
			throw io_error(fmt::format("reader: refusing to seek to {} (max. {}): out-of-range", offset, size()));
		}

		_m_current = _m_begin + offset;
	}

	void vector_reader::ignore(u64 count) {
		if (_m_current + count > _m_end) {
			throw io_error(fmt::format("reader: refusing to ignore {} bytes starting at {} (max. {}): out-of-range",
			                           count,
			                           tell(),
			                           size()));
		}

		_m_current += count;
	}

	vector_reader vector_reader::fork(u64 size, s64 offset) {
		if (offset < 0) {
			if (_m_current + size > _m_end) {
				throw io_error(fmt::format("reader: refusing to fork at {} with size {} (max. {}): out-of-range",
				                           tell(),
				                           size,
				                           this->size()));
			}

			std::vector<u8> copy {};
			std::copy(_m_current, _m_current + offset, std::back_insert_iterator(copy));

			auto forked = vector_reader {std::move(copy)};
			_m_current += size;

			return forked;
		} else {
			if (_m_begin + offset + size > _m_end) {
				throw io_error(fmt::format("reader: refusing to fork at {} with size {} (max. {}): out-of-range",
				                           offset,
				                           size,
				                           this->size()));
			}

			std::vector<u8> copy {};
			std::copy(_m_source.begin() + offset, _m_source.end() + offset + size, std::back_insert_iterator(copy));
			return vector_reader {std::move(copy)};
		}
	}

	void vector_reader::read(void* buffer, u64 size) {
		if (_m_current + size > _m_end) {
			throw io_error(fmt::format("reader: refusing to read {} bytes at {} (max. {}): out-of-range",
			                           size,
			                           tell(),
			                           this->size()));
		}

		std::copy_n(_m_current, size, static_cast<char*>(buffer));
		_m_current += size;
	}

	std::string vector_reader::read_line(bool skipws) {
		auto end_of_line =
		    std::find_if(_m_current, _m_end, [](char chr) { return chr == '\r' || chr == '\n' || chr == '\0'; });
		auto tmp = read_string(end_of_line - _m_current);

		// don't forget to skip the char we originally matched :)
		_m_current++;

		if (skipws) {
			_m_current = std::find_if_not(_m_current, _m_end, [](char chr) {
				return chr == '\r' || chr == '\n' || chr == '\0';
			});
		}

		return tmp;
	}

	writer::writer(std::ostream& out) : _m_stream(out), _m_begin(out.tellp()) {
		try {
			_m_stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
		} catch (const std::ios::failure& fail) { throw io_error(fail.what()); }
	}

	void writer::write(const void* data, u64 size) {
		try {
			_m_stream.write(static_cast<const char*>(data), static_cast<s32>(size));
		} catch (const std::ios::failure& fail) { throw io_error(fail.what()); }
	}

	void writer::seek(u64 offset) {
		try {
			_m_stream.seekp(static_cast<s64>(_m_begin + offset));
		} catch (const std::ios::failure& fail) { throw io_error(fail.what()); }
	}
} // namespace phoenix
