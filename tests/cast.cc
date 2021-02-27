// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <mstch/mstch-json.hpp>
#include <gtest/gtest.h>

using namespace std::literals;

namespace mstch::testing {
	TEST(cast, mutable_map) {
		mstch::map value{{"key", "value"}};
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_map) {
		mstch::map orig{{"key", "value"}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}
	TEST(cast, mutable_map_node) {
		mstch::node value{map{{"key", "value"}}};
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_map_node) {
		mstch::node orig{map{{"key", "value"}}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, mutable_map_node_ptr) {
		mstch::node orig{map{{"key", "value"}}};
		auto* value = &orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_map_node_ptr) {
		mstch::node orig{map{{"key", "value"}}};
		auto const* value = &orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, mutable_nonmap_node) {
		mstch::node value{array{"key", "value"}};
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_EQ(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_nonmap_node) {
		mstch::node orig{array{"key", "value"}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_EQ(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, cast_from_null) {
		mstch::node* nil = nullptr;
		auto actual = mstch::cast_from_json<mstch::map>(nil, "");
		ASSERT_EQ(nullptr, actual);
	}

	TEST(cast, cast_from_const_null) {
		mstch::node const* nil = nullptr;
		auto actual = mstch::cast_from_json<mstch::map>(nil, "");
		ASSERT_EQ(nullptr, actual);
	}
}  // namespace mstch::testing
