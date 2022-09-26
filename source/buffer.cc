// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#include "phoenix/buffer.hh"

#include <fmt/format.h>
#include <mio/mmap.hpp>

#include <fstream>

namespace phoenix {
	namespace detail {
		/// \brief A buffer backing which saves data on the heap.
		class heap_backing : public buffer_backing {
		public:
			/// \brief Creates a new, empty heap buffer backing with the given size.
			/// \param size The number of bytes in the backing.
			explicit heap_backing(std::uint64_t size) : _m_readonly(false) {
				_m_data.resize(size);
			}

			/// \brief Creates a new heap buffer backing from the given vector.
			/// \param buf The data to insert into the backing.
			/// \param readonly Set to `false` to allow writing to the backing.
			heap_backing(std::vector<std::byte>&& buf, bool readonly)
			    : _m_data(std::move(buf)), _m_readonly(readonly) {}

			[[nodiscard]] bool direct() const noexcept override {
				return false;
			}

			[[nodiscard]] bool readonly() const noexcept override {
				return _m_readonly;
			}

			[[nodiscard]] uint64_t size() const noexcept override {
				return _m_data.size();
			}

			[[nodiscard]] const std::byte* array() const override {
				return _m_data.data();
			}

			void read(std::byte* buf, std::uint64_t size, std::uint64_t offset) const override {
				if (offset + size > this->size()) {
					throw buffer_underflow {offset, size, "in backing"};
				}

				std::copy_n(_m_data.cbegin() + static_cast<long>(offset), size, buf);
			}

			void write([[maybe_unused]] const std::byte* buf,
			           [[maybe_unused]] std::uint64_t size,
			           [[maybe_unused]] std::uint64_t offset) override {
				if (this->readonly()) {
					throw buffer_readonly {};
				}

				if (offset + size > this->size()) {
					throw buffer_overflow {offset, size, "in backing"};
				}

				std::copy_n(buf, size, const_cast<std::byte*>(_m_data.data()) + static_cast<long>(offset));
			}

		private:
			std::vector<std::byte> _m_data;
			bool _m_readonly;
		};

		/// \brief A buffer backing for memory-mapped files.
		template <mio::access_mode mode>
		class mmap_backing : public buffer_backing {
		public:
			/// \brief Creates a new memory-mapped buffer backing by mapping the file at the given path into memory
			/// \param file The path of the file to map
			explicit mmap_backing(const std::filesystem::path& file) : _m_data(file.c_str()) {}

			[[nodiscard]] bool direct() const noexcept override {
				return true;
			}

			[[nodiscard]] bool readonly() const noexcept override {
				return mode == mio::access_mode::read;
			}

			[[nodiscard]] uint64_t size() const noexcept override {
				return _m_data.size();
			}

			[[nodiscard]] const std::byte* array() const noexcept override {
				return _m_data.data();
			}

			void read(std::byte* buf, std::uint64_t size, std::uint64_t offset) const override {
				if (offset + size > this->size()) {
					throw buffer_underflow {offset, size, "in backing"};
				}

				std::copy_n(_m_data.cbegin() + static_cast<long>(offset), size, buf);
			}

			void write([[maybe_unused]] const std::byte* buf,
			           [[maybe_unused]] std::uint64_t size,
			           [[maybe_unused]] std::uint64_t offset) override {
				if (this->readonly()) {
					throw buffer_readonly {};
				}

				if (offset + size > this->size()) {
					throw buffer_overflow {offset, size, "in backing"};
				}

				std::copy_n(buf, size, const_cast<std::byte*>(_m_data.data()) + static_cast<long>(offset));
			}

		private:
			mio::basic_mmap<mode, std::byte> _m_data;
		};
	} // namespace detail

	buffer_underflow::buffer_underflow(std::uint64_t byte, std::uint64_t size)
	    : buffer_error(fmt::format("buffer underflow at byte {:X} while reading {:X} additional bytes", byte, size)),
	      byte(byte), size(size), context(std::nullopt) {}

	buffer_underflow::buffer_underflow(std::uint64_t byte, std::uint64_t size, std::string&& context)
	    : buffer_error(fmt::format("buffer underflow at byte {:X} while reading {:X} additional bytes [context: {}]",
	                               byte,
	                               size,
	                               context)),
	      byte(byte), size(size), context(std::move(context)) {}

