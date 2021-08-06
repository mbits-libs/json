// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <mstch/mstch-json.hpp>
#include <iomanip>
#include <iostream>
#include <gtest/gtest.h>

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

namespace mstch::testing {
	struct node_testcase {
		std::string_view name{};
		std::string_view text{};
		mstch::node node{};

		friend std::ostream& operator<<(std::ostream& out,
		                                node_testcase const& tc) {
			return out << tc.name << ", " << cxx_string{tc.text};
		}
	};

	inline std::ostream& operator<<(std::ostream& out, node const& value_node) {
		std::visit(
		    [&](auto const& value) {
			    using type =
			        std::remove_cv_t<std::remove_reference_t<decltype(value)>>;
			    if constexpr (std::is_same_v<type, std::monostate>) {
				    out << "node{}";
			    } else if constexpr (std::is_same_v<type, mstch::object> ||
			                         std::is_same_v<type, mstch::callback> ||
			                         std::is_same_v<type, mstch::lambda>) {
				    out << "???";
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
			    } else if constexpr (std::is_same_v<type, std::string>) {
				    out << cxx_string{value};
				    if (value.find('\000') != std::string::npos) out << "s";
			    } else if constexpr (std::is_same_v<type, mstch::map>) {
				    out << "mstch::map{";
				    for (auto const& [fld, subvalue] : value) {
					    out << '{' << cxx_string{fld} << ',' << subvalue
					        << "},";
				    }
				    out << '}';
			    } else if constexpr (std::is_same_v<type, mstch::array>) {
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
		                std::string const& path) {
			static constexpr char const* types[] = {
			    "monostate", "null", "string", "long long",
			    "double",    "bool", "lambda", "object",
			    "callback",  "map",  "array"};

			ASSERT_STREQ(types[expected_node.index()],
			             types[actual_node.index()])
			    << "Path: " << path;
			auto test = static_cast<::testing::Test*>(this);
			std::visit(
			    [&](auto const& expected) {
				    using type = std::remove_cv_t<
				        std::remove_reference_t<decltype(expected)>>;
				    auto const& actual = std::get<type>(actual_node);
				    if constexpr (std::is_same_v<type, mstch::lambda>) {
				    } else if constexpr (std::is_same_v<type, mstch::map>) {
					    ASSERT_EQ(expected.size(), actual.size())
					        << "Path: " << path;
					    for (auto const& [fld, exp_value] : expected) {
						    auto const subpath = path + fld + "/";
						    auto it = actual.find(fld);
						    ASSERT_NE(it, actual.end()) << "Path: " << path;
						    assert_eq_(exp_value, it->second, subpath);
						    if (test->HasFatalFailure()) return;
					    }
				    } else if constexpr (std::is_same_v<type, mstch::array>) {
					    ASSERT_EQ(expected.size(), actual.size())
					        << "Path: " << path;
					    for (size_t index = 0; index < expected.size();
					         ++index) {
						    auto const subpath =
						        path.substr(path.length() - 1) + "[" +
						        std::to_string(index) + "]/";
						    assert_eq_(expected[index], actual[index], subpath);
						    if (test->HasFatalFailure()) return;
					    }
				    } else {
					    ASSERT_EQ(expected, actual) << "Path: " << path;
				    };
			    },
			    expected_node.base());
		}

	protected:
		void assert_eq(node const& expected_node, node const& actual_node) {
			assert_eq_(expected_node, actual_node, "/");
		}
	};
}  // namespace mstch::testing
