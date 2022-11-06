// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <fmt/format.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#if PHOENIX_LOG_LEVEL > 0
#define PX_LOGE(...) phoenix::logging::log(phoenix::logging::level::error, fmt::format(__VA_ARGS__))
#else
#defined PX_LOGE(...)
#endif

#if PHOENIX_LOG_LEVEL > 1
#define PX_LOGW(...) phoenix::logging::log(phoenix::logging::level::warn, fmt::format(__VA_ARGS__))
#else
#defined PX_LOGW(...)
#endif

#if PHOENIX_LOG_LEVEL > 2
#define PX_LOGI(...) phoenix::logging::log(phoenix::logging::level::info, fmt::format(__VA_ARGS__))
#else
#defined PX_LOGI(...)
#endif

#if PHOENIX_LOG_LEVEL > 3
#define PX_LOGD(...) phoenix::logging::log(phoenix::logging::level::debug, fmt::format(__VA_ARGS__))
#else
#define PX_LOGD(...)
#endif

namespace phoenix {
	class buffer;
	class archive_reader;

	/// \brief An enum for providing a game version hint to some functions
	enum class game_version {
		gothic_1, ///< Represents any patch of Gothic
		gothic_2, ///< Represents any patch of Gothic II, including _Night of the Raven_.
	};

	/// \brief Tests whether two strings are equal when ignoring case.
	///
	/// Internally, uses std::tolower to compare the strings character by character.
	///
	/// \param a A string.
	/// \param b Another string.
	/// \return ``true`` if both strings are equal when ignoring case.
	bool iequals(std::string_view a, std::string_view b);

	/// \brief Performs a binary search within the given iterator pair.
	///
	/// Taken directly from The example implementation of `lower_bound` found on cppreference
	/// https://en.cppreference.com/w/cpp/algorithm/lower_bound#Possible_implementation
	///
	/// \tparam ForwardIt The type of iterator.
	/// \tparam T The type of value to search for.
	/// \tparam Compare The type of the comparator function.
	/// \param first The begin iterator
	/// \param last The end iterator
	/// \param value The value to search for.
	/// \param comp A function comparing a value from the iterator with the given search value.
	/// \return An iterator pointing to the first matched value in the input range or \p last if no elements matched.
	template <class ForwardIt, class T, class Compare>
	ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
		ForwardIt it;
		typename std::iterator_traits<ForwardIt>::difference_type count, step;
		count = std::distance(first, last);

		while (count > 0) {
			it = first;
			step = count / 2;
			std::advance(it, step);

			if (comp(*it, value)) {
				first = ++it;
				count -= step + 1;
			} else {
				count = step;
			}
		}

		return first;
	}

	/// \brief A basic datetime structure used by the *ZenGin*.
	struct date {
		/// \brief Parses a date from a buffer.
		/// \param buf The buffer to read from
		/// \return The date.
		static date parse(buffer& buf);

		std::uint32_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
	};

	/// \brief Logging manager for phoenix.
	class logging {
	public:
		enum class level { error, warn, info, debug };

		/// \brief Supply a custom logger callback to be used for log output from phoenix.
		/// \param callback The callback to use.
		static void use_logger(std::function<void(level, const std::string&)>&& callback);

		/// \brief Use the default logger callback for phoenix.
		static void use_default_logger();

		/// \brief Send a logging event to the underlying log callback.
		/// \param lvl The level of the log message.
		/// \param message The message to send.
		static void log(level lvl, std::string&& message) {
			if (logging::callback) {
				(*logging::callback)(lvl, message);
			}
		}

	private:
		static std::optional<std::function<void(level, const std::string&)>> callback;
	};

	/// \brief Base class for all exceptions.
	class error : public std::exception {
	public:
		explicit error(std::string&& message);

		[[nodiscard]] inline const char* what() const noexcept override {
			return message.c_str();
		}

	public:
		const std::string message;
	};

	/// \brief An error representing a parsing failure of any kind.
	class parser_error : public error {
	public:
		explicit parser_error(std::string&& resource_type);
		explicit parser_error(std::string&& resource_type, std::string&& context);
		explicit parser_error(std::string&& resource_type, const std::exception& cause);
		explicit parser_error(std::string&& resource_type, const std::exception& cause, std::string&& context);

	public:
		const std::string resource_type;
		const std::optional<std::string> context {std::nullopt};
		const std::optional<std::exception> cause {std::nullopt};
	};

	template <typename T>
	T parse([[maybe_unused]] buffer& buf) {
		throw parser_error {"unknown", "parsing routine not implemented"};
	}

	template <typename T>
	inline T parse(buffer&& buf) {
		return parse<T>(buf);
	}

	template <typename T>
	T parse([[maybe_unused]] archive_reader& buf) {
		throw parser_error {"unknown", "parsing routine not implemented"};
	}
} // namespace phoenix
