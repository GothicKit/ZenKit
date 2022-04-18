// Copyright © 2022 Luis Michaelis
// Licensed under MIT (https://mit-license.org/).
#include <mio/mmap.hpp>
#include <phoenix/detail/buffer.hh>

namespace phoenix {
	namespace detail {
		/**
		 * @brief A buffer backing which saves data on the heap.
		 */
		class heap_backing : public buffer_backing {
		public:
			/**
			 * @brief Creates a new, empty heap buffer backing with the given size.
			 * @param size The number of bytes in the backing.
			 */
			explicit heap_backing(std::uint64_t size) : _m_readonly(false) {
				_m_data.resize(size);
			}

			/**
			 * @brief Creates a new heap buffer backing from the given vector.
			 * @param buf The data to insert into the backing.
			 * @param readonly Set to `false` to allow writing to the backing.
			 */
			heap_backing(std::vector<std::uint8_t>&& buf, bool readonly)
			    : _m_data(std::move(buf)), _m_readonly(readonly) {}

			[[nodiscard]] bool direct() const noexcept override {
				return false;
			}

			[[nodiscard]] bool readonly() const noexcept override {
				return _m_readonly;
			}

			[[nodiscard]] std::span<const std::uint8_t> array(std::uint64_t off, std::uint64_t size) const override {
				if (off + size > _m_data.size())
					throw buffer_underflow_error {};

				return std::span<const std::uint8_t> {_m_data.begin() + static_cast<long>(off),
				                                      _m_data.begin() + static_cast<long>(off + size)};
			}

			[[nodiscard]] uint64_t size() const noexcept override {
				return _m_data.size();
			}

			void read(void* buf, std::uint64_t offset, std::uint64_t size) const override {
				if (offset + size > _m_data.size())
					throw buffer_underflow_error {};

				std::copy_n(_m_data.begin() + static_cast<long>(offset), size, static_cast<std::uint8_t*>(buf));
			}

			void write(const void* buf, std::uint64_t offset, std::uint64_t size) override {
				if (readonly())
					throw buffer_readonly_error {};

				if (offset + size > _m_data.size())
					throw buffer_overflow_error {};

				std::copy_n(static_cast<const std::uint8_t*>(buf), size, _m_data.begin() + static_cast<long>(offset));
			}

		private:
			std::vector<std::uint8_t> _m_data;
			bool _m_readonly;
		};

		/**
		 * @brief A buffer backing for memory-mapped files.
		 */
		template <mio::access_mode mode>
		class mmap_backing : public buffer_backing {
		public:
			/**
			 * @brief Creates a new memory-mapped buffer backing by mapping the file at the given path into memory
			 * @param file The path of the file to map
			 */
			explicit mmap_backing(const std::string& file) : _m_data(file) {}

			[[nodiscard]] bool direct() const noexcept override {
				return true;
			}

			[[nodiscard]] bool readonly() const noexcept override {
				return mode == mio::access_mode::read;
			}

			[[nodiscard]] std::span<const std::uint8_t> array(std::uint64_t off, std::uint64_t size) const override {
				if (off + size > _m_data.size())
					throw buffer_underflow_error {};

				return std::span<const std::uint8_t> {_m_data.begin() + off, _m_data.begin() + off + size};
			}

			[[nodiscard]] uint64_t size() const noexcept override {
				return _m_data.size();
			}

			void read(void* buf, std::uint64_t offset, std::uint64_t size) const override {
				if (offset + size > _m_data.size())
					throw buffer_underflow_error {};

				std::copy_n(_m_data.begin() + static_cast<long>(offset), size, static_cast<char*>(buf));
			}

			void write(const void* buf, std::uint64_t offset, std::uint64_t size) override {
				if (readonly())
					throw buffer_readonly_error {};

				if (offset + size > _m_data.size())
					throw buffer_overflow_error {};

				// Yes kids, const_cast is very bad, but we've checked if we're read-only already.
				std::copy_n(static_cast<const std::uint8_t*>(buf),
				            size,
				            const_cast<std::uint8_t*>(_m_data.begin()) + static_cast<long>(offset));
			}

