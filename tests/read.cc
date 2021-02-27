// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include "json_tests.hh"
#include <gtest/gtest.h>

using namespace std::literals;

namespace mstch::testing {

	class json_read_test
	    : public node_assert_eq<::testing::TestWithParam<node_testcase>> {};

	class js_read_test
	    : public node_assert_eq<::testing::TestWithParam<node_testcase>> {};

	TEST_P(json_read_test, parser) {
		auto& param = GetParam();
		auto actual = mstch::read_json(param.text);

#if 0
		std::cerr << "{\n  " << cxx_string{param.name} << "sv,\n  "
		          << cxx_string{param.text} << "sv,\n";
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
		auto actual = mstch::read_json(param.text, ")]}'\n"sv, read_mode::ECMA);

#if 0
		std::cerr << "{\n  " << cxx_string{param.name} << "sv,\n  "
		          << cxx_string{param.text} << "sv,\n";
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
	        "-0"sv,
	        0ll,
	    },
	    {
	        "illegal_string_continuation_A"sv,
	        "\"string with \\\r\n continuation\""sv,
	    },
	    {
	        "illegal_string_continuation_B"sv,
	        "\"string with \\\n\r continuation\""sv,
	    },
	    {
	        "illegal_string_vertical_tab"sv,
	        "\"string with \\v\""sv,
	    },
	};

	node_testcase const js_values[] = {
	    {
	        "structure_whitespace_formfeed"sv,
	        "[\f]"sv,
	        mstch::array{},
	    },
	    {
	        "structure_whitespace_formfeed"sv,
	        "[\f]"sv,
	        mstch::array{},
	    },
	    {
	        "string_escape_slash_n"sv,
	        "\"aaa\\\nbbb\""sv,
	        "aaabbb",
	    },
	    {
	        "string_escape_slash_nr"sv,
	        "\"aaa\\\n\rbbb\""sv,
	        "aaabbb",
	    },
	    {
	        "string_escape_slash_n2"sv,
	        "\"aaa\\\rbbb\""sv,
	        "aaabbb",
	    },
	    {
	        "string_escape_slash_nr2"sv,
	        "\"aaa\\\r\nbbb\""sv,
	        "aaabbb",
	    },
	    {
	        "string_escapes"sv,
	        "\"\\b\\f\\n\\r\\t\\v\\z\""sv,
	        "\b\f\n\r\t\vz",
	    },
	    {
	        "string_x_300"sv,
	        "\"\\x33\\x30\\x30\""sv,
	        "300",
	    },
	    {
	        "string_escaped_ctrl_char_tab"sv,
	        "\"\\\t\""sv,
	        "\t",
	    },
	    {
	        "string_u_star__shooting_star__invalid"sv,
	        "\"\\u{000000002605}:\\u{1f320}:\\u{222222}\""sv,
	        "\342\230\205:\xF0\x9F\x8C\xA0:\xEF\xBF\xBD",
	    },
	    {
	        "gerrit_with_string_u_star__shooting_star__invalid"sv,
	        ")]}'\n\"\\u{000000002605}:\\u{1f320}:\\u{222222}\""sv,
	        "\342\230\205:\xF0\x9F\x8C\xA0:\xEF\xBF\xBD",
	    },
	    {
	        "string_u_overflow"sv,
	        "\"\\u{260500260500260500260500260500260500260500260500260500}\""sv,
	    },
	    {
	        "array_of_numbers"sv,
	        "[+0b1010101101100110, -0644, 0755, -0o755, 0xcafe, 0XBABE]"sv,
	        mstch::array{
	            43878ll,
	            -420ll,
	            493ll,
	            -493ll,
	            51966ll,
	            47806ll,
	        },
	    },
	    {
	        "number_zero_garbage"sv,
	        "[+0a]"sv,
	    },
	    {
	        "number_oct_overflow"sv,
	        "0260500260500260500260500260500260500260500260500260500"sv,
	    },
	    {
	        "number_benders_nightmare"sv,
	        "[-0b1010101101200110]"sv,
	    },
	    {
	        "number_benders_nightmare_cubed"sv,
	        "0o9"sv,
	    },
	    {
	        "array_of_floats"sv,
	        "[0.5, 0e+3, -0E-3]"sv,
	        mstch::array{
	            0.500000,
	            0e+3,
	            -0e-3,
	        },
	    },
	    {
	        "undefined_recognized_and_turned_to_null"sv,
	        "undefined"sv,
	        nullptr,
	    },
	    {
	        "structure_with_non_string_keys"sv,
	        "{+123: undefined, -0xcafe: \"a\", 55.55e-10: false, code: true}"sv,
	        mstch::map{
	            {"123", nullptr},
	            {"-51966", "a"},
	            {"0.000000", false},
	            {"code", true},
	        },
	    },
	    {
	        "structure_with_faulty_non_string_keys"sv,
	        "{+a: undefined}"sv,
	    },
	    {
	        "structure_with_faulty_non_string_keys_2"sv,
	        "{12a: undefined}"sv,
	    },
	    {
	        "structure_with_faulty_non_string_keys_3"sv,
	        "{12.a: undefined}"sv,
	    },
	    {
	        "structure_with_faulty_value"sv,
	        "{code: unknown}"sv,
	    },
	    {
	        "legal_string_continuation_A"sv,
	        "\"string with \\\r\n continuation\""sv,
	        "string with  continuation"s,
	    },
	    {
	        "legal_string_continuation_B"sv,
	        "\"string with \\\n\r continuation\""sv,
	        "string with  continuation"s,
	    },
	    {
	        "legal_string_vertical_tab"sv,
	        "\"string with \\v\""sv,
	        "string with \v"s,
	    },
	    {
	        "broken_file_string_with_bad_hex_1st"sv,
	        "'\\x"sv,
	    },
	    {
	        "broken_file_string_with_bad_hex_2nd"sv,
	        "'\\x0"sv,
	    },
	    {
	        "broken_file_string_with_bad_unicode"sv,
	        "'\\u"sv,
	    },
	    {
	        "broken_file_string_with_bad_unicode_braces"sv,
	        "'\\u{baddigit}"sv,
	    },
	    {
	        "broken_file_bad_number"sv,
	        "0x"sv,
	    },
	    {
	        "broken_file_array_with_nonexistent_negative"sv,
	        "['there will be number next', -"sv,
	    },
	    {
	        "y_number_neg_real_without_int_part"sv,
	        "[-.123]"sv,
	        array{-.123},
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
}  // namespace mstch::testing