	buffer_underflow::buffer_underflow(std::uint64_t byte, std::string&& context)
	    : buffer_error(fmt::format("buffer underflow at byte {:X} [context: {}]", byte, context)), byte(byte), size(0),
	      context(std::move(context)) {}

	buffer_overflow::buffer_overflow(std::uint64_t byte, std::uint64_t size)
	    : buffer_error(fmt::format("buffer overflow at byte {:X} while writing {:X} additional bytes", byte, size)),
	      byte(byte), size(size), context(std::nullopt) {}

	buffer_overflow::buffer_overflow(std::uint64_t byte, std::uint64_t size, std::string&& context)
	    : buffer_error(fmt::format("buffer overflow at byte {:X} while writing {:X} additional bytes [context: {}]",
	                               byte,
	                               size,
	                               context)),
	      byte(byte), size(size), context(std::move(context)) {}

	std::unique_ptr<buffer> buffer::_m_empty {};

	buffer::buffer(std::shared_ptr<buffer_backing> backing)
	    : _m_backing(std::move(backing)), _m_backing_begin(0), _m_backing_end(_m_backing->size()),
	      _m_capacity(_m_backing->size()), _m_position(0) {}

	buffer::buffer(std::shared_ptr<buffer_backing> backing, std::uint64_t begin, std::uint64_t end)
	    : _m_backing(std::move(backing)), _m_backing_begin(begin), _m_backing_end(end), _m_capacity(end - begin),
	      _m_position(0) {}

	buffer::buffer(std::shared_ptr<buffer_backing> backing,
	               std::uint64_t begin,
	               std::uint64_t end,
	               std::uint64_t capacity,
	               std::uint64_t position,
	               std::optional<std::uint64_t> mark)
	    : _m_backing(std::move(backing)), _m_backing_begin(begin), _m_backing_end(end), _m_capacity(capacity),
	      _m_position(position), _m_mark(mark) {}

	buffer buffer::allocate(std::uint64_t size) {
		return buffer {std::make_shared<detail::heap_backing>(size)};
	}

	buffer buffer::of(std::vector<std::byte>&& buf, bool readonly) {
		return buffer {std::make_shared<detail::heap_backing>(std::forward<std::vector<std::byte>>(buf), readonly)};
	}

	buffer buffer::mmap(const std::filesystem::path& path, bool readonly) {
		if (readonly) {
			return buffer {std::make_shared<detail::mmap_backing<mio::access_mode::read>>(path)};
		}

		return buffer {std::make_shared<detail::mmap_backing<mio::access_mode::write>>(path)};
	}

	buffer buffer::read(const std::filesystem::path& path, bool readonly) {
		std::ifstream in {path, std::ios::binary | std::ios::ate};
		std::vector<std::byte> data {static_cast<size_t>(in.tellg())};

		in.seekg(0);
		in.read((char*) data.data(), data.size());

		return buffer::of(std::move(data), readonly);
	}

	buffer buffer::empty() {
		if (buffer::_m_empty == nullptr) {
			// If we don't have an empty buffer yet, create one.
			buffer::_m_empty = std::make_unique<buffer>(buffer::allocate(0));
		}

		return buffer::_m_empty->duplicate();
	}

	void buffer::clear() noexcept {
		_m_position = 0;
		_m_backing_end = _m_backing_begin + _m_capacity;
		_m_mark.reset();
	}

	buffer buffer::duplicate() const noexcept {
		return buffer {_m_backing, _m_backing_begin, _m_backing_end, _m_capacity, _m_position, _m_mark};
	}

	void buffer::flip() noexcept {
		_m_backing_end = _m_backing_begin + _m_position;
		_m_position = 0;
		_m_mark.reset();
	}

	void buffer::limit(std::uint64_t limit) {
		if (limit > this->capacity()) {
			throw buffer_underflow {limit, "setting limit"};
		}

		_m_position = std::min(limit, _m_position);
		_m_backing_end = _m_backing_begin + limit;

		if (_m_mark && *_m_mark > limit) {
			_m_mark.reset();
		}
	}

	void buffer::position(std::uint64_t pos) {
		if (pos > this->limit()) {
			throw buffer_underflow {pos, "setting position"};
		}

		if (_m_mark && *_m_mark > pos) {
			_m_mark.reset();
		}

		_m_position = pos;
	}

	buffer buffer::slice() const noexcept {
		return buffer {_m_backing, _m_backing_begin + _m_position, _m_backing_end};
	}

