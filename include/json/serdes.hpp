// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <optional>
#include <json/json.hpp>

namespace json {
	enum class conv_result { ok, opt, failed, updated };
	inline constexpr bool is_ok(conv_result res) noexcept {
		return res < conv_result::failed;
	}

	template <typename T>
	concept StoresJson = requires(T const& obj) {
		{ obj.to_json() } -> std::convertible_to<node>;
	};

	template <typename T>
	concept LoadsJsonRaw = requires(T& lval, node const& data) {
		{ lval.from_json(data) } -> std::convertible_to<conv_result>;
	};

	template <typename T>
	concept LoadsJson = requires(T& lval, map const& data) {
		{ lval.from_json(data) } -> std::convertible_to<conv_result>;
		requires !LoadsJsonRaw<T>;
	};

	template <typename T, typename... Kind>
	concept LoadsJsonEx = requires(T& lval, Kind const*... data) {
		(NodeType<Kind> && ...);
		{ lval.from_json(data...) } -> std::convertible_to<conv_result>;
	};

	inline auto to_json(StoresJson auto const& obj) { return obj.to_json(); }
	inline long long to_json(std::integral auto val) {
		return static_cast<long long>(val);
	}
	inline long long to_json(std::chrono::sys_seconds const& val) {
		return static_cast<long long>(val.time_since_epoch().count());
	}
	inline string const& to_json(string const& val) { return val; }

	inline conv_result load(node const& src, LoadsJson auto& obj) {
		auto const data = cast<map>(src);
		if (!data) return conv_result::opt;
		return obj.from_json(*data);
	}

	inline conv_result load(node const& src, LoadsJsonRaw auto& obj) {
		return obj.from_json(src);
	}

	template <NodeType... Kind>
	inline conv_result load_multi(node const& src,
	                              LoadsJsonEx<Kind...> auto& obj) {
		return obj.from_json(cast<Kind>(src)...);
	}

	template <std::integral Int>
	inline conv_result load(node const& src, Int& val) {
		auto const data = cast<long long>(src);
		if (!data) return conv_result::opt;
		if (!*data) return conv_result::updated;
		val = static_cast<Int>(*data);
		return conv_result::ok;
	}

	inline conv_result load(node const& src, std::chrono::sys_seconds& val) {
		auto const data = cast<long long>(src);
		if (!data) return conv_result::opt;
		if (!*data) return conv_result::updated;
		val = std::chrono::sys_seconds{std::chrono::seconds{*data}};
		return conv_result::ok;
	}

	inline conv_result load(node const& src, string& val) {
		auto const data = cast<string>(src);
		if (!data) return conv_result::opt;
		if (data->empty()) return conv_result::updated;
		val = *data;
		return conv_result::ok;
	}

	template <typename T>
	concept LoadableJsonValue =
	    LoadsJson<T> || LoadsJsonRaw<T> || std::integral<T> ||
	    std::same_as<T, std::chrono::sys_seconds> || std::same_as<T, string>;

