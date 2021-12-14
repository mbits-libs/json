# JSON parser

[![GitHub Workflow Status][Workflow badge]][Actions]
[![Coveralls github][Coveralls badge]][Coveralls]
[![Cpp Standard][20-badge]][20]

Open-source library implementing JSON parser and writer using C++17 `std::variant`. It assumes all strings are UTF-8, including the argument to the parser.

## Strings in C++20

C++20 introduces both `char8_t` with `std::u8string` and `std::u8string_view`. It also deprecates `u8path` in favor of `path` constructor taking `std::u8string const&`. The intertpretation there is, paths created from `char`-based strings represent current OS settings, while paths created from `char8_t`-based strings represent paths, that are OS-independent.

This library copies this assumption. The text to read from and write to are both `char8_t`-based strings, as well as indexes to maps and strings held by a JSON node. There are `char8_t` aliases provided for both `json::string` and `json::string_view` and `cast<>` and `cast_from_json<>` will not accept `std::string`.

## See also:

1. [Basic JSON interface](docs/json.hpp.md)
1. [Serializer/deserializer](docs/serder.hpp.md)


[Workflow badge]: https://img.shields.io/github/workflow/status/mbits-libs/json/C++%20build?style=flat-square
[Actions]: https://github.com/mbits-libs/json/actions/workflows/build.yaml
[Coveralls badge]: https://img.shields.io/coveralls/github/mbits-libs/json?style=flat-square
[Coveralls]: https://coveralls.io/github/mbits-libs/json "Coveralls"
[20-badge]: https://img.shields.io/badge/C%2B%2B-20-informational?style=flat-square
[20]: https://en.wikipedia.org/wiki/C%2B%2B20 "Wikipedia C++20"
