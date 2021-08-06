# JSON parser

[![Travis (.org)][Travis badge]][Travis]
[![Coveralls github][Coveralls badge]][Coveralls]
[![Cpp Standard][20-badge]][20]

Open-source library implementing JSON parser and writer using . It assumes all strings are UTF-8, including the argument to the parser.

## Strings in C++20

C++20 introduces both `char8_t` with `std::u8string` and `std::u8string_view`. It also deprecates `u8path` in favor of `path` constructor taking `std::u8string const&`. The intertpretation there is, paths created from `char`-based strings represent current OS settings, while paths created from `char8_t`-based strings represent paths, that are OS-independent.

This library copies this assumption. The text to read from and write to are both `char8_t`-based strings, as well as indexes to maps and strings held by a JSON node. There are `char8_t` aliases provided for both `json::string` and `json::string_view` and `cast<>` and `cast_from_json<>` will not accept `std::string`.

## Synopsis

```cpp
#include <json/json.hpp>
```

### json::NodeType&lt;Kind&gt;

```cpp
template <typename Kind>
concept json::NodeType;
```

A concept accepting only the types `json::node` can hold (with exception of `std::monostate`). It is used by all the `json::cast` and `json::cast_from_json` to weed out incompatible `Kind` arguments, especially wrong types of strings.

### json::read_json

```cpp
enum class json::read_mode { strict, ECMA };
json::node json::read_json(
    json::string_view text,
    json::string_view skip = {},
    json::read_mode mode = json::read_mode::strict);
```

Parses the input to a `node`. If the input text cannot be parsed, returned `node` will hold `std::monostate`.

If the `skip` is present at the start of `text`, it will be removed before handiong over to the parser. This helps in formatting JSONs returned by some services. For instance responses from Gerrit could be parsed with:

```cpp
auto result = json::read_json(result_text, u8")]}'\n"sv);
```

The `read_mode::strict` accepts only JSON input, while `read_mode::ECMA` allows grammar similar to JavaScript, with `'apostrophe enclosed strings'` and commas at the end of dictionaries and array.

### json::write_json

```cpp
void write_json(output&, node const&);
void write_json(FILE*, node const&);
void write_json(json::string&, node const&);
```

Formats the `node` value to provided output. If the node contains only JSON-compatible values, the outpout is a valid JSON document. If the node contains a `std::monostate`, the output will contain a `"undefined"`, which will disqualify the result as a valid JSON.

### json::output

```cpp
struct output {
    virtual ~output();
    virtual void write(json::string_view) = 0;
    virtual void write(char) = 0;
};
```

Used internally by `write_json` function to output the formatted text. Library contains implementation backed by `FILE*` and `json::string` reference, but users can provide their own backends.

### json::cast&lt;Kind&gt;

```cpp
Kind* cast(node& value);
Kind const* cast(node const& value);
Kind* cast(node* value);
Kind const* cast(node const* value);
```

Casts the node to the expected type. If the node holds that particular alternative, the function returns pointer to the value. Otherwise, it returns `nullptr`. Versions taking pointers will also return `nullptr` if input pointer is null.

```cpp
Kind* cast(map& value, json::string const& key);
Kind const* cast(map const& value, json::string const& key);
Kind* cast(map* value, json::string const& key);
Kind const* cast(map const* value, json::string const& key);
```

Looks up the `key` in the map and then casts the mapped node to the expected type. Will return `nullptr`, if input pointer is null, if `key` is not found inside map or if mapped value cannot be cast.

```cpp
Kind* cast(node& value, json::string const& key);
Kind const* cast(node const& value, json::string const& key);
Kind* cast(node* value, json::string const& key);
Kind const* cast(node const* value, json::string const& key);
```

Equivalent of `cast<Kind>(cast<map>(value), key);`

### json::from_json

```cpp
node* from_json(map& value, json::string_view path);
node const* from_json(map const& value, json::string_view path);
node* from_json(map* value, json::string_view path);
node const* from_json(map const* value, json::string_view path);
```

Locates a `node` inside a possibly multi-level `map`. The `path` is a dot-separated list of keys. For instance, `from_json(dict, u8"object.subobject.property")` will lookup:

- `"object"` in `dict`, then
- `"subobject"` in `*cast<map>(dict["object"])`, finally
- `"property"` in `*(*cast<map>(dict["object"]))["subobject"]`.

Will return `nullptr` is it fails at any of those steps.

```cpp
node* from_json(node& value, json::string_view path);
node const* from_json(node const& value, json::string_view path);
node* from_json(node* value, json::string_view path);
node const* from_json(node const* value, json::string_view path);
```

Equivalent of `from_json(cast<map>(value), key);`

### json::cast_from_json&lt;Kind&gt;

```cpp
Kind* cast_from_json(map& value, json::string_view const& path);
Kind const* cast_from_json(map const& value, json::string_view const& path);
Kind* cast_from_json(map* value, json::string_view const& path);
Kind const* cast_from_json(map const* value, json::string_view const& path);
Kind* cast_from_json(node& value, json::string_view const& path);
Kind const* cast_from_json(node const& value, json::string_view const& path);
Kind* cast_from_json(node* value, json::string_view const& path);
Kind const* cast_from_json(node const* value, json::string_view const& path);
```

Equivalent of `cast<Kind>(from_json(value, key));`

[Travis badge]: https://img.shields.io/travis/mbits-libs/libmstch-json?style=flat-square
[Travis]: https://travis-ci.org/mbits-libs/libmstch-json "Travis-CI"
[Coveralls badge]: https://img.shields.io/coveralls/github/mbits-libs/libmstch-json?style=flat-square
[Coveralls]: https://coveralls.io/github/mbits-libs/libmstch-json "Coveralls"
[20-badge]: https://img.shields.io/badge/C%2B%2B-20-informational?style=flat-square
[20]: https://en.wikipedia.org/wiki/C%2B%2B20 "Wikipedia C++20"
