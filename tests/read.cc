// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include "json_tests.hh"
#include <gtest/gtest.h>

using namespace std::literals;

namespace json::testing {

	class json_read_test
	    : public node_assert_eq<::testing::TestWithParam<node_testcase>> {};

	class js_read_test
	    : public node_assert_eq<::testing::TestWithParam<node_testcase>> {};

	TEST_P(json_read_test, parser) {
		auto& param = GetParam();
		auto actual = json::read_json(param.text);

#if 0
		std::cerr << "{\n  " << cxx_string{param.name} << "sv,\n  "
		          << cxx_u8string{param.text} << "sv,\n";
		if (actual.index()) std::cerr << "  " << actual << ",\n";
		std::cerr << "},\n";
#endif

		assert_eq(param.node, actual);
		if (HasFatalFailure())
			std::cout << "Expected: " << param.node << '\n'
			          << "Actual: " << actual << '\n';
	}

	TEST_P(js_read_test, parser) {
		auto& param = GetParam();
		auto actual =
		    json::read_json(param.text, u8")]}'\n"sv, read_mode::ECMA);

#if 0
		std::cerr << "{\n  " << cxx_string{param.name} << "sv,\n  "
		          << cxx_u8string{param.text} << "sv,\n";
		if (actual.index()) std::cerr << "  " << actual << ",\n";
		std::cerr << "},\n";
#endif

		assert_eq(param.node, actual);
		if (HasFatalFailure())
			std::cout << "Expected: " << param.node << '\n'
			          << "Actual: " << actual << '\n';
	}

	node_testcase const json_additional_values[] = {
	    {
	        "file_with_negative_zero"sv,
	        u8"-0"sv,
	        0ll,
	    },
	    {
	        "illegal_string_continuation_A"sv,
	        u8"\"string with \\\r\n continuation\""sv,
	    },
	    {
	        "illegal_string_continuation_B"sv,
	        u8"\"string with \\\n\r continuation\""sv,
	    },
	    {
	        "illegal_string_vertical_tab"sv,
	        u8"\"string with \\v\""sv,
	    },
	};

