// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Stream.hh"
#include "zenkit/Mmap.hh"

#include "Internal.hh"
#include "zenkit/Vfs.hh"

#include <algorithm>
#include <cstring>
#include <fstream>

namespace zenkit {
	template <typename T>
	ZKINT T read_any(Read* r) noexcept {
		T v {};
		r->read(&v, sizeof v);
		return v;
	}

	template <typename T>
	ZKINT void write_any(Write* r, T const& v) noexcept {
		r->write(&v, sizeof v);
	}

	char Read::read_char() noexcept {
		return read_any<char>(this);
	}

	int8_t Read::read_byte() noexcept {
		return read_any<int8_t>(this);
	}

	uint8_t Read::read_ubyte() noexcept {
		return read_any<uint8_t>(this);
	}

	int16_t Read::read_short() noexcept {
		return read_any<int16_t>(this);
	}

	uint16_t Read::read_ushort() noexcept {
		return read_any<uint16_t>(this);
	}

	int32_t Read::read_int() noexcept {
		return read_any<int32_t>(this);
	}

	uint32_t Read::read_uint() noexcept {
		return read_any<uint32_t>(this);
	}

	float Read::read_float() noexcept {
		return read_any<float>(this);
	}

	Vec2 Read::read_vec2() noexcept {
		Vec2 v {};
		this->read(v.pointer(), sizeof(float) * 2);
		return v;
	}

	Vec3 Read::read_vec3() noexcept {
		Vec3 v {};
		this->read(v.pointer(), sizeof(float) * 3);
		return v;
	}

	Mat3 Read::read_mat3() noexcept {
		Mat3 v {};
		this->read(v.pointer(), sizeof(float) * 9);
		return v.transpose();
	}

	Mat4 Read::read_mat4() noexcept {
		Mat4 v {};
		this->read(v.pointer(), sizeof(float) * 16);
		return v.transpose();
	}

	std::string Read::read_string(size_t len) noexcept {
		std::string str(len, '\0');
		this->read(str.data(), len);
		return str;
	}

	std::string Read::read_line(bool skipws) noexcept {
		return read_line_then_ignore(skipws ? " \t\r\n\v\f" : "");
	}

	std::string Read::read_line_then_ignore(std::string_view chars) noexcept {
		std::string str {};

		char c;
		while (c = this->read_char(), c != '\0' && c != '\r' && c != '\n') {
			str.push_back(c);
		}

		// Don't ignore the given chars if we're at the end of a C-style string.
		if (chars.empty() || c == '\0' || this->eof()) return str;

		do {
			c = this->read_char();
		} while (c != '\0' && chars.find(c) != std::string_view::npos);

		// If we're at EOF and read a null-byte, don't backtrack.
		if (this->eof() && c == '\0') return str;

		// Backtrack by one byte since we have definitely consumed one too many.
		this->seek(-1, Whence::CUR);
		return str;
	}

	void Write::write_char(char v) noexcept {
		write_any(this, v);
	}

	void Write::write_byte(int8_t v) noexcept {
		write_any(this, v);
	}

	void Write::write_ubyte(uint8_t v) noexcept {
		write_any(this, v);
	}

	void Write::write_short(int16_t v) noexcept {
		write_any(this, v);
	}

	void Write::write_ushort(uint16_t v) noexcept {
		write_any(this, v);
	}

	void Write::write_int(int32_t v) noexcept {
		write_any(this, v);
	}

	void Write::write_uint(uint32_t v) noexcept {
		write_any(this, v);
	}

	void Write::write_float(float v) noexcept {
		write_any(this, v);
	}

	void Write::write_string(std::string_view v) noexcept {
		this->write(v.data(), v.length());
	}

	void Write::write_string0(std::string_view v) noexcept {
		this->write(v.data(), v.length());
		this->write_char('\0');
	}

	void Write::write_line(std::string_view v) noexcept {
		this->write_string(v);
		this->write_char('\n');
	}

	void Write::write_vec2(Vec2 const& v) noexcept {
		this->write(v.pointer(), 2 * sizeof(float));
	}

	void Write::write_vec3(Vec3 const& v) noexcept {
		this->write(v.pointer(), 3 * sizeof(float));
	}

	void Write::write_mat3(Mat3 const& v) noexcept {
		auto vT = v.transpose();
		this->write(vT.pointer(), 9 * sizeof(float));
	}

	void Write::write_mat4(Mat4 const& v) noexcept {
		auto vT = v.transpose();
		this->write(vT.pointer(), 16 * sizeof(float));
	}

	namespace detail {
		constexpr int INTO_C_WHENCE[] = {
		    SEEK_SET,
		    SEEK_CUR,
		    SEEK_END,
		};

		constexpr std::ios::seekdir INTO_CXX_WHENCE[] = {
		    std::ios::beg,
		    std::ios::cur,
		    std::ios::end,
		};

