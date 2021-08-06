// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <iomanip>
#include <iostream>
#include <gtest/gtest.h>
#include <json/json.hpp>

using namespace std::literals;

struct cxx_string {
	std::string_view val;

	friend std::ostream& operator<<(std::ostream& out, cxx_string const& data) {
		if (data.val.empty()) return out << "\"\"";
		out << '"';
		for (auto c : data.val) {
			auto const uc = static_cast<unsigned char>(c);
			switch (c) {
				case '\\':
					out << "\\\\";
					break;
				case '\"':
					out << "\\\"";
					break;
				case '\a':
					out << "\\a";
					break;
				case '\b':
					out << "\\b";
					break;
				case '\f':
					out << "\\f";
					break;
				case '\n':
					out << "\\n";
					break;
				case '\r':
					out << "\\r";
					break;
				case '\t':
					out << "\\t";
					break;
				case '\v':
					out << "\\v";
					break;
				default:
					if (std::isprint(uc)) {
						out << c;
					} else {
						out << "\\" << std::oct << std::setw(3)
						    << std::setfill('0') << static_cast<unsigned>(uc)
						    << std::dec;
					}
					break;
			}
		}
		return out << "\"";
	}
};

struct cxx_u8string {
	json::string_view val;

	template <class String, class StringView>
	static inline String convert(StringView src) {
		String out;
		auto source = src.begin();
		auto sourceEnd = src.end();
		auto target = std::back_inserter(out);

		while (source < sourceEnd) {
			bool ok = false;
			char32_t ch = decode(source, sourceEnd, ok);
			if (!ok) return {};

			encode(ch, target);
		}

		return out;
	}

	static inline void encode(
	    char32_t ch,
	    std::back_insert_iterator<std::u32string>& target) {
		*target++ = ch;
	}

	/*
	 * Copyright 2001-2004 Unicode, Inc.
	 *
	 * Disclaimer
	 *
	 * This source code is provided as is by Unicode, Inc. No claims are
	 * made as to fitness for any particular purpose. No warranties of any
	 * kind are expressed or implied. The recipient agrees to determine
	 * applicability of information provided. If this file has been
	 * purchased on magnetic or optical media from Unicode, Inc., the
	 * sole remedy for any claim will be exchange of defective media
	 * within 90 days of receipt.
	 *
	 * Limitations on Rights to Redistribute This Code
	 *
	 * Unicode, Inc. hereby grants the right to freely use the information
	 * supplied in this file in the creation of products supporting the
	 * Unicode Standard, and to make copies of this file in any form
	 * for internal or external distribution as long as this notice
	 * remains attached.
	 */

	template <typename T>
	static bool isLegalUTF8(T source, int length) {
		uint8_t a;
		auto srcptr = source + length;
		switch (length) {
			default:
				return false;
				/* Everything else falls through when "true"... */
			case 4:
				if ((a = static_cast<uint8_t>(*--srcptr)) < 0x80 || a > 0xBF)
					return false;
				[[fallthrough]];
			case 3:
				if ((a = static_cast<uint8_t>(*--srcptr)) < 0x80 || a > 0xBF)
					return false;
				[[fallthrough]];
			case 2:
				if ((a = static_cast<uint8_t>(*--srcptr)) < 0x80 || a > 0xBF)
					return false;

				switch (static_cast<uint8_t>(*source)) {
						/* no fall-through in this inner switch */
					case 0xE0:
						if (a < 0xA0) return false;
						break;
					case 0xED:
						if (a > 0x9F) return false;
						break;
					case 0xF0:
						if (a < 0x90) return false;
						break;
					case 0xF4:
						if (a > 0x8F) return false;
						break;
					default:
						if (a < 0x80) return false;
				}
				[[fallthrough]];

			case 1:
				if (static_cast<uint8_t>(*source) >= 0x80 &&
				    static_cast<uint8_t>(*source) < 0xC2)
					return false;
		}
		if (static_cast<uint8_t>(*source) > 0xF4) return false;
		return true;
	}

