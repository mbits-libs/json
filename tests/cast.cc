// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <gtest/gtest.h>
#include <json/json.hpp>

using namespace std::literals;

namespace json::testing {
	TEST(cast, mutable_map) {
		json::map value{{u8"key", u8"value"}};
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_NE(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(u8"value", *cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, const_map) {
		json::map orig{{u8"key", u8"value"}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_NE(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(u8"value", *cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}
	TEST(cast, mutable_map_node) {
		json::node value{map{{u8"key", u8"value"}}};
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_NE(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(u8"value", *cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, const_map_node) {
		json::node orig{map{{u8"key", u8"value"}}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_NE(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(u8"value", *cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, mutable_map_node_ptr) {
		json::node orig{map{{u8"key", u8"value"}}};
		auto* value = &orig;
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_NE(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(u8"value", *cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, const_map_node_ptr) {
		json::node orig{map{{u8"key", u8"value"}}};
		auto const* value = &orig;
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_NE(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(u8"value", *cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, mutable_nonmap_node) {
		json::node value{array{u8"key", u8"value"}};
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_EQ(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, const_nonmap_node) {
		json::node orig{array{u8"key", u8"value"}};
		auto const& value = orig;
		ASSERT_EQ(nullptr, cast<string>(value, u8"key2"));
		ASSERT_EQ(nullptr, cast<string>(value, u8"key"));
		ASSERT_EQ(nullptr, cast<double>(value, u8"key"));
	}

	TEST(cast, cast_from_null) {
		json::node* nil = nullptr;
		auto actual = json::cast_from_json<json::map>(nil, u8"");
		ASSERT_EQ(nullptr, actual);
	}

	TEST(cast, cast_from_const_null) {
		json::node const* nil = nullptr;
		auto actual = json::cast_from_json<json::map>(nil, u8"");
		ASSERT_EQ(nullptr, actual);
	}
}  // namespace json::testing