		ZKINT inline size_t seek_internal(size_t pos, size_t end, ssize_t off, Whence whence) {
			auto new_position = static_cast<ssize_t>(pos);

			switch (whence) {
			case Whence::BEG:
				new_position = off;
				break;
			case Whence::CUR:
				new_position = new_position + off;
				break;
			case Whence::END:
				new_position = static_cast<ssize_t>(end) + off;
				break;
			}

			return static_cast<size_t>(new_position);
		}

		class ReadFile ZKINT : public Read {
		public:
			explicit ReadFile(FILE* stream) : ReadFile(stream, 0, -1) {}

			ReadFile(FILE* stream, uint64_t offset, int64_t size) : _m_stream(stream), _m_offset(offset), _m_size(size) {
				if (size < 0) {
					fseek(_m_stream, 0, SEEK_END);
					_m_size = ftell(_m_stream) - _m_offset;
					fseek(_m_stream, 0, SEEK_SET);
				}

				fseek(_m_stream, _m_offset, SEEK_SET);
			}

			size_t read(void* buf, size_t len) noexcept override {
				auto remaining = _m_size - tell();
				len = remaining < len ? remaining : len;
				return fread(buf, 1, len, _m_stream);
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				switch (whence) {
				case Whence::BEG:
					fseek(_m_stream, _m_offset + off, SEEK_SET);
					break;
				case Whence::CUR:
					fseek(_m_stream, off, SEEK_CUR);
					break;
				case Whence::END:
					fseek(_m_stream, _m_offset + _m_size + off, SEEK_SET);
					break;
				}
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return ftell(_m_stream) - _m_offset;
			}

			[[nodiscard]] bool eof() const noexcept override {
				return tell() >= _m_size || feof(_m_stream);
			}

		private:
			FILE* _m_stream;
			uint64_t _m_offset = 0;
			uint64_t _m_size = -1;
		};

		static FILE* try_fopen(std::filesystem::path const& path, char const* mode) {
			auto name = path.string();
			FILE* stream = fopen(name.c_str(), mode);
			if (stream == nullptr) {
				throw std::system_error(errno, std::system_category(), "fopen");
			}
			return stream;
		}

		class ReadFileOwned final ZKINT : public ReadFile {
		public:
			explicit ReadFileOwned(std::filesystem::path const& path) : ReadFileOwned(path, 0, -1) {}

			ReadFileOwned(std::filesystem::path const& path, int64_t offset, int64_t size)
			    : ReadFileOwned(try_fopen(path, "rb"), offset, size) {}

			~ReadFileOwned() noexcept override {
				if (_m_own) {
					fclose(_m_own);
				}
			}

		protected:
			ReadFileOwned(FILE* stream, int64_t offset, int64_t size) : ReadFile(stream, offset, size), _m_own(stream) {
			}

		private:
			FILE* _m_own;
		};

		class ReadStream final ZKINT : public Read {
		public:
			explicit ReadStream(std::istream* stream) : _m_stream(stream) {}

			size_t read(void* buf, size_t len) noexcept override {
				_m_stream->read(static_cast<char*>(buf), static_cast<long>(len));
				return static_cast<size_t>(_m_stream->gcount());
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				_m_stream->seekg(off, INTO_CXX_WHENCE[static_cast<int>(whence)]);
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return _m_stream->tellg();
			}

			[[nodiscard]] bool eof() const noexcept override {
				return _m_stream->eof();
			}

		private:
			std::istream* _m_stream;
		};

		class ReadMemory ZKINT : public Read {
		public:
			ReadMemory(std::byte const* byte, size_t len) : _m_bytes(byte), _m_length(len) {}

			size_t read(void* buf, size_t len) noexcept override {
				len = _m_position + len > _m_length ? _m_length - _m_position : len;
				memcpy(buf, _m_bytes + _m_position, len);
				_m_position += len;
				return len;
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				auto new_position = seek_internal(_m_position, _m_length, off, whence);

				if (new_position > _m_length) return;

				_m_position = new_position;
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return _m_position;
			}

			[[nodiscard]] bool eof() const noexcept override {
				return _m_position >= _m_length;
			}

		private:
			std::byte const* _m_bytes;
			size_t _m_length, _m_position {0};
		};

		class ReadVector final ZKINT : public ReadMemory {
		public:
			explicit ReadVector(std::vector<std::byte> vec)
			    : ReadMemory(vec.data(), vec.size()), _m_vector(std::move(vec)) {}

		private:
			std::vector<std::byte> _m_vector;
		};

#ifdef _ZK_WITH_MMAP
		class ReadMmap final ZKINT : public ReadMemory {
		public:
			explicit ReadMmap(std::filesystem::path const& path) : ReadMmap(Mmap {path}) {}

			explicit ReadMmap(Mmap mmap) : ReadMemory(mmap.data(), mmap.size()), _m_mmap(std::move(mmap)) {}

		private:
			Mmap _m_mmap;
		};
#endif