	using utf8_it = std::u8string_view::const_iterator;
	static inline char32_t decode(utf8_it& source,
	                              utf8_it sourceEnd,
	                              bool& ok) {
		static constexpr const uint8_t trailingBytesForUTF8[256] = {
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 00
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 10
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 20
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 30
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 40
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 50
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 60
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 70
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 80
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 90
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // A0
		    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // B0
		    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // C0
		    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // D0
		    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // E0
		    3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5   // F0
		};

		static constexpr const char32_t offsetsFromUTF8[6] = {
		    0x00000000UL, 0x00003080UL, 0x000E2080UL,
		    0x03C82080UL, 0xFA082080UL, 0x82082080UL};

		ok = false;
		auto const uchar = static_cast<uint8_t>(*source);
		auto extraBytesToRead = trailingBytesForUTF8[uchar];
		if (extraBytesToRead >= sourceEnd - source) return 0;

		if (!isLegalUTF8(source, extraBytesToRead + 1)) return 0;

		char32_t ch = 0;
		/*
		 * The cases all fall through.
		 */
		switch (extraBytesToRead) {
			// 4 and 5 extra bytes are not passed through by isLegalUTF8
			case 3:
				ch += static_cast<uint8_t>(*source++);
				ch <<= 6;
				[[fallthrough]];
			case 2:
				ch += static_cast<uint8_t>(*source++);
				ch <<= 6;
				[[fallthrough]];
			case 1:
				ch += static_cast<uint8_t>(*source++);
				ch <<= 6;
				[[fallthrough]];
			case 0:
				ch += static_cast<uint8_t>(*source++);
		}
		ch -= offsetsFromUTF8[extraBytesToRead];
		ok = true;
		return ch;
	}

	friend std::ostream& operator<<(std::ostream& out,
	                                cxx_u8string const& data) {
		if (data.val.empty()) return out << "u8\"\"";
		out << "u8\"";
		for (auto c : convert<std::u32string>(data.val)) {
			auto const uc = static_cast<uint32_t>(c);
			switch (c) {
				case '\\':
					out << "\\\\";
					break;
				case '\"':
					out << "\\\"";
					break;
				case '\a':
					out << "\\a";
					break;
				case '\b':
					out << "\\b";
					break;
				case '\f':
					out << "\\f";
					break;
				case '\n':
					out << "\\n";
					break;
				case '\r':
					out << "\\r";
					break;
				case '\t':
					out << "\\t";
					break;
				case '\v':
					out << "\\v";
					break;
				default:
					if (c <= 0xFF) {
						if (std::isprint(uc)) {
							out << static_cast<char>(c);
						} else {
							out << "\\" << std::oct << std::setw(3)
							    << std::setfill('0')
							    << static_cast<unsigned>(uc) << std::dec;
						}
					} else if (c <= 0xFFFF) {
						out << "\\u" << std::hex << std::setw(4)
						    << std::setfill('0') << static_cast<unsigned>(uc)
						    << std::dec;
					} else {
						out << "\\U" << std::hex << std::setw(8)
						    << std::setfill('0') << static_cast<unsigned>(uc)
						    << std::dec;
					}
					break;
			}
		}
		return out << "\"";
	}
};

namespace json::testing {
	struct node_testcase {
		std::string_view name{};
		json::string_view text{};
		json::node node{};

		friend std::ostream& operator<<(std::ostream& out,
		                                node_testcase const& tc) {
			return out << tc.name << ", " << cxx_u8string{tc.text};
		}
	};

	inline static std::ostream& write_json(std::ostream& output,
	                                       node const& value) {
		class output_impl final : public output {
		public:
			output_impl(std::ostream& out) : out_{out} {};

			void write(string_view str) override {
				out_ << std::string_view{
				    reinterpret_cast<char const*>(str.data()), str.size()};
			}
			void write(char c) override { out_ << c; }

		private:
			std::ostream& out_;
		};
		output_impl impl{output};
		json::write_json(impl, value);
		return output;
	}

