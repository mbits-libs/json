// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <algorithm>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4455)  // literal suffix identifiers that do not start
                                 // with an underscore are reserved
#endif

using std::literals::operator""sv;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace json {
	using string = std::u8string;
	using string_view = std::u8string_view;

	template <typename Value>
	struct ordered_map {
		ordered_map() = default;
		ordered_map(ordered_map const&) = default;
		ordered_map& operator=(ordered_map const&) = default;
		ordered_map(ordered_map&&) = default;
		ordered_map& operator=(ordered_map&&) = default;

		using tree_map_t = std::map<string, Value>;
		using tree_map_value_t = tree_map_t::value_type;

		ordered_map(std::initializer_list<tree_map_value_t> tree_list)
		    : tree_{tree_list} {
			order_.reserve(tree_list.size());
			for (auto const& [key, _] : tree_list) {
				order_.push_back(key);
			}
		}

		std::map<string, Value> const& items() const noexcept { return tree_; }
		std::span<string const> keys() const noexcept { return order_; }

		bool empty() const noexcept { return tree_.empty(); }
		size_t size() const noexcept { return tree_.size(); }
		auto set(string const& key, Value&& value) {
			auto it = lower_bound(key);
			if (it != end() && it->first == key) {
				it->second = std::move(value);
			} else {
				insert(it, {std::move(key), std::move(value)});
			}
		}

		auto lower_bound(string const& key) { return tree_.lower_bound(key); }
		auto lower_bound(string const& key) const {
			return tree_.lower_bound(key);
		}

		auto find(string const& key) { return tree_.find(key); }
		auto find(string const& key) const { return tree_.find(key); }

		auto erase(string const& key) noexcept {
			auto tree_it = tree_.find(key);
			auto key_it = std::find(order_.begin(), order_.end(), key);
			if (tree_it != tree_.end()) tree_.erase(tree_it);
			if (key_it != order_.end()) order_.erase(key_it);
		}

		auto end() noexcept { return tree_.end(); }
		auto end() const noexcept { return tree_.end(); }

		auto& front() noexcept { return *tree_.begin(); }
		auto const& front() const noexcept { return *tree_.begin(); }

		auto insert(typename std::map<string, Value>::const_iterator hint,
		            typename std::map<string, Value>::value_type&& value) {
			order_.push_back(value.first);
			return tree_.insert(hint, std::move(value));
		}

		auto insert(typename std::map<string, Value>::const_iterator hint,
		            typename std::map<string, Value>::value_type const& value) {
			order_.push_back(value.first);
			return tree_.insert(hint, value);
		}

		auto insert_at_front(
		    typename std::map<string, Value>::const_iterator hint,
		    typename std::map<string, Value>::value_type&& value) {
			order_.insert(order_.begin(), value.first);
			return tree_.insert(hint, std::move(value));
		}

		auto insert_at_front(
		    typename std::map<string, Value>::const_iterator hint,
		    typename std::map<string, Value>::value_type const& value) {
			order_.insert(order_.begin(), value.first);
			return tree_.insert(hint, value);
		}

		auto insert_after(
		    string const& pos,
		    typename std::map<string, Value>::const_iterator hint,
		    typename std::map<string, Value>::value_type&& value) {
			auto it = std::find(order_.begin(), order_.end(), pos);
			order_.insert(it, value.first);
			return tree_.insert(hint, std::move(value));
		}

		auto insert_after(
		    string const& pos,
		    typename std::map<string, Value>::const_iterator hint,
		    typename std::map<string, Value>::value_type const& value) {
			auto it = std::find(order_.begin(), order_.end(), pos);
			order_.insert(it, value.first);
			return tree_.insert(hint, value);
		}

		bool operator==(ordered_map const& other) const noexcept {
			return tree_ == other.tree_;
		}

		auto operator<=>(ordered_map const& other) const noexcept {
			return tree_ <=> other.tree_;
		}

	private:
		tree_map_t tree_{};
		std::vector<string> order_{};
	};

	struct node : std::variant<std::monostate,
	                           std::nullptr_t,
	                           string,
	                           long long,
	                           double,
	                           bool,
	                           ordered_map<node>,
	                           std::vector<node>> {
		using map_type = ordered_map<node>;
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
		    : base_type(std::in_place_type<string>, value) {}

		base_type& base() { return *this; }
		base_type const& base() const { return *this; }
	};

	using map = ordered_map<node>;
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

#if defined(__APPLE__) && defined(__clang__)
#define NODE_TYPE_TMPLT      \
	template <typename Kind> \
	    requires NodeType<Kind>
#else
#define NODE_TYPE_TMPLT template <NodeType Kind>
#endif

	NODE_TYPE_TMPLT
	static inline Kind* cast(node& value) {
		if (!std::holds_alternative<Kind>(value)) return nullptr;
		return &std::get<Kind>(value);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast(node const& value) {
		if (!std::holds_alternative<Kind>(value)) return nullptr;
		return &std::get<Kind>(value);
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast(map& value, string const& key) {
		auto it = value.find(key);
		if (it == value.end()) return nullptr;
		return cast<Kind>(it->second);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast(map const& value, string const& key) {
		auto it = value.find(key);
		if (it == value.end()) return nullptr;
		return cast<Kind>(it->second);
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast(node* value) {
		if (!value) return nullptr;
		return cast<Kind>(*value);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast(node const* value) {
		if (!value) return nullptr;
		return cast<Kind>(*value);
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast(map* value, string const& key) {
		if (!value) return nullptr;
		return cast<Kind>(*value, key);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast(map const* value, string const& key) {
		if (!value) return nullptr;
		return cast<Kind>(*value, key);
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast(node& value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast(node const& value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast(node* value, string const& key) {
		return cast<Kind>(cast<map>(value), key);
	}

	NODE_TYPE_TMPLT
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

	NODE_TYPE_TMPLT
	static inline Kind* cast_from_json(map& value, string_view const& path) {
		return cast<Kind>(from_json(value, path));
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast_from_json(map const& value,
	                                         string_view const& path) {
		return cast<Kind>(from_json(value, path));
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast_from_json(map* value, string const& path) {
		return cast<Kind>(from_json(value, path));
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast_from_json(map const* value,
	                                         string_view const& path) {
		return cast<Kind>(from_json(value, path));
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast_from_json(node& value, string_view const& path) {
		auto result = from_json(value, path);
		if (!result) return nullptr;
		return cast<Kind>(*result);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast_from_json(node const& value,
	                                         string_view const& path) {
		auto result = from_json(value, path);
		if (!result) return nullptr;
		return cast<Kind>(*result);
	}

	NODE_TYPE_TMPLT
	static inline Kind* cast_from_json(node* value, string_view const& path) {
		if (!value) return nullptr;
		return cast_from_json<Kind>(*value, path);
	}

	NODE_TYPE_TMPLT
	static inline Kind const* cast_from_json(node const* value,
	                                         string_view const& path) {
		if (!value) return nullptr;
		return cast_from_json<Kind>(*value, path);
	}

	enum class read_mode { strict, ECMA, serialized };
	node read_json(string_view text,
	               string_view skip = {},
	               read_mode mode = read_mode::strict,
	               size_t* read = nullptr);

	struct output {
		using byte_type = string_view::value_type;
		virtual ~output();
		virtual void write(string_view) = 0;
		virtual void write(byte_type) = 0;
	};

	struct write_config {
		std::variant<std::monostate, int, std::u8string_view> indent{};
		struct separators_type {
			std::u8string_view item{};
			std::u8string_view key{};
			std::u8string_view alt_item{item};
		} separators{};
		bool inline_single_item{};
		size_t horiz_space{80};
		constexpr write_config with_horiz_space(size_t allowed) const noexcept {
			auto copy = *this;
			copy.horiz_space = allowed;
			return copy;
		}
	};

	inline constexpr write_config concise{
	    .indent{},
	    .separators{.item = u8","sv, .key = u8":"sv},
	    .inline_single_item{},
	};

	inline constexpr write_config tab{
	    .indent{u8"\t"sv},
	    .separators{.item = u8","sv, .key = u8": "sv, .alt_item = u8", "sv},
	    .inline_single_item = true,
	};

	inline constexpr write_config two_spaces{
	    .indent{2},
	    .separators{.item = u8","sv, .key = u8": "sv, .alt_item = u8", "sv},
	    .inline_single_item = true,
	};

	inline constexpr write_config four_spaces{
	    .indent{4},
	    .separators{.item = u8","sv, .key = u8": "sv, .alt_item = u8", "sv},
	    .inline_single_item = true,
	};

	void write_json(output&,
	                node const&,
	                write_config const& config = two_spaces);
	void write_json(FILE*,
	                node const&,
	                write_config const& config = two_spaces);
	void write_json(string&,
	                node const&,
	                write_config const& config = two_spaces);
}  // namespace json
