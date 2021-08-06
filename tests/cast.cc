// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <json/json.hpp>

using namespace std::literals;

namespace json::testing {
	TEST(cast, mutable_map) {
		json::map value{{"key", "value"}};
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_map) {
		json::map orig{{"key", "value"}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}
	TEST(cast, mutable_map_node) {
		json::node value{map{{"key", "value"}}};
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_map_node) {
		json::node orig{map{{"key", "value"}}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, mutable_map_node_ptr) {
		json::node orig{map{{"key", "value"}}};
		auto* value = &orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_map_node_ptr) {
		json::node orig{map{{"key", "value"}}};
		auto const* value = &orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_NE(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ("value", *cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, mutable_nonmap_node) {
		json::node value{array{"key", "value"}};
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_EQ(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, const_nonmap_node) {
		json::node orig{array{"key", "value"}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<std::string>(value, "key2"));
		ASSERT_EQ(nullptr, cast<std::string>(value, "key"));
		ASSERT_EQ(nullptr, cast<double>(value, "key"));
	}

	TEST(cast, cast_from_null) {
		json::node* nil = nullptr;
		auto actual = json::cast_from_json<json::map>(nil, "");
		ASSERT_EQ(nullptr, actual);
	}

	TEST(cast, cast_from_const_null) {
		json::node const* nil = nullptr;
		auto actual = json::cast_from_json<json::map>(nil, "");
		ASSERT_EQ(nullptr, actual);
	}
}  // namespace json::testing
