// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once
#include <optional>
#include <json/json.hpp>

using namespace std::literals;

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
	concept LoadsJsonRaw = requires(T& lval,
	                                node const& data,
	                                std::string& dbg) {
		{ lval.from_json(data, dbg) } -> std::convertible_to<conv_result>;
	};

	template <typename T>
	concept LoadsJson = requires(T& lval, map const& data, std::string& dbg) {
		{ lval.from_json(data, dbg) } -> std::convertible_to<conv_result>;
		requires !LoadsJsonRaw<T>;
	};

	template <typename T, typename... Kind>
	concept LoadsJsonEx = requires(T& lval,
	                               Kind const*... data,
	                               std::string& dbg) {
		(NodeType<Kind> && ...);
		{ lval.from_json(data..., dbg) } -> std::convertible_to<conv_result>;
	};

	inline auto to_json(StoresJson auto const& obj) { return obj.to_json(); }
	inline long long to_json(std::integral auto val) {
		return static_cast<long long>(val);
	}
	inline long long to_json(std::chrono::sys_seconds const& val) {
		return static_cast<long long>(val.time_since_epoch().count());
	}
	inline string const& to_json(string const& val) { return val; }

	inline string to_json(string_view val) { return {val.data(), val.size()}; }

	inline string to_json(std::string const& val) {
		return {reinterpret_cast<char8_t const*>(val.data()), val.size()};
	}

	inline string to_json(std::string_view val) {
		return {reinterpret_cast<char8_t const*>(val.data()), val.size()};
	}

	inline conv_result load(node const& src,
	                        LoadsJson auto& obj,
	                        std::string& dbg) {
		auto const data = cast<map>(src);
		if (!data) return conv_result::opt;
		return obj.from_json(*data, dbg);
	}

	inline conv_result load(node const& src,
	                        LoadsJsonRaw auto& obj,
	                        std::string& dbg) {
		return obj.from_json(src, dbg);
	}

	template <NodeType... Kind>
	inline conv_result load_multi(node const& src,
	                              LoadsJsonEx<Kind...> auto& obj,
	                              std::string& dbg) {
		return obj.from_json(cast<Kind>(src)..., dbg);
	}

	template <std::integral Int>
	inline conv_result load(node const& src, Int& val, std::string& dbg) {
		auto const data = cast<long long>(src);
		if (!data) return conv_result::opt;
		if (!*data) {
			dbg.append("\n- Loading a zero numeric value"sv);
			return conv_result::updated;
		}
		val = static_cast<Int>(*data);
		return conv_result::ok;
	}

	template <std::integral Int>
	inline conv_result load_zero(node const& src, Int& val, std::string&) {
		auto const data = cast<long long>(src);
		if (!data) return conv_result::opt;
		val = static_cast<Int>(*data);
		return conv_result::ok;
	}

	inline conv_result load(node const& src, bool& val, std::string&) {
		auto const data = cast<bool>(src);
		val = data ? *data : false;
		return conv_result::ok;
	}

	inline conv_result load(node const& src,
	                        std::chrono::sys_seconds& val,
	                        std::string& dbg) {
		auto const data = cast<long long>(src);
		if (!data) return conv_result::opt;
		if (!*data) {
			dbg.append("\n- Loading a zero time value"sv);
			return conv_result::updated;
		}
		val = std::chrono::sys_seconds{std::chrono::seconds{*data}};
		return conv_result::ok;
	}

	inline conv_result load_zero(node const& src,
	                             std::chrono::sys_seconds& val,
	                             std::string&) {
		auto const data = cast<long long>(src);
		if (!data) return conv_result::opt;
		val = std::chrono::sys_seconds{std::chrono::seconds{*data}};
		return conv_result::ok;
	}

	inline conv_result load(node const& src, string& val, std::string& dbg) {
		auto const data = cast<string>(src);
		if (!data) return conv_result::opt;
		if (data->empty()) {
			dbg.append("\n- Loading an empty string value"sv);
			return conv_result::updated;
		}
		val = *data;
		return conv_result::ok;
	}

	inline conv_result load(node const& src,
	                        std::string& val,
	                        std::string& dbg) {
		auto const data = cast<string>(src);
		if (!data) return conv_result::opt;
		if (data->empty()) {
			dbg.append("\n- Loading an empty string value"sv);
			return conv_result::updated;
		}
		val.assign(reinterpret_cast<char const*>(data->data()), data->size());
		return conv_result::ok;
	}

	template <typename T>
	concept LoadableJsonValue = LoadsJson<T> || LoadsJsonRaw<T> ||
	    std::integral<T> || std::same_as<T, std::chrono::sys_seconds> ||
	    std::same_as<T, string> || std::same_as<T, std::string>;

	inline void append_name(conv_result res,
	                        string const& key,
	                        std::string& dbg) {
		if (res != conv_result::updated) return;
		dbg.append(" ("sv);
		dbg.append(reinterpret_cast<char const*>(key.data()), key.size());
		dbg.push_back(')');
	}

	template <LoadableJsonValue Payload>
	inline conv_result load(map const& src,
	                        string const& key,
	                        std::optional<Payload>& value,
	                        std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		value = Payload{};
		auto res = json::load(it->second, *value, dbg);
		if (res == conv_result::failed || res == conv_result::opt) {
			value = std::nullopt;
		}
		append_name(res, key, dbg);
		return res;
	}

	template <LoadableJsonValue Payload>
	inline conv_result load_zero(map const& src,
	                             string const& key,
	                             std::optional<Payload>& value,
	                             std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		value = Payload{};
		auto res = load(it->second, *value, dbg);
		if (res == conv_result::failed || res == conv_result::opt) {
			value = std::nullopt;
		}
		append_name(res, key, dbg);
		return res;
	}

	inline conv_result load(map const& src,
	                        string const& key,
	                        LoadableJsonValue auto& value,
	                        std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto res = json::load(it->second, value, dbg);
		append_name(res, key, dbg);
		return res;
	}

	inline conv_result load_zero(map const& src,
	                             string const& key,
	                             LoadableJsonValue auto& value,
	                             std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto res = json::load_zero(it->second, value, dbg);
		append_name(res, key, dbg);
		return res;
	}

	template <NodeType... Kind, LoadsJsonEx<Kind...> T>
	inline conv_result load_multi(map const& src,
	                              string const& key,
	                              T& value,
	                              std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto res = json::load_multi<Kind...>(it->second, value, dbg);
		append_name(res, key, dbg);
		return res;
	}

	template <LoadableJsonValue ValueType>
	inline conv_result load(array const& src,
	                        std::vector<ValueType>& value,
	                        std::string& dbg) {
		auto result = conv_result::ok;
		value.clear();
		value.reserve(src.size());
		for (auto& node_item : src) {
			value.push_back({});
			auto const res = json::load(node_item, value.back(), dbg);
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
	                        std::vector<ValueType>& value,
	                        std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto const arr = cast<array>(it->second);
		if (!arr) return conv_result::opt;
		auto res = json::load(*arr, value, dbg);
		append_name(res, key, dbg);
		return res;
	}

	template <LoadableJsonValue ValueType>
	inline conv_result load_or_value(map const& src,
	                                 string const& key,
	                                 std::vector<ValueType>& value,
	                                 std::string& dbg) {
		auto it = src.find(key);
		if (it == src.end()) return conv_result::opt;
		auto const arr = cast<array>(it->second);
		if (arr) return json::load(*arr, value, dbg);
		value.push_back({});
		auto const res = load(it->second, value.back(), dbg);
		if (res == conv_result::failed || res == conv_result::opt)
			value.pop_back();
		append_name(res, key, dbg);
		if (res != conv_result::opt) return res;
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
		if (value) json::store(dst, key, *value);
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
			json::store(result, subkey, value);
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
#define LOAD(value) LOAD_EX(::json::load(data, NAMED(value), dbg))
#define LOAD_OR_VALUE(value) \
	LOAD_EX(::json::load_or_value(data, NAMED(value), dbg))
#define LOAD_MULTI(value, ...) \
	LOAD_EX((::json::load_multi<__VA_ARGS__>(data, NAMED(value), dbg)))
