# UTF converter

[![Travis (.org)][Travis badge]][Travis]
[![Coveralls github][Coveralls badge]][Coveralls]
[![Cpp Standard][17-badge]][17]

Open-source library implementing JSON parser and writer in terms of [Mustache](https://github.com/mbits-libs/libmstch) library, which is a clone of [no1msd's](https://github.com/no1msd/mstch) implementation with couple of small changes. It assumes all strings are UTF-8, including the argument to the parser.

## Synopsis

```cpp
#include <mstch/mstch-json.hpp>
```

### mstch::read_json

```cpp
enum class mstch::read_mode { strict, ECMA };
mstch::node mstch::read_json(
    std::string_view text,
    std::string_view skip = {},
    mstch::read_mode mode = mstch::read_mode::strict);
```

Parses the input to a `node`. If the input text cannot be parsed, returned `node` will hold `std::monostate`.

If the `skip` is present at the start of `text`, it will be removed before handiong over to the parser. This helps in formatting JSONs returned by some services. For instance responses from Gerrit could be parsed with:

```cpp
auto result = mstch::read_json(result_text, ")]}'\n"sv);
```

The `read_mode::strict` accepts only JSON input, while `read_mode::ECMA` allows grammar similar to JavaScript, with `'apostrophe enclosed strings'` and commas at the end of dictionaries and array.

### mstch::write_json

```cpp
void write_json(output&, node const&);
void write_json(FILE*, node const&);
void write_json(std::string&, node const&);
```

Formats the `node` value to provided output. If the node contains only JSON-compatible values, the outpout is a valid JSON document. If the node contains a lambda, callback, dynamic object or a `std::monostate`, the output will contain a `"undefined"`, which will disqualify the result as a valid JSON.

### mstch::output

```cpp
struct output {
    virtual ~output();
    virtual void write(std::string_view) = 0;
    virtual void write(char) = 0;
};
```

Used internally by `write_json` function to output the formatted text. Library contains implementation backed by `FILE*` and `std::string` reference, but users can provide their own backends.

### mstch::cast&lt;Kind&gt;

```cpp
Kind* cast(node& value);
Kind const* cast(node const& value);
Kind* cast(node* value);
Kind const* cast(node const* value);
```

Casts the node to the expected type. If the node holds that particular alternative, the function returns pointer to the value. Otherwise, it returns `nullptr`. Versions taking pointers will also return `nullptr` if input pointer is null.

```cpp
Kind* cast(map& value, std::string const& key);
Kind const* cast(map const& value, std::string const& key);
Kind* cast(map* value, std::string const& key);
Kind const* cast(map const* value, std::string const& key);
```

Looks up the `key` in the map and then casts the mapped node to the expected type. Will return `nullptr`, if input pointer is null, if `key` is not found inside map or if mapped value cannot be cast.

```cpp
Kind* cast(node& value, std::string const& key);
Kind const* cast(node const& value, std::string const& key);
Kind* cast(node* value, std::string const& key);
Kind const* cast(node const* value, std::string const& key);
```

Equivalent of `cast<Kind>(cast<map>(value), key);`

### mstch::from_json

```cpp
node* from_json(map& value, std::string_view path);
node const* from_json(map const& value, std::string_view path);
node* from_json(map* value, std::string_view path);
node const* from_json(map const* value, std::string_view path);
```

Locates a `node` inside a possibly multi-level `map`. The `path` is a slash-separated list of keys. For instance, `from_json(dict, "key/subkey/subsubkey")` will lookup:

- `"key"` in `dict`, then
- `"subkey"` in `*cast<map>(dict["key"])`, finally
- `"subsubkey"` in `*(*cast<map>(dict["key"]))["subkey"]`.

Will return `nullptr` is it fails at eny of those steps.

```cpp
node* from_json(node& value, std::string_view path);
node const* from_json(node const& value, std::string_view path);
node* from_json(node* value, std::string_view path);
node const* from_json(node const* value, std::string_view path);
```

Equivalent of `from_json(cast<map>(value), key);`

### mstch::cast_from_json&lt;Kind&gt;

```cpp
Kind* cast_from_json(map& value, std::string const& path);
Kind const* cast_from_json(map const& value, std::string const& path);
Kind* cast_from_json(map* value, std::string const& path);
Kind const* cast_from_json(map const* value, std::string const& path);
Kind* cast_from_json(node& value, std::string const& path);
Kind const* cast_from_json(node const& value, std::string const& path);
Kind* cast_from_json(node* value, std::string const& path);
Kind const* cast_from_json(node const* value, std::string const& path);
```

Equivalent of `cast<Kind>(from_json(value, key));`

[Travis badge]: https://img.shields.io/travis/mbits-libs/libmstch-json?style=flat-square
[Travis]: https://travis-ci.org/mbits-libs/libmstch-json "Travis-CI"
[Coveralls badge]: https://img.shields.io/coveralls/github/mbits-libs/libmstch-json?style=flat-square
[Coveralls]: https://coveralls.io/github/mbits-libs/libmstch-json "Coveralls"
[17-badge]: https://img.shields.io/badge/C%2B%2B-17-informational?style=flat-square
[17]: https://en.wikipedia.org/wiki/C%2B%2B17 "Wikipedia C++17"