		private:
			mio::basic_mmap<mode, std::uint8_t> _m_data;
		};
	} // namespace detail

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
	               bool mark_set,
	               std::uint64_t mark)
	    : _m_backing(std::move(backing)), _m_backing_begin(begin), _m_backing_end(end), _m_capacity(capacity),
	      _m_position(position), _m_mark_set(mark_set), _m_mark(mark) {}

	buffer buffer::allocate(std::uint64_t size) {
		return buffer {std::make_shared<detail::heap_backing>(size)};
	}

	buffer buffer::wrap(std::vector<std::uint8_t>&& buf, bool readonly) {
		return buffer {std::make_shared<detail::heap_backing>(std::forward<std::vector<std::uint8_t>>(buf), readonly)};
	}

	buffer buffer::open(const std::string& path, bool readonly) {
		if (readonly) {
			return buffer {std::make_shared<detail::mmap_backing<mio::access_mode::read>>(path)};
		}

		return buffer {std::make_shared<detail::mmap_backing<mio::access_mode::write>>(path)};
	}

	buffer buffer::empty() {
		if (buffer::_m_empty == nullptr) {
			// If we don't have an empty buffer yet, create one.
			buffer::_m_empty = std::make_unique<buffer>(buffer::allocate(0));
		}

		return buffer::_m_empty->duplicate();
	}

	buffer& buffer::clear() noexcept {
		_m_position = 0;
		_m_backing_end = _m_backing_begin + _m_capacity;
		_m_mark_set = false;
		return *this;
	}

	buffer buffer::duplicate() const {
		return buffer {_m_backing, _m_backing_begin, _m_backing_end, _m_capacity, _m_position, _m_mark_set, _m_mark};
	}

	buffer& buffer::flip() noexcept {
		_m_backing_end = _m_backing_begin + _m_position;
		_m_position = 0;
		_m_mark_set = false;
		return *this;
	}

	buffer& buffer::limit(std::uint64_t new_limit) {
		if (new_limit > _m_capacity)
			throw buffer_overflow_error {};

		_m_position = std::min(new_limit, _m_position);
		_m_backing_end = _m_backing_begin + new_limit;

		if (_m_mark_set && _m_mark > new_limit)
			_m_mark_set = false;

		return *this;
	}

	buffer& buffer::position(std::uint64_t new_position) {
		if (new_position > limit())
			throw buffer_underflow_error {};

		if (_m_mark_set && _m_mark > new_position)
			_m_mark_set = false;

		_m_position = new_position;
		return *this;
	}

	buffer& buffer::skip(std::uint64_t count) {
		if (remaining() < count)
			throw buffer_underflow_error {};
		_m_position += count;
		return *this;
	}

	buffer buffer::slice() const {
		return buffer {_m_backing, _m_backing_begin + _m_position, _m_backing_end};
	}

	buffer buffer::slice(std::uint64_t index, std::uint64_t size) const {
		if (index + size > limit())
			throw buffer_underflow_error {};

		return buffer {_m_backing, _m_backing_begin + index, _m_backing_begin + index + size};
	}

	buffer& buffer::get(std::span<std::uint8_t> buf) {
		if (remaining() < buf.size())
			throw buffer_underflow_error {};

		_m_backing->read(buf.data(), _m_backing_begin + _m_position, buf.size());
		_m_position += buf.size();
		return *this;
	}

	const buffer& buffer::get(std::uint64_t index, std::span<std::uint8_t> buf) const {
		if (index + buf.size() > limit())
			throw buffer_underflow_error {};

		_m_backing->read(buf.data(), _m_backing_begin + index, buf.size());
		return *this;
	}

	buffer& buffer::put(std::span<const std::uint8_t> buf) {
		if (remaining() < buf.size())
			throw buffer_overflow_error {};

		_m_backing->write(buf.data(), _m_backing_begin + _m_position, buf.size());
		_m_position += buf.size();
		return *this;
	}

	std::string buffer::get_string(std::uint64_t size) {
		if (remaining() < size)
			throw buffer_overflow_error {};

		std::string tmp {};
		tmp.resize(size);
		get({(uint8_t*) tmp.begin().base(), (uint8_t*) tmp.end().base()});
		return tmp;
	}

	std::string buffer::get_string(std::uint64_t index, std::uint64_t size) const {
		if (index + size > limit())
			throw buffer_overflow_error {};

		std::string tmp {};
		tmp.resize(size);
		get(index, {(uint8_t*) tmp.begin().base(), (uint8_t*) tmp.end().base()});
		return tmp;
	}

	std::string buffer::get_line(bool skip_whitespace) {
		auto lf = mismatch([](char chr) { return chr == '\n' || chr == '\0'; });

		if (lf == -1)
			throw buffer_underflow_error {};

		auto tmp = get_string(lf);
		(void) get_char(); // ignore the <LF> character itself

		if (skip_whitespace)
			position(position() + std::max(0L, mismatch([](char chr) { return !std::isspace(chr); })));

		return tmp;
	}

	std::string buffer::get_line_at(std::uint64_t index) const {
		auto lf = mismatch(index, [](char chr) { return chr == '\n' || chr == '\0'; });

		if (lf == -1)
			throw buffer_underflow_error {};

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
		if (index > limit())
			throw buffer_underflow_error {};

		auto remaining = std::min(limit() - index, other.remaining());
		for (std::uint64_t i = 0; i < remaining; ++i) {
			if (this->get_char(index + i) != other.get_char(other.position() + i)) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	std::int64_t buffer::mismatch(const std::function<bool(std::uint8_t)>& each) const {
		for (std::uint64_t i = 0; i < remaining(); ++i) {
			if (each(this->get_char(this->position() + i))) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	std::int64_t buffer::mismatch(std::uint64_t index, const std::function<bool(std::uint8_t)>& each) const {
		if (index > limit())
			throw buffer_underflow_error {};

		auto remaining = limit() - index;
		for (std::uint64_t i = 0; i < remaining; ++i) {
			if (each(this->get_char(index + i))) {
				return static_cast<long>(i);
			}
		}

		return -1;
	}

	buffer& buffer::put_string(std::string_view str) {
		return put({(uint8_t*) str.begin(), (uint8_t*) str.end()});
	}

	buffer& buffer::put_line(std::string_view str) {
		return put_string(str).put_char('\n');
	}

	bool operator==(const buffer& self, const buffer& other) {
		return &other == &self ||
		    (other._m_backing == self._m_backing && other._m_backing_begin == self._m_backing_begin &&
		     other._m_backing_end == self._m_backing_end && other._m_capacity == self._m_capacity &&
		     other._m_position == self._m_position);
	}
} // namespace phoenix