	buffer buffer::slice(std::uint64_t index, std::uint64_t size) const {
		if (index + size > this->limit()) {
			throw buffer_underflow {index, size, "slicing"};
		}

		return buffer {_m_backing, _m_backing_begin + index, _m_backing_begin + index + size};
	}

	void buffer::get(std::byte* buf, std::uint64_t size) {
		if (this->remaining() < size) {
			throw buffer_underflow {this->position(), size, "relative bulk get"};
		}

		_m_backing->read(buf, size, _m_backing_begin + _m_position);
		_m_position += size;
	}

	void buffer::get(std::uint64_t index, std::byte* buf, std::uint64_t size) const {
		if (index + size > this->limit()) {
			throw buffer_underflow {index, size, "absolute bulk get"};
		}

		_m_backing->read(buf, size, _m_backing_begin + index);
	}

	std::string buffer::get_string(std::uint64_t size) {
		if (this->remaining() < size) {
			throw buffer_overflow {position(), size, "relative string get"};
		}

		std::string tmp {};
		tmp.resize(size);
		this->get((std::byte*) tmp.data(), size);
		return tmp;
	}

	std::string buffer::get_string(std::uint64_t index, std::uint64_t size) const {
		if (index + size > this->limit()) {
			throw buffer_overflow {index, size, "absolute string get"};
		}

		std::string tmp {};
		tmp.resize(size);
		this->get(index, (std::byte*) tmp.data(), size);
		return tmp;
	}

	std::string buffer::get_line(bool skip_whitespace) {
		auto lf = mismatch([](char chr) { return chr == '\n' || chr == '\0' || chr == '\r'; });

		if (lf == -1) {
			throw buffer_underflow {position(), "relative line get"};
		}

		auto tmp = get_string(lf);
		(void) get_char(); // ignore the <LF> character itself

		if (skip_whitespace) {
			auto count = mismatch([](char chr) { return !std::isspace(chr); });
			if (count == -1) {
				position(limit()); // the end of the buffer has been reached
			} else {
				position(position() + count);
			}
		}

		return tmp;
	}

	std::string buffer::get_line_at(std::uint64_t index) const {
		auto lf = mismatch(index, [](char chr) { return chr == '\n' || chr == '\0'; });

		if (lf == -1) {
			throw buffer_underflow {index, "absolute line get"};
		}

		return get_string(index, lf);
	}

	std::int64_t buffer::mismatch(const buffer& other) const {
		auto remaining = std::min(this->remaining(), other.remaining());

		for (std::uint64_t i = 0; i < remaining; ++i) {
			if (this->get_char(this->position() + i) != other.get_char(other.position() + i)) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	std::int64_t buffer::mismatch(std::uint64_t index, const buffer& other) const {
		if (index > this->limit()) {
			throw buffer_underflow {index, "absolute mismatch"};
		}

		auto remaining = std::min(this->limit() - index, other.remaining());
		for (std::uint64_t i = 0; i < remaining; ++i) {
			if (this->get_char(index + i) != other.get_char(other.position() + i)) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	std::int64_t buffer::mismatch(const std::function<bool(std::uint8_t)>& each) const {
		for (std::uint64_t i = 0; i < this->remaining(); ++i) {
			if (each(this->get_char(this->position() + i))) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	std::int64_t buffer::mismatch(std::uint64_t index, const std::function<bool(std::uint8_t)>& each) const {
		if (index > this->limit()) {
			throw buffer_underflow {index, "absolute mismatch"};
		}

		auto remaining = limit() - index;
		for (std::uint64_t i = 0; i < remaining; ++i) {
			if (each(this->get_char(index + i))) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	void buffer::put(const std::byte* buf, std::uint64_t size) {
		if (this->remaining() < size) {
			throw buffer_overflow {this->position(), size, "relative bulk put"};
		}

		_m_backing->write(buf, size, _m_backing_begin + _m_position);
		_m_position += size;
	}

	void buffer::put_string(std::string_view str) {
		this->put((const std::byte*) str.data(), str.size());
	}

	void buffer::put_line(std::string_view str) {
		put_string(str);
		put_char('\n');
	}

	bool operator==(const buffer& self, const buffer& other) {
		return &other == &self ||
		    (other._m_backing == self._m_backing && other._m_backing_begin == self._m_backing_begin &&
		     other._m_backing_end == self._m_backing_end && other._m_capacity == self._m_capacity &&
		     other._m_position == self._m_position);
	}
} // namespace phoenix
