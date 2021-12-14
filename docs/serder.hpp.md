# Serializer/deserializer

## Synopsis

```cpp
#include <json/serdes.hpp>
```

## json::conv_result

```cpp
enum class json::conv_result {
    ok,
    opt,
    failed,
    updated
};
inline constexpr bool is_ok(conv_result res) noexcept;
```

Enum type returned by `load()` with a helper unifying `ok` and `opt`.

| Enum | Meaning |
| ---- | ------- |
| `ok` | A call to `load()` ended with success |
| `opt` | Like `ok`, but means either the key was not present, or the type was different |
| `failed` | Returned by `load()` if underlying `from_json()` method returns it. Used to indicate unreperable error. |
| `updated` | The value was a default one, which would not be stored, when stored. |


## json::StoresJson&lt;T&gt;

```cpp
template <typename T>
concept StoresJson;
```

A concept accepting types with `to_json()` method, with no arguments and returnig a type, which can be used to intialize a `json::node`.

## json::LoadsJsonRaw&lt;T&gt;

```cpp
template <typename T>
concept LoadsJsonRaw;
```

A concept accepting types with `from_json(json::node const&)` method returnig a `json::conv_result`.

## json::LoadsJson&lt;T&gt;

```cpp
template <typename T>
concept LoadsJson;
```

A concept accepting types with `from_json(json::map const&)` method returnig a `json::conv_result`.

## json::LoadsJsonEx&lt;T, Kind...&gt;

```cpp
template <typename T, typename... Kind>
concept LoadsJsonEx;
```

A concept accepting types with `from_json(Kind const* ...)` method returnig a `json::conv_result`.

## json::LoadableJsonValue&lt;T&gt;

```cpp
template <typename T>
concept LoadableJsonValue =
    LoadsJson<T> || LoadsJsonRaw<T> || std::integral<T> ||
    std::same_as<T, std::chrono::sys_seconds> || std::same_as<T, string>;
```

A concept accepting types with a single-argument `from_json()` method returnig a `json::conv_result`, numbers, timestamps and strings.

## json::to_json

```cpp
auto to_json(StoresJson auto const& obj);
long long to_json(std::integral auto val);
long long to_json(std::chrono::sys_seconds const& val);
string const& to_json(string const& val);
```

Converts values to something compatible with `node`. Used in `store()`.

## json::store

```cpp
template <JsonStorableValue T>
void store(map& dst, string_view const& key, T const& value);
template <JsonStorableValue T>
void store(map& dst, string_view const& key, std::optional<T> const& value);
template <JsonStorableValue T>
void store(map& dst, string_view const& key, std::vector<T> const& values)
template <JsonStorableValue T>
void store(map& dst, string_view const& key, std::map<string, T> const& values);
```

Adds a value to `json::map` under given _key_, but only, if the value is _truthy_.

|Type|Falsy, when|
|----|-----------|
|`node`|`std::holds_alternative<std::monotype>`|
|`optional`|optional itself is _falsy_|
|`std::vector` and `std::map`|`empty()`, after converting items to JSON and filtering out _falsy_ values|


## json::store_or_value

```cpp
template <JsonStorableValue T>
void store_or_value(map& dst, string_view const& key, std::vector<T> const& values);
```

Converts _values_ to array of `json::node`s and either adds them to `json::map` under given _key_ (if size is larger than 1), adds first element in the same place (if the size is exactly 1) or does nothing (if converted array is empty).


## json::load

```cpp
conv_result load(node const& src, LoadsJson auto& obj);
```

Loads the _obj_ from _src_, provided the node holds a `json::map`. Returns `conv_result::opt` if _src_ didn't have a map, or the result of `obj.from_json()`.

```cpp
conv_result load(node const& src, LoadsJsonRaw auto& obj);
```

Loads the _obj_ from _src_ directly, returning the result of `obj.from_json()`.

```cpp
template <std::integral Int>
conv_result load(node const& src, Int& val);
conv_result load(node const& src, std::chrono::sys_seconds& val);
```

Loads the _obj_ from a `long long` held in _src_. If the node holds another alternative, the functions return `conv_result::opt`. If the value is `0`, the function does not change the _val_ and returns `conv_result::updated`. Otherwise, it converts the `long long` to expected type and returns `conv_result::ok`.

```cpp
conv_result load(node const& src, string& val);
```

Loads the _obj_ from a `json::string` held in _src_. If the node holds another alternative, the functions return `conv_result::opt`. If the value is empty, the function does not change the _val_ and returns `conv_result::updated`. Otherwise, it copies the string and returns `conv_result::ok`.

```cpp
template <LoadableJsonValue ValueType>
conv_result load(array const& src, std::vector<ValueType>& value);
```

Loads the `ValueType`s from the _src_ array. For each item `opt` results are filtered out and `failed` results break the loop. If any iteration ended with either `failed` or `updated`, the function will also return it, otherwise `ok` will be returned.

```cpp
template <LoadableJsonValue Payload>
conv_result load(map const& src, string const& key, std::optional<Payload>& value);
conv_result load(map const& src, string const& key, LoadableJsonValue auto& value);
template <LoadableJsonValue ValueType>
conv_result load(map const& src, string const& key, std::vector<ValueType>& value)
```

If the _key_ is not in the _src_ map, the functions return `conv_result::opt`. Otherwise, they use one of the six versions above to load the value from the item in the map.

The `optional` version also resets the _value_ to `nullopt`, if the inner `load()` returned either `failed` or `opt`.

Ther `vector` version also converts the mapped item to `node::array` and returns `conv_result::opt`, if there was no array.

## json::load_or_value

```cpp
template <LoadableJsonValue ValueType>
conv_result load_or_value(map const& src, string const& key,
                          std::vector<ValueType>& value)
```

If the _key_ is not in the _src_ map, the function returns `conv_result::opt`. Otherwise, if the mapped item holds an array, the array version of `load` is used. Otherwise, a single value is loaded. If the last `load()` returns `failed`, this function returns `failed`. Otherwise, if the last `load()` returns `ok` or `updated`, the single value is pushed into the vector and the result is returned from this function. Otherwise, this function returns `ok` (or in other words: `opt` is ignored).

## json::load_multi

```cpp
template <NodeType... Kind>
conv_result load_multi(node const& src,
                       LoadsJsonEx<Kind...> auto& obj);
```

Loads the _obj_ from _src_ directly, trying to get all the pointers from the node and returning the result of `obj.from_json()`. The `from_json` should check, which pointers are null and which are usable.


```cpp
template <NodeType... Kind, LoadsJsonEx<Kind...> T>
conv_result load_multi(map const& src, string const& key, T& value);
```

If the _key_ is not in the _src_ map, the function returns `conv_result::opt`. Otherwise, it uses the version above to multi-load the value from the item in the map.
