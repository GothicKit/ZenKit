// Copyright © 2022-2024 GothicKit Contributors.
// SPDX-License-Identifier: MIT
#include "zenkit/Stream.hh"
#include "zenkit/Mmap.hh"

#include "Internal.hh"

#include <algorithm>
#include <cstring>
#include <fstream>
#ifdef _ZK_WITH_ZIPPED_VDF
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#include <miniz.h>
#endif

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

		class ReadFile final ZKINT : public Read {
		public:
			explicit ReadFile(FILE* stream) : _m_stream(stream) {}

			size_t read(void* buf, size_t len) noexcept override {
				return fread(buf, 1, len, _m_stream);
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				fseek(_m_stream, off, INTO_C_WHENCE[static_cast<int>(whence)]);
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return static_cast<size_t>(ftell(_m_stream));
			}

			[[nodiscard]] bool eof() const noexcept override {
				return feof(_m_stream);
			}

		private:
			FILE* _m_stream;
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
		std::vector<std::byte> data {};
		std::ifstream stream {path, std::ios::ate | std::ios::binary | std::ios::in};
		data.resize(static_cast<size_t>(stream.tellg()));
		stream.seekg(0);
		stream.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));

		return Read::from(std::move(data));
#endif
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
	// -----------------------------------------------------------------------------------------------------------------

#ifdef _ZK_WITH_ZIPPED_VDF
	namespace detail {
		/// Reads file data stored as a Union ZippedStream.
		///
		/// In a zipped VDF (VolumeHeader.Flags == 0xA0), the catalog/file table remains
		/// uncompressed, but each file's data at its catalog offset is stored as a
		/// ZippedStream — a block-compressed format used by Union's VDFS library.
		///
		/// ZippedStream layout (at the file's offset):
		///   Stream header:  Length (4) | BlockSize (4) | BlocksCount (4)
		///   Per block (interleaved):
		///     Block header:  LengthSource (4) | LengthCompressed (4) | BlockSize (4)
		///     Block data:    [LengthCompressed bytes of zlib-compressed data]
		///
		/// Each block is independently zlib-compressed and can be decompressed on demand.
		class ReadZipped final : public Read {
		public:
			explicit ReadZipped(std::unique_ptr<Read> r) : _m_stream(std::move(r)) {
				_m_stream->seek(0, Whence::BEG);
			}

			~ReadZipped() override = default;

			size_t read(void* buf, size_t len) noexcept override {
				// Implementation of reading logic using blocks
				uint8_t* out = static_cast<uint8_t*>(buf);
				size_t total_read = 0;

				while (len > 0) {
					if (_m_current_block >= _m_header.blocks_count) break;

					// Ensure current block is cached/decompressed
					if (!_m_cache_valid || _m_cache_idx != _m_current_block) {
						if (!cache_block(_m_current_block)) return total_read;
					}

					size_t offset_in_block = _m_position - (_m_current_block * _m_header.block_size);
					size_t available = _m_blocks[_m_current_block].len_src - offset_in_block;
					size_t to_copy = std::min(len, available);

					memcpy(out, _m_cache.data() + offset_in_block, to_copy);

					out += to_copy;
					len -= to_copy;
					total_read += to_copy;
					_m_position += to_copy;

					if (to_copy == available) {
						_m_current_block++;
					}
				}
				return total_read;
			}

			void seek(ssize_t off, Whence whence) noexcept override {
				// Update _m_position and _m_current_block
				size_t new_pos = 0;
				if (whence == Whence::BEG)
					new_pos = off;
				else if (whence == Whence::CUR)
					new_pos = _m_position + off;
				else if (whence == Whence::END)
					new_pos = _m_header.length_uncompressed + off; // stored in header

				_m_position = new_pos;
				if (_m_header.block_size > 0) _m_current_block = _m_position / _m_header.block_size;
			}

			[[nodiscard]] size_t tell() const noexcept override {
				return _m_position;
			}

			[[nodiscard]] bool eof() const noexcept override {
				return _m_position >= _m_header.length_uncompressed;
			}

			/// Reads the ZippedStream header and block table from the underlying stream.
			/// Block headers and data are interleaved: each block header (12 bytes) is
			/// immediately followed by its compressed payload (len_cmp bytes).
			bool init() {
				try {
					_m_header.length_uncompressed = _m_stream->read_uint();
					_m_header.block_size = _m_stream->read_uint();
					_m_header.blocks_count = _m_stream->read_uint();

					// Validate: a valid ZippedStream must have at least one block
					// and a non-zero block size.
					if (_m_header.blocks_count == 0 || _m_header.block_size == 0 ||
					    _m_header.length_uncompressed == 0) {
						return false;
					}

					// Validate: blocks_count must be consistent with the header.
					// In a valid ZippedStream, blocks_count == ceil(length / block_size).
					// Reject if it doesn't match — the data is not a ZippedStream.
					uint32_t expected_blocks =
					    (_m_header.length_uncompressed + _m_header.block_size - 1) / _m_header.block_size;
					if (_m_header.blocks_count != expected_blocks) {
						return false;
					}

					_m_blocks.resize(_m_header.blocks_count);

					// Scan through the interleaved block headers to record each
					// block's compressed data offset, then skip past its data.
					for (auto& blk : _m_blocks) {
						blk.len_src = _m_stream->read_uint();
						blk.len_cmp = _m_stream->read_uint();
						blk.size_blk = _m_stream->read_uint();
						blk.offset = _m_stream->tell(); // compressed data starts here
						_m_stream->seek(static_cast<ssize_t>(blk.len_cmp), Whence::CUR);
					}

					return true;
				} catch (...) {
					return false;
				}
			}

		private:
			std::unique_ptr<Read> _m_stream;

			struct StreamHeader {
				uint32_t length_uncompressed;
				uint32_t block_size;
				uint32_t blocks_count;
			} _m_header {};

			struct BlockInfo {
				uint32_t len_src;
				uint32_t len_cmp;
				uint32_t size_blk;
				size_t offset;
			};
			std::vector<BlockInfo> _m_blocks;

			size_t _m_position = 0;
			uint32_t _m_current_block = 0;

			// Cache
			std::vector<uint8_t> _m_cache;
			uint32_t _m_cache_idx = 0xFFFFFFFF;
			bool _m_cache_valid = false;

			bool cache_block(uint32_t idx) {
				if (idx >= _m_blocks.size()) return false;

				BlockInfo& blk = _m_blocks[idx];
				_m_stream->seek(blk.offset, Whence::BEG);
				std::vector<uint8_t> cmp_data(blk.len_cmp);
				if (_m_stream->read(cmp_data.data(), blk.len_cmp) != blk.len_cmp) return false;

				_m_cache.resize(blk.len_src);

				mz_ulong out_len = static_cast<mz_ulong>(blk.len_src);
				int res = mz_uncompress(_m_cache.data(), &out_len, cmp_data.data(), static_cast<mz_ulong>(blk.len_cmp));

				if (res != MZ_OK) return false;

				_m_cache_idx = idx;
				_m_cache_valid = true;
				return true;
			}
		};
	} // namespace detail

	std::unique_ptr<Read> Read::from_zipped(std::unique_ptr<Read> stream) {
		auto reader = std::make_unique<detail::ReadZipped>(std::move(stream));
		if (!reader->init()) return nullptr;
		return reader;
	}
#endif // _ZK_WITH_ZIPPED_VDF
} // namespace zenkit