		class WriteFile final ZKINT : public Write {
		public:
			explicit WriteFile(FILE* stream) : _m_stream(stream) {}

			size_t write(void const* buf, size_t len) noexcept override {
				return fwrite(buf, 1, len, _m_stream);
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				fseek(_m_stream, off, INTO_C_WHENCE[static_cast<int>(whence)]);
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return static_cast<size_t>(ftell(_m_stream));
			}

		private:
			FILE* _m_stream;
		};

		class WriteStream final ZKINT : public Write {
		public:
			explicit WriteStream(std::ostream* stream) : _m_stream(stream), _m_own(false) {}
			explicit WriteStream(std::filesystem::path const& path)
			    : _m_stream(new std::ofstream(path, std::ios::binary | std::ios::out | std::ios::trunc)), _m_own(true) {
			}

			~WriteStream() noexcept override {
				if (_m_own) {
					delete _m_stream;

					_m_own = false;
					_m_stream = nullptr;
				}
			}

			size_t write(void const* buf, size_t len) noexcept override {
				_m_stream->write(static_cast<char const*>(buf), static_cast<long>(len));
				return len;
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				_m_stream->seekp(off, INTO_CXX_WHENCE[static_cast<int>(whence)]);
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return _m_stream->tellp();
			}

		private:
			std::ostream* _m_stream;
			bool _m_own;
		};

		class WriteStatic final ZKINT : public Write {
		public:
			explicit WriteStatic(std::byte* buf, size_t len) : _m_bytes(buf), _m_length(len) {}

			size_t write(void const* buf, size_t len) noexcept override {
				len = _m_position + len > _m_length ? _m_length - _m_position : len;
				memcpy(_m_bytes + _m_position, buf, len);
				_m_position += len;
				return len;
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				auto new_position = seek_internal(_m_position, _m_length, off, whence);

				if (new_position > _m_length) return;
				_m_position = new_position;
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return _m_position;
			}

		private:
			std::byte* _m_bytes;
			size_t _m_length, _m_position {0};
		};

		class WriteDynamic final ZKINT : public Write {
		public:
			explicit WriteDynamic(std::vector<std::byte>* vec) : _m_vector(vec) {}

			size_t write(void const* buf, size_t len) noexcept override {
				auto const* cbuf = static_cast<std::byte const*>(buf);

				if (_m_position + len > _m_vector->capacity()) {
					this->_m_vector->reserve(
					    len + this->_m_vector->capacity() < 1024 ? 1024 : this->_m_vector->capacity() * 2);
				}

				if (_m_position + len > _m_vector->size()) {
					_m_vector->resize(_m_position + len);
				}

				std::copy_n(cbuf, len, _m_vector->data() + _m_position);
				_m_position += len;
				return len;
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				_m_position = seek_internal(_m_position, _m_vector->size(), off, whence);
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return _m_position;
			}

		private:
			std::vector<std::byte>* _m_vector;
			size_t _m_position {0};
		};
	} // namespace detail

	std::unique_ptr<Read> Read::from(FILE* stream) {
		return std::make_unique<detail::ReadFile>(stream);
	}

	std::unique_ptr<Read> Read::from(std::istream* stream) {
		return std::make_unique<detail::ReadStream>(stream);
	}

	std::unique_ptr<Read> Read::from(std::byte const* bytes, size_t len) {
		return std::make_unique<detail::ReadMemory>(bytes, len);
	}

	std::unique_ptr<Read> Read::from(std::vector<std::byte> const* vector) {
		return std::make_unique<detail::ReadMemory>(vector->data(), vector->size());
	}

	std::unique_ptr<Read> Read::from(std::vector<std::byte> vector) {
		return std::make_unique<detail::ReadVector>(std::move(vector));
	}

	std::unique_ptr<Read> Read::from(std::filesystem::path const& path) {
#ifdef _ZK_WITH_MMAP
		return std::make_unique<detail::ReadMmap>(path);
#else
		return from(path, 0, -1);
#endif
	}

	std::unique_ptr<Read> Read::from(std::filesystem::path const& path, int64_t offset, int64_t size) {
		return std::make_unique<detail::ReadFileOwned>(path, offset, size);
	}

	std::unique_ptr<Write> Write::to(std::filesystem::path const& path) {
		return std::make_unique<detail::WriteStream>(path);
	}

	std::unique_ptr<Write> Write::to(FILE* stream) {
		return std::make_unique<detail::WriteFile>(stream);
	}

	std::unique_ptr<Write> Write::to(std::ostream* stream) {
		return std::make_unique<detail::WriteStream>(stream);
	}

	std::unique_ptr<Write> Write::to(std::byte* bytes, size_t len) {
		return std::make_unique<detail::WriteStatic>(bytes, len);
	}

	std::unique_ptr<Write> Write::to(std::vector<std::byte>* vector) {
		return std::make_unique<detail::WriteDynamic>(vector);
	}
} // namespace zenkit
