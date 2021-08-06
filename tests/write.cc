// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include "mstch-json.hh"
#include "std_capture.hh"

using namespace std::literals;

namespace mstch::testing {
	struct write_testcase {
		std::string_view name{};
		mstch::node node{};

		friend std::ostream& operator<<(std::ostream& out,
		                                write_testcase const& tc) {
			return out << tc.name << ", " << tc.node;
		}
	};

	class write_test
	    : public node_assert_eq<::testing::TestWithParam<write_testcase>> {};

	TEST_F(write_test, map_to_stdout) {
		std_capture out;
		out.lock();
		write_json(stdout, mstch::map{{"key", "value"}});
		out.unlock();
		ASSERT_EQ("{\"key\": \"value\"}", out.str());
	}

	TEST_P(write_test, identity) {
		auto& param = GetParam();
		std::string text;
		write_json(text, param.node);
		auto const actual = read_json(text);

		assert_eq(param.node, actual);
		if (HasFatalFailure())
			std::cout << "Expected: " << param.node << '\n'
			          << "  Actual: " << actual << '\n'
			          << "  String: " << cxx_string{text} << '\n'
			          << "          " << text << '\n';
	}

	static const write_testcase tests[] = {
	    {
	        "undefined_in_output"sv,
	    },
	    {
	        "empty_array_in_array"sv,
	        array{array{}},
	    },
	    {
	        "array"sv,
	        array{0ll, 1ll, 2.5, nullptr, true, false},
	    },
	    {
	        "map"sv,
	        map{
	            {"zero", 0ll},
	            {"integer", 1ll},
	            {"floating", 2.5},
	            {"nullptr", nullptr},
	            {"boolA", true},
	            {"boolB", false},
	            {"map", map{}},
	            {
	                "map2",
	                map{
	                    {"zero", 0ll},
	                    {"integer", 1ll},
	                    {"floating", 2.5},
	                    {"nullptr", nullptr},
	                    {"boolA", true},
	                    {"boolB", false},
	                    {"map", map{}},
	                },
	            },
	        },
	    },
	    {
	        "string_controls"sv,
	        "\001\002\003\004\005\006\007\010\011\012\013\014\015\016\017\020"
	        "\021\022\023\024\025\026\027\030\031\032\033\034\035\036\037",
	    },
	    {
	        "string_embedded_null"sv,
	        "aaa\000bbb"s,
	    },
	    {
	        "string_escaped_quote"sv,
	        "\"title\"",
	    },
	    {
	        "string_many_escapes"sv,
	        "\\\\\\\\\\\"",
	    },
	};

	INSTANTIATE_TEST_SUITE_P(samples, write_test, ::testing::ValuesIn(tests));
}  // namespace mstch::testing