	inline std::ostream& operator<<(std::ostream& out, node const& value_node) {
		std::visit(
		    [&](auto const& value) {
			    using type =
			        std::remove_cv_t<std::remove_reference_t<decltype(value)>>;
			    if constexpr (std::is_same_v<type, std::monostate>) {
				    out << "node{}";
			    } else if constexpr (std::is_same_v<type, long long>) {
				    out << value << "ll";
			    } else if constexpr (std::is_same_v<type, double>) {
				    auto const sval = std::to_string(value);
				    out << sval;
				    if (sval.find_first_of(".eE") == std::string::npos)
					    out << ".0";
			    } else if constexpr (std::is_same_v<type, bool>) {
				    out << (value ? "true" : "false");
			    } else if constexpr (std::is_same_v<type, std::nullptr_t>) {
				    out << "nullptr";
			    } else if constexpr (std::is_same_v<type, json::string>) {
				    out << cxx_u8string{value};
				    if (value.find(u8'\000') != std::string::npos) out << "s";
			    } else if constexpr (std::is_same_v<type, json::map>) {
				    out << "json::map{";
				    for (auto const& [fld, subvalue] : value) {
					    out << '{' << cxx_u8string{fld} << ',' << subvalue
					        << "},";
				    }
				    out << '}';
			    } else if constexpr (std::is_same_v<type, json::array>) {
				    bool first{true};
				    out << "mk_array(";
				    for (auto const& subvalue : value) {
					    if (first)
						    first = false;
					    else
						    out << ',';
					    out << subvalue;
				    }
				    out << ')';
			    };
		    },
		    value_node.base());
		return out;
	}

	template <typename BaseClass>
	class node_assert_eq : public BaseClass {
	private:
		void assert_eq_(node const& expected_node,
		                node const& actual_node,
		                string const& path) {
			static constexpr char const* types[] = {
			    "monostate", "null", "string", "long long",
			    "double",    "bool", "map",    "array"};

			ASSERT_STREQ(types[expected_node.index()],
			             types[actual_node.index()])
			    << "Path: " << cxx_u8string{path};
			auto test = static_cast<::testing::Test*>(this);
			std::visit(
			    [&](auto const& expected) {
				    using type = std::remove_cv_t<
				        std::remove_reference_t<decltype(expected)>>;
				    auto const& actual = std::get<type>(actual_node);
				    if constexpr (std::is_same_v<type, json::map>) {
					    ASSERT_EQ(expected.size(), actual.size())
					        << "Path: " << cxx_u8string{path};
					    for (auto const& [fld, exp_value] : expected) {
						    auto const subpath = path + fld + u8"/";
						    auto it = actual.find(fld);
						    ASSERT_NE(it, actual.end())
						        << "Path: " << cxx_u8string{path};
						    assert_eq_(exp_value, it->second, subpath);
						    if (test->HasFatalFailure()) return;
					    }
				    } else if constexpr (std::is_same_v<type, json::array>) {
					    ASSERT_EQ(expected.size(), actual.size())
					        << "Path: " << cxx_u8string{path};
					    for (size_t index = 0; index < expected.size();
					         ++index) {
						    auto str = std::to_string(index);
						    auto const subpath =
						        path.substr(path.length() - 1) + u8"[" +
						        string{reinterpret_cast<char8_t const*>(
						                   str.data()),
						               str.length()} +
						        u8"]/";
						    assert_eq_(expected[index], actual[index], subpath);
						    if (test->HasFatalFailure()) return;
					    }
				    } else {
					    ASSERT_EQ(expected, actual)
					        << "Path: " << cxx_u8string{path};
				    };
			    },
			    expected_node.base());
		}

	protected:
		void assert_eq(node const& expected_node, node const& actual_node) {
			assert_eq_(expected_node, actual_node, u8"/");
		}
	};
}  // namespace json::testing
