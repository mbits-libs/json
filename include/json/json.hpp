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
	using string = std::u8string;
	using string_view = std::u8string_view;

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

		node(const char* value)
		    : node(reinterpret_cast<const char8_t*>(value)) {}
		node(const char8_t* value)
		    : base_type(std::in_place_type<string>,
		                reinterpret_cast<const char8_t*>(value)) {}

		base_type& base() { return *this; }
		base_type const& base() const { return *this; }
	};

	using map = std::map<string, node>;
	using array = std::vector<node>;

	template <typename Arg, typename... List>
	struct is_one_of : std::bool_constant<(std::is_same_v<Arg, List> || ...)> {
	};

	template <typename Kind>
	concept NodeType = is_one_of<Kind,
	                             std::nullptr_t,
	                             string,
	                             long long,
	                             double,
	                             bool,
	                             map,
	                             array>::value;

	template <NodeType Kind>
	static inline Kind* cast(node& value) {
		if (!std::holds_alternative<Kind>(value)) return nullptr;
		return &std::get<Kind>(value);
	}

	template <NodeType Kind>
	static inline Kind const* cast(node const& value) {
		if (!std::holds_alternative<Kind>(value)) return nullptr;
		return &std::get<Kind>(value);
	}

	template <NodeType Kind>
	static inline Kind* cast(map& value, string const& key) {
		auto it = value.find(key);
		if (it == value.end()) return nullptr;
		return cast<Kind>(it->second);
	}

	template <NodeType Kind>
	static inline Kind const* cast(map const& value, string const& key) {
		auto it = value.find(key);
		if (it == value.end()) return nullptr;
		return cast<Kind>(it->second);
	}

	template <NodeType Kind>
	static inline Kind* cast(node* value) {
		if (!value) return nullptr;
		return cast<Kind>(*value);
	}

	template <NodeType Kind>
	static inline Kind const* cast(node const* value) {
		if (!value) return nullptr;
		return cast<Kind>(*value);
	}

	template <NodeType Kind>
	static inline Kind* cast(map* value, string const& key) {
		if (!value) return nullptr;
		return cast<Kind>(*value, key);
	}

	template <NodeType Kind>
	static inline Kind const* cast(map const* value, string const& key) {
		if (!value) return nullptr;
		return cast<Kind>(*value, key);
	}

	template <NodeType Kind>
	static inline Kind* cast(node& value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	template <NodeType Kind>
	static inline Kind const* cast(node const& value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	template <NodeType Kind>
	static inline Kind* cast(node* value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	template <NodeType Kind>
	static inline Kind const* cast(node const* value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	node* from_json(map& value, string_view path);
	node const* from_json(map const& value, string_view path);

	static inline node* from_json(map* value, string_view path) {
		if (!value) return nullptr;
		return from_json(*value, path);
	}

	static inline node const* from_json(map const* value, string_view path) {
		if (!value) return nullptr;
		return from_json(*value, path);
	}

	static inline node* from_json(node& value, string_view path) {
		return from_json(cast<map>(value), path);
	}

	static inline node const* from_json(node const& value, string_view path) {
		return from_json(cast<map>(value), path);
	}

	static inline node* from_json(node* value, string_view path) {
		return from_json(cast<map>(value), path);
	}

	static inline node const* from_json(node const* value, string_view path) {
		return from_json(cast<map>(value), path);
	}

	template <NodeType Kind>
	static inline Kind* cast_from_json(map& value, string_view const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <NodeType Kind>
	static inline Kind const* cast_from_json(map const& value,
	                                         string_view const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <NodeType Kind>
	static inline Kind* cast_from_json(map* value, string const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <NodeType Kind>
	static inline Kind const* cast_from_json(map const* value,
	                                         string_view const& path) {
		return cast<Kind>(from_json(value, path));
	}

	template <NodeType Kind>
	static inline Kind* cast_from_json(node& value, string_view const& path) {
		auto result = from_json(value, path);
		if (!result) return nullptr;
		return cast<Kind>(*result);
	}

	template <NodeType Kind>
	static inline Kind const* cast_from_json(node const& value,
	                                         string_view const& path) {
		auto result = from_json(value, path);
		if (!result) return nullptr;
		return cast<Kind>(*result);
	}

	template <NodeType Kind>
	static inline Kind* cast_from_json(node* value, string_view const& path) {
		if (!value) return nullptr;
		return cast_from_json<Kind>(*value, path);
	}

	template <NodeType Kind>
	static inline Kind const* cast_from_json(node const* value,
	                                         string_view const& path) {
		if (!value) return nullptr;
		return cast_from_json<Kind>(*value, path);
	}

	enum class read_mode { strict, ECMA };
	node read_json(string_view text,
	               string_view skip = {},
	               read_mode mode = read_mode::strict);

	struct output {
		virtual ~output();
		virtual void write(string_view) = 0;
		virtual void write(char) = 0;
	};

	void write_json(output&, node const&);
	void write_json(FILE*, node const&);
	void write_json(string&, node const&);
}  // namespace json