	node_testcase const js_values[] = {
	    {
	        "structure_whitespace_formfeed"sv,
	        u8"[\f]"sv,
	        mk_array(),
	    },
	    {
	        "structure_whitespace_formfeed"sv,
	        u8"[\f]"sv,
	        mk_array(),
	    },
	    {
	        "string_escape_slash_n"sv,
	        u8"\"aaa\\\nbbb\""sv,
	        u8"aaabbb",
	    },
	    {
	        "string_escape_slash_nr"sv,
	        u8"\"aaa\\\n\rbbb\""sv,
	        u8"aaabbb",
	    },
	    {
	        "string_escape_slash_n2"sv,
	        u8"\"aaa\\\rbbb\""sv,
	        u8"aaabbb",
	    },
	    {
	        "string_escape_slash_nr2"sv,
	        u8"\"aaa\\\r\nbbb\""sv,
	        u8"aaabbb",
	    },
	    {
	        "string_escapes"sv,
	        u8"\"\\b\\f\\n\\r\\t\\v\\z\""sv,
	        u8"\b\f\n\r\t\vz",
	    },
	    {
	        "string_x_300"sv,
	        u8"\"\\x33\\x30\\x30\""sv,
	        u8"300",
	    },
	    {
	        "string_escaped_ctrl_char_tab"sv,
	        u8"\"\\\t\""sv,
	        u8"\t",
	    },
	    {
	        "string_u_star__shooting_star__invalid"sv,
	        u8"\"\\u{000000002605}:\\u{1f320}:\\u{222222}\""sv,
	        u8"\u2605:\U0001f320:\ufffd",
	    },
	    {
	        "gerrit_with_string_u_star__shooting_star__invalid"sv,
	        u8")]}'\n\"\\u{000000002605}:\\u{1f320}:\\u{222222}\""sv,
	        u8"\u2605:\U0001f320:\ufffd",
	    },
	    {
	        "string_u_overflow"sv,
	        u8"\"\\u{260500260500260500260500260500260500260500260500260500}\""sv,
	    },
	    {
	        "array_of_numbers"sv,
	        u8"[+0b1010101101100110, -0644, 0755, -0o755, 0xcafe, 0XBABE]"sv,
	        mk_array(43878ll, -420ll, 493ll, -493ll, 51966ll, 47806ll),
	    },
	    {
	        "number_zero_garbage"sv,
	        u8"[+0a]"sv,
	    },
	    {
	        "number_oct_overflow"sv,
	        u8"0260500260500260500260500260500260500260500260500260500"sv,
	    },
	    {
	        "number_benders_nightmare"sv,
	        u8"[-0b1010101101200110]"sv,
	    },
	    {
	        "number_benders_nightmare_cubed"sv,
	        u8"0o9"sv,
	    },
	    {
	        "array_of_floats"sv,
	        u8"[0.5, 0e+3, -0E-3]"sv,
	        mk_array(0.500000, 0.000000, -0.000000),
	    },
	    {
	        "undefined_recognized_and_turned_to_null"sv,
	        u8"undefined"sv,
	        nullptr,
	    },
	    {
	        "structure_with_non_string_keys"sv,
	        u8"{+123: undefined, -0xcafe: \"a\", 55.55e-10: false, code: true}"sv,
	        json::map{
	            {u8"-51966", u8"a"},
	            {u8"0.000000", false},
	            {u8"123", nullptr},
	            {u8"code", true},
	        },
	    },
	    {
	        "structure_with_faulty_non_string_keys"sv,
	        u8"{+a: undefined}"sv,
	    },
	    {
	        "structure_with_faulty_non_string_keys_2"sv,
	        u8"{12a: undefined}"sv,
	    },
	    {
	        "structure_with_faulty_non_string_keys_3"sv,
	        u8"{12.a: undefined}"sv,
	    },
	    {
	        "structure_with_faulty_value"sv,
	        u8"{code: unknown}"sv,
	    },
	    {
	        "legal_string_continuation_A"sv,
	        u8"\"string with \\\r\n continuation\""sv,
	        u8"string with  continuation",
	    },
	    {
	        "legal_string_continuation_B"sv,
	        u8"\"string with \\\n\r continuation\""sv,
	        u8"string with  continuation",
	    },
	    {
	        "legal_string_vertical_tab"sv,
	        u8"\"string with \\v\""sv,
	        u8"string with \v",
	    },
	    {
	        "broken_file_string_with_bad_hex_1st"sv,
	        u8"'\\x"sv,
	    },
	    {
	        "broken_file_string_with_bad_hex_2nd"sv,
	        u8"'\\x0"sv,
	    },
	    {
	        "broken_file_string_with_bad_unicode"sv,
	        u8"'\\u"sv,
	    },
	    {
	        "broken_file_string_with_bad_unicode_braces"sv,
	        u8"'\\u{baddigit}"sv,
	    },
	    {
	        "broken_file_bad_number"sv,
	        u8"0x"sv,
	    },
	    {
	        "broken_file_array_with_nonexistent_negative"sv,
	        u8"['there will be number next', -"sv,
	    },
	    {
	        "y_number_neg_real_without_int_part"sv,
	        u8"[-.123]"sv,
	        mk_array(-0.123000),
	    },
	};

	INSTANTIATE_TEST_SUITE_P(JSONTestSuite,
	                         json_read_test,
	                         ::testing::ValuesIn(github_com_nst_JSONTestSuite));
	INSTANTIATE_TEST_SUITE_P(additional,
	                         json_read_test,
	                         ::testing::ValuesIn(json_additional_values));
	INSTANTIATE_TEST_SUITE_P(JavaScript,
	                         js_read_test,
	                         ::testing::ValuesIn(js_values));
}  // namespace json::testing