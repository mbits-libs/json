// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include "json.hh"

using namespace std::literals;

namespace json::testing {
	template <typename Expected>
	struct nav_testcase {
		std::string_view name;
		json::string_view path;
		node value;
		std::optional<Expected> expected{};

		friend std::ostream& operator<<(std::ostream& out,
		                                nav_testcase const& tc) {
			return out << tc.name << ", " << cxx_u8string{tc.path};
		}
	};

	class from_json
	    : public node_assert_eq<::testing::TestWithParam<nav_testcase<node>>> {
	protected:
		template <typename Value>
		void path(ParamType const& param, Value&& value) {
			auto actual = json::from_json(value, param.path);

			if (!param.expected) {
				ASSERT_EQ(nullptr, actual);
				return;
			}

			ASSERT_NE(nullptr, actual);
			assert_eq(*param.expected, *actual);
			if (HasFatalFailure())
				std::cout << "Expected: " << *param.expected << '\n'
				          << "Actual: " << *actual << '\n';
		}
	};

	TEST_P(from_json, paths_const) {
		auto& param = GetParam();
		path(param, param.value);
	}

	TEST_P(from_json, paths_mutable) {
		auto& param = GetParam();
		json::node copy{param.value};
		path(param, copy);
	}

	TEST_P(from_json, paths_const_map) {
		auto& param = GetParam();
		auto* mapped = cast<map>(param.value);
		if (!mapped) {
			ASSERT_FALSE(!!param.expected);
			return;
		}
		path(param, *mapped);
	}

	TEST_P(from_json, paths_mutable_map) {
		auto& param = GetParam();
		json::node copy{param.value};
		auto* mapped = cast<map>(copy);
		if (!mapped) {
			ASSERT_FALSE(!!param.expected);
			return;
		}
		path(param, *mapped);
	}

	TEST_P(from_json, paths_const_ptr) {
		auto& param = GetParam();
		path(param, &param.value);
	}

	TEST_P(from_json, paths_mutable_ptr) {
		auto& param = GetParam();
		json::node copy{param.value};
		path(param, &copy);
	}

	TEST_P(from_json, paths_const_map_ptr) {
		auto& param = GetParam();
		path(param, cast<map>(param.value));
	}

	TEST_P(from_json, paths_mutable_map_ptr) {
		auto& param = GetParam();
		json::node copy{param.value};
		path(param, cast<map>(copy));
	}

	static const nav_testcase<node> tests[] = {
	    {
	        "zeroth_level"sv,
	        u8"",
	        map{{u8"key", u8"value"}},
	    },
	    {
	        "single_level"sv,
	        u8"key",
	        map{{u8"key", u8"value"}},
	        u8"value",
	    },
	    {
	        "single_level_missing"sv,
	        u8"key2",
	        map{{u8"key", u8"value"}},
	    },
	    {
	        "not_map"sv,
	        u8"key",
	        array{u8"value1", u8"value2"},
	    },
	    {
	        "multi_level"sv,
	        u8"key.sub1.sub2",
	        map{{
	            u8"key",
	            map{{
	                u8"sub1",
	                map{
	                    {u8"sub2", 3.14},
	                    {u8"unused", 2.71},
	                },
	            }},
	        }},
	        3.14,
	    },
	    {
	        "multi_level_missing"sv,
	        u8"key.sub1.sub2",
	        map{{
	            u8"key",
	            map{{
	                u8"subA",
	                map{
	                    {u8"sub2", 3.14},
	                    {u8"unused", 2.71},
	                },
	            }},
	        }},
	    },
	    {
	        "multi_level_not_map"sv,
	        u8"key.sub1.sub2",
	        map{{
	            u8"key",
	            map{{
	                u8"sub1",
	                array{u8"sub2", 3.14, u8"unused", 2.71},
	            }},
	        }},
	    },
	};

	INSTANTIATE_TEST_SUITE_P(samples, from_json, ::testing::ValuesIn(tests));

	inline string as_str(string_view view) {
		return {view.data(), view.size()};
	}

	class cast_from_json
	    : public node_assert_eq<
	          ::testing::TestWithParam<nav_testcase<json::string>>> {
	protected:
		template <typename Value>
		void path(ParamType const& param, Value&& value) {
			auto actual =
			    json::cast_from_json<string>(value, as_str(param.path));
			if (!param.expected) {
				ASSERT_EQ(nullptr, actual);
				return;
			}

			ASSERT_NE(nullptr, actual);
			ASSERT_EQ(*param.expected, *actual);
		}
	};

	TEST_P(cast_from_json, paths_const) {
		auto& param = GetParam();
		path(param, param.value);
	}

	TEST_P(cast_from_json, paths_mutable) {
		auto& param = GetParam();
		auto copy = param.value;
		path(param, copy);
	}

	TEST_P(cast_from_json, paths_const_ptr) {
		auto& param = GetParam();
		path(param, &param.value);
	}

	TEST_P(cast_from_json, paths_mutable_ptr) {
		auto& param = GetParam();
		auto copy = param.value;
		path(param, &copy);
	}

	TEST_P(cast_from_json, paths_const_map) {
		auto& param = GetParam();
		auto* mapped = cast<map>(param.value);
		if (!mapped) {
			ASSERT_FALSE(!!param.expected);
			return;
		}
		path(param, *mapped);
	}

	TEST_P(cast_from_json, paths_mutable_map) {
		auto& param = GetParam();
		auto copy = param.value;
		auto* mapped = cast<map>(copy);
		if (!mapped) {
			ASSERT_FALSE(!!param.expected);
			return;
		}
		path(param, *mapped);
	}

	TEST_P(cast_from_json, paths_const_map_ptr) {
		auto& param = GetParam();
		path(param, cast<map>(param.value));
	}

	TEST_P(cast_from_json, paths_mutable_map_ptr) {
		auto& param = GetParam();
		auto copy = param.value;
		path(param, cast<map>(copy));
	}

	static const nav_testcase<json::string> strings[] = {
	    {
	        "single_level"sv,
	        u8"key",
	        map{{u8"key", u8"value"}},
	        u8"value",
	    },
	    {
	        "single_level_nonstring"sv,
	        u8"key",
	        map{{u8"key", false}},
	    },
	    {
	        "single_level_missing"sv,
	        u8"key2",
	        map{{u8"key", u8"value"}},
	    },
	    {
	        "multi_level"sv,
	        u8"key.sub1.sub2",
	        map{{
	            u8"key",
	            map{{
	                u8"sub1",
	                map{{u8"sub2", u8"answer"}},
	            }},
	        }},
	        u8"answer",
	    },
	    {
	        "multi_level_nonstring"sv,
	        u8"key.sub1.sub2",
	        map{{
	            u8"key",
	            map{{
	                u8"sub1",
	                map{{u8"sub2", 3.14}},
	            }},
	        }},
	    },
	    {
	        "multi_level_missing"sv,
	        u8"key.sub1.sub2",
	        map{{
	            u8"key",
	            map{{
	                u8"subA",
	                map{
	                    {u8"sub2", 3.14},
	                    {u8"unused", 2.71},
	                },
	            }},
	        }},
	    },
	};

	INSTANTIATE_TEST_SUITE_P(strings,
	                         cast_from_json,
	                         ::testing::ValuesIn(strings));
}  // namespace json::testing
