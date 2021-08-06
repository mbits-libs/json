// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace json {
	using string = std::string;
	using string_view = std::string_view;

	struct node : std::variant<std::monostate,
	                           std::nullptr_t,
	                           string,
	                           long long,
	                           double,
	                           bool,
	                           std::map<string, node>,
	                           std::vector<node>> {
		using map_type = std::map<string, node>;
		using vector_type = std::vector<node>;

		using base_type = std::variant<std::monostate,
		                               std::nullptr_t,
		                               string,
		                               long long,
		                               double,
		                               bool,
		                               map_type,
		                               vector_type>;

		using base_type::base_type;

		node() : base_type(std::in_place_type<std::monostate>) {}

		node(const char8_t* value)
		    : node(reinterpret_cast<const char*>(value)) {}
		node(const char* value)
		    : base_type(std::in_place_type<string>,
		                reinterpret_cast<const char*>(value)) {}

		base_type& base() { return *this; }
		base_type const& base() const { return *this; }
	};

	using map = std::map<string, node>;
	using array = std::vector<node>;

	template <typename Kind>
	static inline Kind* cast(node& value) {
		if (!std::holds_alternative<Kind>(value)) return nullptr;
		return &std::get<Kind>(value);
	}

	template <typename Kind>
	static inline Kind const* cast(node const& value) {
		if (!std::holds_alternative<Kind>(value)) return nullptr;
		return &std::get<Kind>(value);
	}

	template <typename Kind>
	static inline Kind* cast(map& value, std::string const& key) {
		auto it = value.find(key);
		if (it == value.end()) return nullptr;
		return cast<Kind>(it->second);
	}

	template <typename Kind>
	static inline Kind const* cast(map const& value, std::string const& key) {
		auto it = value.find(key);
		if (it == value.end()) return nullptr;
		return cast<Kind>(it->second);
	}

	template <typename Kind>
	static inline Kind* cast(node* value) {
		if (!value) return nullptr;
		return cast<Kind>(*value);
	}

	template <typename Kind>
	static inline Kind const* cast(node const* value) {
		if (!value) return nullptr;
		return cast<Kind>(*value);
	}

	template <typename Kind>
	static inline Kind* cast(map* value, std::string const& key) {
		if (!value) return nullptr;
		return cast<Kind>(*value, key);
	}

	template <typename Kind>
	static inline Kind const* cast(map const* value, std::string const& key) {
		if (!value) return nullptr;
		return cast<Kind>(*value, key);
	}

	template <typename Kind>
	static inline Kind* cast(node& value, std::string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	template <typename Kind>
	static inline Kind const* cast(node const& value, std::string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	template <typename Kind>
	static inline Kind* cast(node* value, std::string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	template <typename Kind>
	static inline Kind const* cast(node const* value, std::string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	node* from_json(map& value, std::string_view path);
	node const* from_json(map const& value, std::string_view path);

	static inline node* from_json(map* value, std::string_view path) {
		if (!value) return nullptr;
		return from_json(*value, path);
	}

	static inline node const* from_json(map const* value,
	                                    std::string_view path) {
		if (!value) return nullptr;
		return from_json(*value, path);
	}

	static inline node* from_json(node& value, std::string_view path) {
		return from_json(cast<map>(value), path);
	}

	static inline node const* from_json(node const& value,
	                                    std::string_view path) {
		return from_json(cast<map>(value), path);
	}

	static inline node* from_json(node* value, std::string_view path) {
		return from_json(cast<map>(value), path);
	}

	static inline node const* from_json(node const* value,
	                                    std::string_view path) {
		return from_json(cast<map>(value), path);
	}

	template <typename Kind>
	static inline Kind* cast_from_json(map& value, std::string const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <typename Kind>
	static inline Kind const* cast_from_json(map const& value,
	                                         std::string const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <typename Kind>
	static inline Kind* cast_from_json(map* value, std::string const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <typename Kind>
	static inline Kind const* cast_from_json(map const* value,
	                                         std::string const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <typename Kind>
	static inline Kind* cast_from_json(node& value, std::string const& path) {
		auto result = from_json(value, path);
		if (!result) return nullptr;
		return cast<Kind>(*result);
	}

	template <typename Kind>
	static inline Kind const* cast_from_json(node const& value,
	                                         std::string const& path) {
		auto result = from_json(value, path);
		if (!result) return nullptr;
		return cast<Kind>(*result);
	}

	template <typename Kind>
	static inline Kind* cast_from_json(node* value, std::string const& path) {
		if (!value) return nullptr;
		return cast_from_json<Kind>(*value, path);
	}

	template <typename Kind>
	static inline Kind const* cast_from_json(node const* value,
	                                         std::string const& path) {
		if (!value) return nullptr;
		return cast_from_json<Kind>(*value, path);
	}

	enum class read_mode { strict, ECMA };
	node read_json(std::string_view text,
	               std::string_view skip = {},
	               read_mode mode = read_mode::strict);

	struct output {
		virtual ~output();
		virtual void write(std::string_view) = 0;
		virtual void write(char) = 0;
	};

	void write_json(output&, node const&);
	void write_json(FILE*, node const&);
	void write_json(std::string&, node const&);
}  // namespace json
