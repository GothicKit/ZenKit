// Copyright © 2022 Luis Michaelis <lmichaelis.all+dev@gmail.com>
// SPDX-License-Identifier: MIT
#pragma once
#include <phoenix/animation.hh>
#include <phoenix/font.hh>
#include <phoenix/mesh.hh>
#include <phoenix/messages.hh>
#include <phoenix/model_hierarchy.hh>
#include <phoenix/model_script.hh>
#include <phoenix/texture.hh>
#include <phoenix/vdfs.hh>

#include <fmt/format.h>

namespace px = phoenix;

template <bool double_escaped_backslash = true>
inline std::string escape(char chr) {
	if (std::isprint(chr)) {
		if (chr == '"')
			return double_escaped_backslash ? "\\\"" : "\"";
		if (chr == '\\')
			return double_escaped_backslash ? "\\\\" : "\\";
		return std::string {chr};
	} else {
		if constexpr (double_escaped_backslash) {
			return fmt::format("\\\\x{:0>2x}", static_cast<uint8_t>(chr));
		} else {
			return fmt::format("\\x{:0>2x}", static_cast<uint8_t>(chr));
		}
	}
}

inline std::string escape(const std::string& str) {
	std::string new_str {};
	new_str.reserve(str.length());

	for (auto chr : str) {
		if (chr == '"') {
			new_str += "\\\"";
		} else if (chr == '\\') {
			new_str += "\\\\";
		} else if (chr == '\n') {
			new_str += "\\n";
		} else if (chr == '\r') {
			new_str += "\\r";
		} else if (chr == '\t') {
			new_str += "\\t";
		} else {
			new_str += chr;
		}
	}

	return new_str;
}

/**
 * @brief Print the given object as human readable text to <tt>stdout</tt>.
 * @tparam T The type of object to print.
 * @param obj The object to print.
 */
template <typename T>
void dump_text(const T& obj);

/**
 * @brief Print the given object as JSON text to <tt>stdout</tt>.
 * @tparam T The type of object to print.
 * @param obj The object to print.
 */
template <typename T>
void dump_json(const T& obj);

/**
 * @brief Helper function for printing the given object as human readable text or JSON to <tt>stdout</tt>.
 * @tparam T The type of object to print.
 * @param obj The object to print.
 * @param json Set to <tt>true</tt> to print JSON text. Prints human readable text otherwise.
 */
template <typename T>
void dump(const T& obj, bool json) {
	if (json) {
		dump_json(obj);
	} else {
		dump_text(obj);
	}
}

/**
 * @brief Prints information about the given font to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::font>(const px::font& obj);

/**
 * @brief Prints all relevant data of the given font to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::font>(const px::font& obj);

/**
 * @brief Prints information about the given message database to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::messages>(const px::messages& obj);

/**
 * @brief Prints all relevant data of the given message database to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::messages>(const px::messages& obj);

/**
 * @brief Prints information about the given animation to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::animation>(const px::animation& obj);

/**
 * @brief Prints all relevant data of the given animation to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::animation>(const px::animation& obj);

/**
 * @brief Prints information about the given skeleton to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::model_hierachy>(const px::model_hierachy& obj);

/**
 * @brief Prints all relevant data of the given skeleton to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::model_hierachy>(const px::model_hierachy& obj);

/**
 * @brief Prints information about the given texture to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::texture>(const px::texture& obj);

/**
 * @brief Prints all relevant data of the given texture to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::texture>(const px::texture& obj);

/**
 * @brief Prints information about the given mesh to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::mesh>(const px::mesh& obj);

/**
 * @brief Prints all relevant data of the given mesh to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::mesh>(const px::mesh& obj);

/**
 * @brief Prints information about the given model script to <tt>stdout</tt>.
 * @param fnt The object to print.
 */
template <>
void dump_text<px::model_script>(const px::model_script& obj);

/**
 * @brief Prints all relevant data of the given model script to <tt>stdout</tt> as JSON text.
 * @param fnt The object to print.
 */
template <>
void dump_json<px::model_script>(const px::model_script& obj);
