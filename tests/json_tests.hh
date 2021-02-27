// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include "mstch-json.hh"

namespace mstch::testing {
	node_testcase const github_com_nst_JSONTestSuite[] = {
		{
			"i_number_double_huge_neg_exp"sv,
			"[123.456e-789]"sv,
		},
		{
			"i_number_huge_exp"sv,
			"[0.4e00669999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999969999999006]"sv,
		},
		{
			"i_number_neg_int_huge_exp"sv,
			"[-1e+9999]"sv,
		},
		{
			"i_number_pos_double_huge_exp"sv,
			"[1.5e+9999]"sv,
		},
		{
			"i_number_real_neg_overflow"sv,
			"[-123123e100000]"sv,
		},
		{
			"i_number_real_pos_overflow"sv,
			"[123123e100000]"sv,
		},
		{
			"i_number_real_underflow"sv,
			"[123e-10000000]"sv,
		},
		{
			"i_number_too_big_neg_int"sv,
			"[-123123123123123123123123123123]"sv,
		},
		{
			"i_number_too_big_pos_int"sv,
			"[100000000000000000000]"sv,
		},
		{
			"i_number_very_big_negative_int"sv,
			"[-237462374673276894279832749832423479823246327846]"sv,
		},
		{
			"i_object_key_lone_2nd_surrogate"sv,
			"{\"\\uDFAA\":0}"sv,
			mstch::map{{"\357\277\275",0ll},},
		},
		{
			"i_string_1st_surrogate_but_2nd_missing"sv,
			"[\"\\uDADA\"]"sv,
			mstch::array{"\357\277\275",},
		},
		{
			"i_string_1st_valid_surrogate_2nd_invalid"sv,
			"[\"\\uD888\\u1234\"]"sv,
			mstch::array{"\357\277\275\341\210\264",},
		},
		{
			"i_string_incomplete_surrogates_escape_valid"sv,
			"[\"\\uD800\\uD800\\n\"]"sv,
			mstch::array{"\357\277\275\357\277\275\n",},
		},
		{
			"i_string_incomplete_surrogate_and_escape_valid"sv,
			"[\"\\uD800\\n\"]"sv,
			mstch::array{"\357\277\275\n",},
		},
		{
			"i_string_incomplete_surrogate_pair"sv,
			"[\"\\uDd1ea\"]"sv,
			mstch::array{"\357\277\275a",},
		},
		{
			"i_string_invalid_lonely_surrogate"sv,
			"[\"\\ud800\"]"sv,
			mstch::array{"\357\277\275",},
		},
		{
			"i_string_invalid_surrogate"sv,
			"[\"\\ud800abc\"]"sv,
			mstch::array{"\357\277\275abc",},
		},
		{
			"i_string_invalid_utf-8"sv,
			"[\"\377\"]"sv,
			mstch::array{"\377",},
		},
		{
			"i_string_inverted_surrogates_U+1D11E"sv,
			"[\"\\uDd1e\\uD834\"]"sv,
			mstch::array{"\357\277\275\357\277\275",},
		},
		{
			"i_string_iso_latin_1"sv,
			"[\"\351\"]"sv,
			mstch::array{"\351",},
		},
		{
			"i_string_lone_second_surrogate"sv,
			"[\"\\uDFAA\"]"sv,
			mstch::array{"\357\277\275",},
		},
		{
			"i_string_lone_utf8_continuation_byte"sv,
			"[\"\201\"]"sv,
			mstch::array{"\201",},
		},
		{
			"i_string_not_in_unicode_range"sv,
			"[\"\364\277\277\277\"]"sv,
			mstch::array{"\364\277\277\277",},
		},
		{
			"i_string_overlong_sequence_2_bytes"sv,
			"[\"\300\257\"]"sv,
			mstch::array{"\300\257",},
		},
		{
			"i_string_overlong_sequence_6_bytes"sv,
			"[\"\374\203\277\277\277\277\"]"sv,
			mstch::array{"\374\203\277\277\277\277",},
		},
		{
			"i_string_overlong_sequence_6_bytes_null"sv,
			"[\"\374\200\200\200\200\200\"]"sv,
			mstch::array{"\374\200\200\200\200\200",},
		},
		{
			"i_string_truncated-utf-8"sv,
			"[\"\340\377\"]"sv,
			mstch::array{"\340\377",},
		},
		// i_string_UTF-16LE_with_BOM
		{
			"i_string_UTF-8_invalid_sequence"sv,
			"[\"\346\227\245\321\210\372\"]"sv,
			mstch::array{"\346\227\245\321\210\372",},
		},
		// i_string_utf16BE_no_BOM
		// i_string_utf16LE_no_BOM
		{
			"i_string_UTF8_surrogate_U+D800"sv,
			"[\"\355\240\200\"]"sv,
			mstch::array{"\355\240\200",},
		},
		{
			"i_structure_500_nested_arrays"sv,
			"[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]",
			mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{mstch::array{},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},},
		},
		// i_structure_UTF-8_BOM_empty_object
		{
			"n_array_1_true_without_comma"sv,
			"[1 true]"sv,
		},
		{
			"n_array_a_invalid_utf8"sv,
			"[a\345]"sv,
		},
		{
			"n_array_colon_instead_of_comma"sv,
			"[\"\": 1]"sv,
		},
		{
			"n_array_comma_after_close"sv,
			"[\"\"],"sv,
		},
		{
			"n_array_comma_and_number"sv,
			"[,1]"sv,
		},
		{
			"n_array_double_comma"sv,
			"[1,,2]"sv,
		},
		{
			"n_array_double_extra_comma"sv,
			"[\"x\",,]"sv,
		},
		{
			"n_array_extra_close"sv,
			"[\"x\"]]"sv,
		},
		{
			"n_array_extra_comma"sv,
			"[\"\",]"sv,
			mstch::array{"",},
		},
		{
			"n_array_incomplete"sv,
			"[\"x\""sv,
		},
		{
			"n_array_incomplete_invalid_value"sv,
			"[x"sv,
		},
		{
			"n_array_inner_array_no_comma"sv,
			"[3[4]]"sv,
		},
		{
			"n_array_invalid_utf8"sv,
			"[\377]"sv,
		},
		{
			"n_array_items_separated_by_semicolon"sv,
			"[1:2]"sv,
		},
		{
			"n_array_just_comma"sv,
			"[,]"sv,
		},
		{
			"n_array_just_minus"sv,
			"[-]"sv,
		},
		{
			"n_array_missing_value"sv,
			"[   , \"\"]"sv,
		},
		{
			"n_array_newlines_unclosed"sv,
			"[\"a\",\n4\n,1,"sv,
		},
		{
			"n_array_number_and_comma"sv,
			"[1,]"sv,
			mstch::array{1ll,},
		},
		{
			"n_array_number_and_several_commas"sv,
			"[1,,]"sv,
		},
		{
			"n_array_spaces_vertical_tab_formfeed"sv,
			"[\"\va\"\\f]"sv,
		},
		{
			"n_array_star_inside"sv,
			"[*]"sv,
		},
		{
			"n_array_unclosed"sv,
			"[\"\""sv,
		},
		{
			"n_array_unclosed_trailing_comma"sv,
			"[1,"sv,
		},
		{
			"n_array_unclosed_with_new_lines"sv,
			"[1,\n1\n,1"sv,
		},
		{
			"n_array_unclosed_with_object_inside"sv,
			"[{}"sv,
		},
		{
			"n_incomplete_false"sv,
			"[fals]"sv,
		},
		{
			"n_incomplete_null"sv,
			"[nul]"sv,
		},
		{
			"n_incomplete_true"sv,
			"[tru]"sv,
		},
		{
			"n_multidigit_number_then_00"sv,
			"123\000"sv,
		},
		{
			"n_number_++"sv,
			"[++1234]"sv,
		},
		{
			"n_number_+1"sv,
			"[+1]"sv,
		},
		{
			"n_number_+Inf"sv,
			"[+Inf]"sv,
		},
		{
			"n_number_-01"sv,
			"[-01]"sv,
		},
		{
			"n_number_-1.0."sv,
			"[-1.0.]"sv,
		},
		{
			"n_number_-2."sv,
			"[-2.]"sv,
		},
		{
			"n_number_-NaN"sv,
			"[-NaN]"sv,
		},
		{
			"n_number_.-1"sv,
			"[.-1]"sv,
		},
		{
			"n_number_.2e-3"sv,
			"[.2e-3]"sv,
		},
		{
			"n_number_0.1.2"sv,
			"[0.1.2]"sv,
		},
		{
			"n_number_0.3e+"sv,
			"[0.3e+]"sv,
		},
		{
			"n_number_0.3e"sv,
			"[0.3e]"sv,
		},
		{
			"n_number_0.e1"sv,
			"[0.e1]"sv,
		},
		{
			"n_number_0e+"sv,
			"[0e+]"sv,
		},
		{
			"n_number_0e"sv,
			"[0e]"sv,
		},
		{
			"n_number_0_capital_E+"sv,
			"[0E+]"sv,
		},
		{
			"n_number_0_capital_E"sv,
			"[0E]"sv,
		},
		{
			"n_number_1.0e+"sv,
			"[1.0e+]"sv,
		},
		{
			"n_number_1.0e-"sv,
			"[1.0e-]"sv,
		},
		{
			"n_number_1.0e"sv,
			"[1.0e]"sv,
		},
		{
			"n_number_1eE2"sv,
			"[1eE2]"sv,
		},
		{
			"n_number_1_000"sv,
			"[1 000.0]"sv,
		},
		{
			"n_number_2.e+3"sv,
			"[2.e+3]"sv,
		},
		{
			"n_number_2.e-3"sv,
			"[2.e-3]"sv,
		},
		{
			"n_number_2.e3"sv,
			"[2.e3]"sv,
		},
		{
			"n_number_9.e+"sv,
			"[9.e+]"sv,
		},
		{
			"n_number_expression"sv,
			"[1+2]"sv,
		},
		{
			"n_number_hex_1_digit"sv,
			"[0x1]"sv,
		},
		{
			"n_number_hex_2_digits"sv,
			"[0x42]"sv,
		},
		{
			"n_number_Inf"sv,
			"[Inf]"sv,
		},
		{
			"n_number_infinity"sv,
			"[Infinity]"sv,
		},
		{
			"n_number_invalid+-"sv,
			"[0e+-1]"sv,
		},
		{
			"n_number_invalid-negative-real"sv,
			"[-123.123foo]"sv,
		},
		{
			"n_number_invalid-utf-8-in-bigger-int"sv,
			"[123\345]"sv,
		},
		{
			"n_number_invalid-utf-8-in-exponent"sv,
			"[1e1\345]"sv,
		},
		{
			"n_number_invalid-utf-8-in-int"sv,
			"[0\345]\n"sv,
		},
		{
			"n_number_minus_infinity"sv,
			"[-Infinity]"sv,
		},
		{
			"n_number_minus_sign_with_trailing_garbage"sv,
			"[-foo]"sv,
		},
		{
			"n_number_minus_space_1"sv,
			"[- 1]"sv,
		},
		{
			"n_number_NaN"sv,
			"[NaN]"sv,
		},
		{
			"n_number_neg_int_starting_with_zero"sv,
			"[-012]"sv,
		},
		{
			"n_number_neg_real_without_int_part"sv,
			"[-.123]"sv,
		},
		{
			"n_number_neg_with_garbage_at_end"sv,
			"[-1x]"sv,
		},
		{
			"n_number_real_garbage_after_e"sv,
			"[1ea]"sv,
		},
		{
			"n_number_real_without_fractional_part"sv,
			"[1.]"sv,
		},
		{
			"n_number_real_with_invalid_utf8_after_e"sv,
			"[1e\345]"sv,
		},
		{
			"n_number_starting_with_dot"sv,
			"[.123]"sv,
		},
		{
			"n_number_U+FF11_fullwidth_digit_one"sv,
			"[\357\274\221]"sv,
		},
		{
			"n_number_with_alpha"sv,
			"[1.2a-3]"sv,
		},
		{
			"n_number_with_alpha_char"sv,
			"[1.8011670033376514H-308]"sv,
		},
		{
			"n_number_with_leading_zero"sv,
			"[012]"sv,
		},
		{
			"n_object_bad_value"sv,
			"[\"x\", truth]"sv,
		},
		{
			"n_object_bracket_key"sv,
			"{[: \"x\"}\n"sv,
		},
		{
			"n_object_comma_instead_of_colon"sv,
			"{\"x\", null}"sv,
		},
		{
			"n_object_double_colon"sv,
			"{\"x\"::\"b\"}"sv,
		},
		{
			"n_object_emoji"sv,
			"{\360\237\207\250\360\237\207\255}"sv,
		},
		{
			"n_object_garbage_at_end"sv,
			"{\"a\":\"a\" 123}"sv,
		},
		{
			"n_object_key_with_single_quotes"sv,
			"{key: 'value'}"sv,
		},
		{
			"n_object_lone_continuation_byte_in_key_and_trailing_comma"sv,
			"{\"\271\":\"0\",}"sv,
			mstch::map{{"\271","0"},},
		},
		{
			"n_object_missing_colon"sv,
			"{\"a\" b}"sv,
		},
		{
			"n_object_missing_key"sv,
			"{:\"b\"}"sv,
		},
		{
			"n_object_missing_semicolon"sv,
			"{\"a\" \"b\"}"sv,
		},
		{
			"n_object_missing_value"sv,
			"{\"a\":"sv,
		},
		{
			"n_object_no-colon"sv,
			"{\"a\""sv,
		},
		{
			"n_object_non_string_key"sv,
			"{1:1}"sv,
		},
		{
			"n_object_non_string_key_but_huge_number_instead"sv,
			"{9999E9999:1}"sv,
		},
		{
			"n_object_repeated_null_null"sv,
			"{null:null,null:null}"sv,
		},
		{
			"n_object_several_trailing_commas"sv,
			"{\"id\":0,,,,,}"sv,
		},
		{
			"n_object_single_quote"sv,
			"{'a':0}"sv,
		},
		{
			"n_object_trailing_comma"sv,
			"{\"id\":0,}"sv,
			mstch::map{{"id",0ll},},
		},
		{
			"n_object_trailing_comment"sv,
			"{\"a\":\"b\"}/**/"sv,
		},
		{
			"n_object_trailing_comment_open"sv,
			"{\"a\":\"b\"}/**//"sv,
		},
		{
			"n_object_trailing_comment_slash_open"sv,
			"{\"a\":\"b\"}//"sv,
		},
		{
			"n_object_trailing_comment_slash_open_incomplete"sv,
			"{\"a\":\"b\"}/"sv,
		},
		{
			"n_object_two_commas_in_a_row"sv,
			"{\"a\":\"b\",,\"c\":\"d\"}"sv,
		},
		{
			"n_object_unquoted_key"sv,
			"{a: \"b\"}"sv,
		},
		{
			"n_object_unterminated-value"sv,
			"{\"a\":\"a"sv,
		},
		{
			"n_object_with_single_string"sv,
			"{ \"foo\" : \"bar\", \"a\" }"sv,
		},
		{
			"n_object_with_trailing_garbage"sv,
			"{\"a\":\"b\"}#"sv,
		},
		{
			"n_single_space"sv,
			" "sv,
		},
		{
			"n_string_1_surrogate_then_escape"sv,
			"[\"\\uD800\\\"]"sv,
		},
		{
			"n_string_1_surrogate_then_escape_u"sv,
			"[\"\\uD800\\u\"]"sv,
		},
		{
			"n_string_1_surrogate_then_escape_u1"sv,
			"[\"\\uD800\\u1\"]"sv,
		},
		{
			"n_string_1_surrogate_then_escape_u1x"sv,
			"[\"\\uD800\\u1x\"]"sv,
		},
		{
			"n_string_accentuated_char_no_quotes"sv,
			"[\303\251]"sv,
		},
		{
			"n_string_backslash_00"sv,
			"[\"\\\000\"]"sv,
		},
		{
			"n_string_escaped_backslash_bad"sv,
			"[\"\\\\\\\"]"sv,
		},
		{
			"n_string_escaped_ctrl_char_tab"sv,
			"[\"\\\t\"]"sv,
		},
		{
			"n_string_escaped_emoji"sv,
			"[\"\\\360\237\214\200\"]"sv,
		},
		{
			"n_string_escape_x"sv,
			"[\"\\x00\"]"sv,
		},
		{
			"n_string_incomplete_escape"sv,
			"[\"\\\"]"sv,
		},
		{
			"n_string_incomplete_escaped_character"sv,
			"[\"\\u00A\"]"sv,
		},
		{
			"n_string_incomplete_surrogate"sv,
			"[\"\\uD834\\uDd\"]"sv,
		},
		{
			"n_string_incomplete_surrogate_escape_invalid"sv,
			"[\"\\uD800\\uD800\\x\"]"sv,
		},
		{
			"n_string_invalid-utf-8-in-escape"sv,
			"[\"\\u\345\"]"sv,
		},
		{
			"n_string_invalid_backslash_esc"sv,
			"[\"\\a\"]"sv,
		},
		{
			"n_string_invalid_unicode_escape"sv,
			"[\"\\uqqqq\"]"sv,
		},
		{
			"n_string_invalid_utf8_after_escape"sv,
			"[\"\\\345\"]"sv,
		},
		{
			"n_string_leading_uescaped_thinspace"sv,
			"[\\u0020\"asd\"]"sv,
		},
		{
			"n_string_no_quotes_with_bad_escape"sv,
			"[\\n]"sv,
		},
		{
			"n_string_single_doublequote"sv,
			"\""sv,
		},
		{
			"n_string_single_quote"sv,
			"['single quote']"sv,
		},
		{
			"n_string_single_string_no_double_quotes"sv,
			"abc"sv,
		},
		{
			"n_string_start_escape_unclosed"sv,
			"[\"\\"sv,
		},
		{
			"n_string_unescaped_ctrl_char"sv,
			"[\"a\000a\"]"sv,
		},
		{
			"n_string_unescaped_newline"sv,
			"[\"new\nline\"]"sv,
		},
		{
			"n_string_unescaped_tab"sv,
			"[\"\t\"]"sv,
		},
		{
			"n_string_unicode_CapitalU"sv,
			"\"\\UA66D\""sv,
		},
		{
			"n_string_with_trailing_garbage"sv,
			"\"\"x"sv,
		},
		{
			"n_structure_100000_opening_arrays",
#include "n_structure_100000_opening_arrays.inc"
		},
		{
			"n_structure_angle_bracket_."sv,
			"<.>"sv,
		},
		{
			"n_structure_angle_bracket_null"sv,
			"[<null>]"sv,
		},
		{
			"n_structure_array_trailing_garbage"sv,
			"[1]x"sv,
		},
		{
			"n_structure_array_with_extra_array_close"sv,
			"[1]]"sv,
		},
		{
			"n_structure_array_with_unclosed_string"sv,
			"[\"asd]"sv,
		},
		{
			"n_structure_ascii-unicode-identifier"sv,
			"a\303\245"sv,
		},
		{
			"n_structure_capitalized_True"sv,
			"[True]"sv,
		},
		{
			"n_structure_close_unopened_array"sv,
			"1]"sv,
		},
		{
			"n_structure_comma_instead_of_closing_brace"sv,
			"{\"x\": true,"sv,
		},
		{
			"n_structure_double_array"sv,
			"[][]"sv,
		},
		{
			"n_structure_end_array"sv,
			"]"sv,
		},
		{
			"n_structure_incomplete_UTF8_BOM"sv,
			"\357\273{}"sv,
		},
		{
			"n_structure_lone-invalid-utf-8"sv,
			"\345"sv,
		},
		{
			"n_structure_lone-open-bracket"sv,
			"["sv,
		},
		{
			"n_structure_no_data"sv,
			""sv,
		},
		{
			"n_structure_null-byte-outside-string"sv,
			"[\000]"sv,
		},
		{
			"n_structure_number_with_trailing_garbage"sv,
			"2@"sv,
		},
		{
			"n_structure_object_followed_by_closing_object"sv,
			"{}}"sv,
		},
		{
			"n_structure_object_unclosed_no_value"sv,
			"{\"\":"sv,
		},
		{
			"n_structure_object_with_comment"sv,
			"{\"a\":/*comment*/\"b\"}"sv,
		},
		{
			"n_structure_object_with_trailing_garbage"sv,
			"{\"a\": true} \"x\""sv,
		},
		{
			"n_structure_open_array_apostrophe"sv,
			"['"sv,
		},
		{
			"n_structure_open_array_comma"sv,
			"[,"sv,
		},
		{
			"n_structure_open_array_object",
#include "n_structure_open_array_object.inc"
		},
		{
			"n_structure_open_array_open_object"sv,
			"[{"sv,
		},
		{
			"n_structure_open_array_open_string"sv,
			"[\"a"sv,
		},
		{
			"n_structure_open_array_string"sv,
			"[\"a\""sv,
		},
		{
			"n_structure_open_object"sv,
			"{"sv,
		},
		{
			"n_structure_open_object_close_array"sv,
			"{]"sv,
		},
		{
			"n_structure_open_object_comma"sv,
			"{,"sv,
		},
		{
			"n_structure_open_object_open_array"sv,
			"{["sv,
		},
		{
			"n_structure_open_object_open_string"sv,
			"{\"a"sv,
		},
		{
			"n_structure_open_object_string_with_apostrophes"sv,
			"{'a'"sv,
		},
		{
			"n_structure_open_open"sv,
			"[\"\\{[\"\\{[\"\\{[\"\\{"sv,
		},
		{
			"n_structure_single_eacute"sv,
			"\351"sv,
		},
		{
			"n_structure_single_star"sv,
			"*"sv,
		},
		{
			"n_structure_trailing_#"sv,
			"{\"a\":\"b\"}#{}"sv,
		},
		{
			"n_structure_U+2060_word_joined"sv,
			"[\342\201\240]"sv,
		},
		{
			"n_structure_uescaped_LF_before_string"sv,
			"[\\u000A\"\"]"sv,
		},
		{
			"n_structure_unclosed_array"sv,
			"[1"sv,
		},
		{
			"n_structure_unclosed_array_partial_null"sv,
			"[ false, nul"sv,
		},
		{
			"n_structure_unclosed_array_unfinished_false"sv,
			"[ true, fals"sv,
		},
		{
			"n_structure_unclosed_array_unfinished_true"sv,
			"[ false, tru"sv,
		},
		{
			"n_structure_unclosed_object"sv,
			"{\"asd\":\"asd\""sv,
		},
		{
			"n_structure_unicode-identifier"sv,
			"\303\245"sv,
		},
		{
			"n_structure_UTF8_BOM_no_data"sv,
			"\357\273\277"sv,
		},
		{
			"n_structure_whitespace_formfeed"sv,
			"[\f]"sv,
		},
		{
			"n_structure_whitespace_U+2060_word_joiner"sv,
			"[\342\201\240]"sv,
		},
		{
			"y_array_arraysWithSpaces"sv,
			"[[]   ]"sv,
			mstch::array{mstch::array{},},
		},
		{
			"y_array_empty-string"sv,
			"[\"\"]"sv,
			mstch::array{"",},
		},
		{
			"y_array_empty"sv,
			"[]"sv,
			mstch::array{},
		},
		{
			"y_array_ending_with_newline"sv,
			"[\"a\"]"sv,
			mstch::array{"a",},
		},
		{
			"y_array_false"sv,
			"[false]"sv,
			mstch::array{false,},
		},
		{
			"y_array_heterogeneous"sv,
			"[null, 1, \"1\", {}]"sv,
			mstch::array{nullptr,1ll,"1",mstch::map{},},
		},
		{
			"y_array_null"sv,
			"[null]"sv,
			mstch::array{nullptr,},
		},
		{
			"y_array_with_1_and_newline"sv,
			"[1\n]"sv,
			mstch::array{1ll,},
		},
		{
			"y_array_with_leading_space"sv,
			" [1]"sv,
			mstch::array{1ll,},
		},
		{
			"y_array_with_several_null"sv,
			"[1,null,null,null,2]"sv,
			mstch::array{1ll,nullptr,nullptr,nullptr,2ll,},
		},
		{
			"y_array_with_trailing_space"sv,
			"[2] "sv,
			mstch::array{2ll,},
		},
		{
			"y_number"sv,
			"[123e65]"sv,
			mstch::array{123e65,},
		},
		{
			"y_number_0e+1"sv,
			"[0e+1]"sv,
			mstch::array{0.000000,},
		},
		{
			"y_number_0e1"sv,
			"[0e1]"sv,
			mstch::array{0.000000,},
		},
		{
			"y_number_after_space"sv,
			"[ 4]"sv,
			mstch::array{4ll,},
		},
		{
			"y_number_double_close_to_zero"sv,
			"[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]\n"sv,
			mstch::array{-1e-78,},
		},
		{
			"y_number_int_with_exp"sv,
			"[20e1]"sv,
			mstch::array{200.000000,},
		},
		{
			"y_number_minus_zero"sv,
			"[-0]"sv,
			mstch::array{0ll,},
		},
		{
			"y_number_negative_int"sv,
			"[-123]"sv,
			mstch::array{-123ll,},
		},
		{
			"y_number_negative_one"sv,
			"[-1]"sv,
			mstch::array{-1ll,},
		},
		{
			"y_number_negative_zero"sv,
			"[-0]"sv,
			mstch::array{0ll,},
		},
		{
			"y_number_real_capital_e"sv,
			"[1E22]"sv,
			mstch::array{1E22,},
		},
		{
			"y_number_real_capital_e_neg_exp"sv,
			"[1E-2]"sv,
			mstch::array{1E-2,},
		},
		{
			"y_number_real_capital_e_pos_exp"sv,
			"[1E+2]"sv,
			mstch::array{1E+2,},
		},
		{
			"y_number_real_exponent"sv,
			"[123e45]"sv,
			mstch::array{123e45,},
		},
		{
			"y_number_real_fraction_exponent"sv,
			"[123.456e78]"sv,
			mstch::array{123.456e78,},
		},
		{
			"y_number_real_neg_exp"sv,
			"[1e-2]"sv,
			mstch::array{1e-2,},
		},
		{
			"y_number_real_pos_exponent"sv,
			"[1e+2]"sv,
			mstch::array{1e+2,},
		},
		{
			"y_number_simple_int"sv,
			"[123]"sv,
			mstch::array{123ll,},
		},
		{
			"y_number_simple_real"sv,
			"[123.456789]"sv,
			mstch::array{123.456789,},
		},
		{
			"y_object"sv,
			"{\"asd\":\"sdf\", \"dfg\":\"fgh\"}"sv,
			mstch::map{{"asd","sdf"},{"dfg","fgh"},},
		},
		{
			"y_object_basic"sv,
			"{\"asd\":\"sdf\"}"sv,
			mstch::map{{"asd","sdf"},},
		},
		{
			"y_object_duplicated_key"sv,
			"{\"a\":\"b\",\"a\":\"c\"}"sv,
			mstch::map{{"a","c"},},
		},
		{
			"y_object_duplicated_key_and_value"sv,
			"{\"a\":\"b\",\"a\":\"b\"}"sv,
			mstch::map{{"a","b"},},
		},
		{
			"y_object_empty"sv,
			"{}"sv,
			mstch::map{},
		},
		{
			"y_object_empty_key"sv,
			"{\"\":0}"sv,
			mstch::map{{"",0ll},},
		},
		{
			"y_object_escaped_null_in_key"sv,
			"{\"foo\\u0000bar\": 42}"sv,
			mstch::map{{"foo\000bar"s,42ll},},
		},
		{
			"y_object_extreme_numbers"sv,
			"{ \"min\": -1.0e+28, \"max\": 1.0e+28 }"sv,
			mstch::map{{"max",9999999999999999583119736832.000000},{"min",-9999999999999999583119736832.000000},},
		},
		{
			"y_object_long_strings"sv,
			"{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}"sv,
			mstch::map{
				{"id","xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
				{"x",mstch::array{
					mstch::map{
						{"id","xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
					},
				}},
			},
		},
		{
			"y_object_simple"sv,
			"{\"a\":[]}"sv,
			mstch::map{{"a",mstch::array{}},},
		},
		{
			"y_object_string_unicode"sv,
			"{\"title\":\"\\u041f\\u043e\\u043b\\u0442\\u043e\\u0440\\u0430 \\u0417\\u0435\\u043c\\u043b\\u0435\\u043a\\u043e\\u043f\\u0430\" }"sv,
			mstch::map{{"title","\320\237\320\276\320\273\321\202\320\276\321\200\320\260 \320\227\320\265\320\274\320\273\320\265\320\272\320\276\320\277\320\260"},},
		},
		{
			"y_object_with_newlines"sv,
			"{\n\"a\": \"b\"\n}"sv,
			mstch::map{{"a","b"},},
		},
		{
			"y_string_1_2_3_bytes_UTF-8_sequences"sv,
			"[\"\\u0060\\u012a\\u12AB\"]"sv,
			mstch::array{"`\304\252\341\212\253",},
		},
		{
			"y_string_accepted_surrogate_pair"sv,
			"[\"\\uD801\\udc37\"]"sv,
			mstch::array{"\357\277\275\357\277\275",},
		},
		{
			"y_string_accepted_surrogate_pairs"sv,
			"[\"\\ud83d\\ude39\\ud83d\\udc8d\"]"sv,
			mstch::array{"\357\277\275\357\277\275\357\277\275\357\277\275",},
		},
		{
			"y_string_allowed_escapes"sv,
			"[\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"]"sv,
			mstch::array{"\"\\/\b\f\n\r\t",},
		},
		{
			"y_string_backslash_and_u_escaped_zero"sv,
			"[\"\\\\u0000\"]"sv,
			mstch::array{"\\u0000",},
		},
		{
			"y_string_backslash_doublequotes"sv,
			"[\"\\\"\"]"sv,
			mstch::array{"\"",},
		},
		{
			"y_string_comments"sv,
			"[\"a/*b*/c/*d//e\"]"sv,
			mstch::array{"a/*b*/c/*d//e",},
		},
		{
			"y_string_double_escape_a"sv,
			"[\"\\\\a\"]"sv,
			mstch::array{"\\a",},
		},
		{
			"y_string_double_escape_n"sv,
			"[\"\\\\n\"]"sv,
			mstch::array{"\\n",},
		},
		{
			"y_string_escaped_control_character"sv,
			"[\"\\u0012\"]"sv,
			mstch::array{"\022",},
		},
		{
			"y_string_escaped_noncharacter"sv,
			"[\"\\uFFFF\"]"sv,
			mstch::array{"\357\277\277",},
		},
		{
			"y_string_in_array"sv,
			"[\"asd\"]"sv,
			mstch::array{"asd",},
		},
		{
			"y_string_in_array_with_leading_space"sv,
			"[ \"asd\"]"sv,
			mstch::array{"asd",},
		},
		{
			"y_string_last_surrogates_1_and_2"sv,
			"[\"\\uDBFF\\uDFFF\"]"sv,
			mstch::array{"\357\277\275\357\277\275",},
		},
		{
			"y_string_nbsp_uescaped"sv,
			"[\"new\\u00A0line\"]"sv,
			mstch::array{"new\302\240line",},
		},
		{
			"y_string_nonCharacterInUTF-8_U+10FFFF"sv,
			"[\"\364\217\277\277\"]"sv,
			mstch::array{"\364\217\277\277",},
		},
		{
			"y_string_nonCharacterInUTF-8_U+FFFF"sv,
			"[\"\357\277\277\"]"sv,
			mstch::array{"\357\277\277",},
		},
		{
			"y_string_null_escape"sv,
			"[\"\\u0000\"]"sv,
			mstch::array{"\000"s,},
		},
		{
			"y_string_one-byte-utf-8"sv,
			"[\"\\u002c\"]"sv,
			mstch::array{",",},
		},
		{
			"y_string_pi"sv,
			"[\"\317\200\"]"sv,
			mstch::array{"\317\200",},
		},
		{
			"y_string_reservedCharacterInUTF-8_U+1BFFF"sv,
			"[\"\360\233\277\277\"]"sv,
			mstch::array{"\360\233\277\277",},
		},
		{
			"y_string_simple_ascii"sv,
			"[\"asd \"]"sv,
			mstch::array{"asd ",},
		},
		{
			"y_string_space"sv,
			"\" \""sv,
			" ",
		},
		{
			"y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF"sv,
			"[\"\\uD834\\uDd1e\"]"sv,
			mstch::array{"\357\277\275\357\277\275",},
		},
		{
			"y_string_three-byte-utf-8"sv,
			"[\"\\u0821\"]"sv,
			mstch::array{"\340\240\241",},
		},
		{
			"y_string_two-byte-utf-8"sv,
			"[\"\\u0123\"]"sv,
			mstch::array{"\304\243",},
		},
		{
			"y_string_u+2028_line_sep"sv,
			"[\"\342\200\250\"]"sv,
			mstch::array{"\342\200\250",},
		},
		{
			"y_string_u+2029_par_sep"sv,
			"[\"\342\200\251\"]"sv,
			mstch::array{"\342\200\251",},
		},
		{
			"y_string_uEscape"sv,
			"[\"\\u0061\\u30af\\u30EA\\u30b9\"]"sv,
			mstch::array{"a\343\202\257\343\203\252\343\202\271",},
		},
		{
			"y_string_uescaped_newline"sv,
			"[\"new\\u000Aline\"]"sv,
			mstch::array{"new\nline",},
		},
		{
			"y_string_unescaped_char_delete"sv,
			"[\"\177\"]"sv,
			mstch::array{"\177",},
		},
		{
			"y_string_unicode"sv,
			"[\"\\uA66D\"]"sv,
			mstch::array{"\352\231\255",},
		},
		{
			"y_string_unicodeEscapedBackslash"sv,
			"[\"\\u005C\"]"sv,
			mstch::array{"\\",},
		},
		{
			"y_string_unicode_2"sv,
			"[\"\342\215\202\343\210\264\342\215\202\"]"sv,
			mstch::array{"\342\215\202\343\210\264\342\215\202",},
		},
		{
			"y_string_unicode_escaped_double_quote"sv,
			"[\"\\u0022\"]"sv,
			mstch::array{"\"",},
		},
		{
			"y_string_unicode_U+10FFFE_nonchar"sv,
			"[\"\\uDBFF\\uDFFE\"]"sv,
			mstch::array{"\357\277\275\357\277\275",},
		},
		{
			"y_string_unicode_U+1FFFE_nonchar"sv,
			"[\"\\uD83F\\uDFFE\"]"sv,
			mstch::array{"\357\277\275\357\277\275",},
		},
		{
			"y_string_unicode_U+200B_ZERO_WIDTH_SPACE"sv,
			"[\"\\u200B\"]"sv,
			mstch::array{"\342\200\213",},
		},
		{
			"y_string_unicode_U+2064_invisible_plus"sv,
			"[\"\\u2064\"]"sv,
			mstch::array{"\342\201\244",},
		},
		{
			"y_string_unicode_U+FDD0_nonchar"sv,
			"[\"\\uFDD0\"]"sv,
			mstch::array{"\357\267\220",},
		},
		{
			"y_string_unicode_U+FFFE_nonchar"sv,
			"[\"\\uFFFE\"]"sv,
			mstch::array{"\357\277\276",},
		},
		{
			"y_string_utf8"sv,
			"[\"\342\202\254\360\235\204\236\"]"sv,
			mstch::array{"\342\202\254\360\235\204\236",},
		},
		{
			"y_string_with_del_character"sv,
			"[\"a\177a\"]"sv,
			mstch::array{"a\177a",},
		},
		{
			"y_structure_lonely_false"sv,
			"false"sv,
			false,
		},
		{
			"y_structure_lonely_int"sv,
			"42"sv,
			42ll,
		},
		{
			"y_structure_lonely_negative_real"sv,
			"-0.1"sv,
			-0.100000,
		},
		{
			"y_structure_lonely_null"sv,
			"null"sv,
			nullptr,
		},
		{
			"y_structure_lonely_string"sv,
			"\"asd\""sv,
			"asd",
		},
		{
			"y_structure_lonely_true"sv,
			"true"sv,
			true,
		},
		{
			"y_structure_string_empty"sv,
			"\"\""sv,
			"",
		},
		{
			"y_structure_trailing_newline"sv,
			"[\"a\"]\n"sv,
			mstch::array{"a",},
		},
		{
			"y_structure_true_in_array"sv,
			"[true]"sv,
			mstch::array{true,},
		},
		{
			"y_structure_whitespace_array"sv,
			" [] "sv,
			mstch::array{},
		},
	};
}  // namespace mstch::testing
#pragma once