	template <LoadableJsonValue Payload>
	inline conv_result load(map const& src,
	                        string const& key,
	                        std::optional<Payload>& value) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		value = Payload{};
		auto res = load(it->second, *value);
		if (res == conv_result::failed || res == conv_result::opt) {
			value = std::nullopt;
		}
		return res;
	}

	inline conv_result load(map const& src,
	                        string const& key,
	                        LoadableJsonValue auto& value) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		return load(it->second, value);
	}

	template <NodeType... Kind, LoadsJsonEx<Kind...> T>
	inline conv_result load_multi(map const& src, string const& key, T& value) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		return load_multi<Kind...>(it->second, value);
	}

	template <LoadableJsonValue ValueType>
	inline conv_result load(array const& src, std::vector<ValueType>& value) {
		auto result = conv_result::ok;
		value.clear();
		value.reserve(src.size());
		for (auto& node_item : src) {
			value.push_back({});
			auto const res = load(node_item, value.back());
			if (res == conv_result::failed || res == conv_result::opt)
				value.pop_back();
			if (!is_ok(res)) result = res;
			if (result == conv_result::failed) break;
		}
		return result;
	}

	template <LoadableJsonValue ValueType>
	inline conv_result load(map const& src,
	                        string const& key,
	                        std::vector<ValueType>& value) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto const arr = cast<array>(it->second);
		if (!arr) return conv_result::opt;
		return load(*arr, value);
	}

	template <LoadableJsonValue ValueType>
	inline conv_result load_or_value(map const& src,
	                                 string const& key,
	                                 std::vector<ValueType>& value) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto const arr = cast<array>(it->second);
		if (arr) return load(*arr, value);
		value.push_back({});
		auto const res = load(it->second, value.back());
		if (res == conv_result::failed || res == conv_result::opt)
			value.pop_back();
		if (res != conv_result::opt) res;
		return conv_result::ok;
	}

	template <typename T>
	concept JsonStorableValue = requires(T const& obj) {
		{ to_json(obj) } -> std::convertible_to<node>;
	};

	template <typename T>
	concept JsonNodeValue = requires(T const& obj) {
		{ to_json(obj) } -> std::same_as<node>;
	};

	template <JsonStorableValue T>
	inline void store(map& dst, string_view const& key, T const& value) {
		if constexpr (JsonNodeValue<T>) {
			auto v = to_json(value);
			if (!std::holds_alternative<std::monostate>(v)) {
				dst[{key.data(), key.size()}] = std::move(v);
			}
		} else {
			dst[{key.data(), key.size()}] = to_json(value);
		}
	}

	template <JsonStorableValue T>
	inline void store(map& dst,
	                  string_view const& key,
	                  std::optional<T> const& value) {
		if (value) store(dst, key, *value);
	}

	template <JsonStorableValue T>
	inline void store_push(array& dst, T const& value) {
		if constexpr (JsonNodeValue<T>) {
			auto v = to_json(value);
			if (!std::holds_alternative<std::monostate>(v)) {
				dst.push_back(std::move(v));
			}
		} else {
			dst.push_back(to_json(value));
		}
	}

	template <JsonStorableValue T>
	inline void store_push(array& dst, std::optional<T> const& value) {
		if (value) store_push(dst, *value);
	}

	template <JsonStorableValue T>
	inline void store(map& dst,
	                  string_view const& key,
	                  std::vector<T> const& values) {
		if (values.empty()) return;
		json::array result{};
		result.reserve(values.size());
		for (auto const& value : values) {
			store_push(result, value);
		}
		if (result.empty()) return;
		dst[{key.data(), key.size()}] = std::move(result);
	}

	template <JsonStorableValue T>
	inline void store_or_value(map& dst,
	                           string_view const& key,
	                           std::vector<T> const& values) {
		if (values.empty()) return;
		json::array result{};
		result.reserve(values.size());
		for (auto const& value : values) {
			store_push(result, value);
		}
		if (result.empty()) return;
		if (result.size() == 1) {
			dst[{key.data(), key.size()}] = std::move(result.front());
		} else {
			dst[{key.data(), key.size()}] = std::move(result);
		}
	}

	template <JsonStorableValue T>
	inline void store(map& dst,
	                  string_view const& key,
	                  std::map<string, T> const& values) {
		if (values.empty()) return;
		json::map result{};
		for (auto const& [subkey, value] : values) {
			store(result, subkey, value);
		}
		if (result.empty()) return;
		dst[{key.data(), key.size()}] = std::move(result);
	}

}  // namespace json

#define NAMED(value) u8## #value, value
#define NAMED_SV(value) u8## #value##sv, value

#define STORE(value) ::json::store(result, NAMED_SV(value))
#define STORE_OR_VALUE(value) \
	::json::store_or_value(result, u8## #value##sv, value)
#define LOAD_EX(LOADER)                                           \
	do {                                                          \
		auto const ret = LOADER;                                  \
		if (!is_ok(ret)) result = ret;                            \
		if (result == ::json::conv_result::failed) return result; \
	} while (0)
#define LOAD(value) LOAD_EX(::json::load(data, NAMED(value)))
#define LOAD_OR_VALUE(value) LOAD_EX(::json::load_or_value(data, NAMED(value)))
#define LOAD_MULTI(value, ...) \
	LOAD_EX((::json::load_multi<__VA_ARGS__>(data, NAMED(value))))