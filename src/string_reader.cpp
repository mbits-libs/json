// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <stack>
#include <json/json.hpp>

using namespace std::literals;

namespace json {
	namespace {
		using uchar = unsigned char;
		using iterator = string_view::iterator;

		enum : uint32_t {
			UNI_SUR_HIGH_START = 0xD800,
			UNI_SUR_HIGH_END = 0xDBFF,
			UNI_SUR_LOW_START = 0xDC00,
			UNI_SUR_LOW_END = 0xDFFF,
			UNI_REPLACEMENT_CHAR = 0x0000FFFD,
			UNI_MAX_BMP = 0x0000FFFF,
			UNI_MAX_UTF16 = 0x0010FFFF,
			UNI_MAX_LEGAL_UTF32 = 0x0010FFFF,
			UNI_NONCHARACTER_MIN = 0xFFFE,
			UNI_NONCHARACTER_MAX = 0xFFFF,
			UNI_NONCHARACTER_STEP = 0x10000,
		};

#define NONCHARACTER_RANGE(N)                                \
	std::pair {                                              \
		UNI_NONCHARACTER_MIN + UNI_NONCHARACTER_STEP*(N),    \
		    UNI_NONCHARACTER_MAX + UNI_NONCHARACTER_STEP*(N) \
	}

		static constexpr std::array noncharacters = {
		    NONCHARACTER_RANGE(0),
		    NONCHARACTER_RANGE(1),
		    NONCHARACTER_RANGE(2),
		    NONCHARACTER_RANGE(3),
		    NONCHARACTER_RANGE(4),
		    NONCHARACTER_RANGE(5),
		    NONCHARACTER_RANGE(6),
		    NONCHARACTER_RANGE(7),
		    NONCHARACTER_RANGE(8),
		    NONCHARACTER_RANGE(9),
		    NONCHARACTER_RANGE(10),
		    NONCHARACTER_RANGE(11),
		    NONCHARACTER_RANGE(12),
		    NONCHARACTER_RANGE(13),
		    NONCHARACTER_RANGE(14),
		    NONCHARACTER_RANGE(15),
		    NONCHARACTER_RANGE(16),
		    std::pair<uint32_t, uint32_t>{0xFDD0, 0xFDEF},
		};

		constinit auto const INV_HEX = 16U;
		constinit auto const INV_HEX_SQUARE = 256U;
		constinit auto const CHAR_SPACE = 0x20;

		unsigned hex_digit(string::value_type c) {
			switch (c) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return static_cast<unsigned>(c - '0');
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
					return static_cast<unsigned>(c - 'A' + 10);  // NOLINT
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					return static_cast<unsigned>(c - 'a' + 10);  // NOLINT
			}
			return INV_HEX;
		}

		unsigned hex_digit(iterator& it, iterator const& end) {
			if (it == end) return INV_HEX;
			auto const val = hex_digit(*it);
			if (val < INV_HEX) ++it;
			return val;
		}

		unsigned hex_escape(iterator& it, iterator const& end) {
			// sanity check for read_string
			assert(*it == 'x');

			++it;
			auto const result = hex_digit(it, end);
			if (result == INV_HEX) return INV_HEX_SQUARE;

			auto const lower = hex_digit(it, end);
			if (lower == INV_HEX) return INV_HEX_SQUARE;
			return result * 16 + lower;  // NOLINT(readability-magic-numbers)
		}

		uint32_t unicode_escape(iterator& it, iterator const& end) {
			static constexpr auto max = std::numeric_limits<uint32_t>::max();
			// sanity check for read_string
			assert(*it == 'u');

			++it;
			if (it == end) return max;
			if (*it == '{') {
				++it;
				uint32_t val = 0;
				while (it != end && *it != '}') {
					auto const dig = hex_digit(it, end);
					if (dig == INV_HEX) return max;
					auto const overflow_guard = val;
					val *= 16;  // NOLINT(readability-magic-numbers)
					val += dig;
					if (overflow_guard > val) return max;
				}
				if (it != end) ++it;
				return val;
			}

			uint32_t val = 0;
			for (int i = 0; i < 4; ++i) {
				auto const dig = hex_digit(it, end);
				if (dig == INV_HEX) return max;
				val *= 16;  // NOLINT(readability-magic-numbers)
				val += dig;
			}
			return val;
		}

		constexpr uchar firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0,
		                                    0xF0, 0xF8, 0xFC};

		constexpr uint32_t byteMask = 0xBF;
		constexpr uint32_t byteMark = 0x80;

		void encode(uint32_t ch, string& target) {
			unsigned short bytesToWrite = 0;

			/* Figure out how many bytes the result will require */
			if (ch < 0x80u)  // NOLINT
				bytesToWrite = 1;
			else if (ch < 0x800u)  // NOLINT
				bytesToWrite = 2;  // NOLINT
			else if (ch >= UNI_SUR_HIGH_START &&
			         ch <= UNI_SUR_LOW_END) {  // NOLINT
				bytesToWrite = 3;              // NOLINT
				ch = UNI_REPLACEMENT_CHAR;
			} else if (ch < 0x10000u)  // NOLINT
				bytesToWrite = 3;      // NOLINT
			else if (ch <= UNI_MAX_LEGAL_UTF32)
				bytesToWrite = 4;  // NOLINT
			else {
				bytesToWrite = 3;  // NOLINT
				ch = UNI_REPLACEMENT_CHAR;
			}

			uchar mid[4];
			uchar* midp = mid + sizeof(mid);
			switch (bytesToWrite) { /* note: everything falls through. */
				case 4:             // NOLINT
					*--midp = static_cast<uchar>((ch | byteMark) & byteMask);
					ch >>= 6;  // NOLINT
					[[fallthrough]];
				case 3:
					*--midp = static_cast<uchar>((ch | byteMark) & byteMask);
					ch >>= 6;  // NOLINT
					[[fallthrough]];
				case 2:
					*--midp = static_cast<uchar>((ch | byteMark) & byteMask);
					ch >>= 6;  // NOLINT
					[[fallthrough]];
				case 1:
					*--midp =
					    static_cast<uchar>(ch | firstByteMark[bytesToWrite]);
			}
			for (int i = 0; i < bytesToWrite; ++i)
				target.push_back(static_cast<string::value_type>(*midp++));
		}

		bool is_noncharacter(uint32_t ch) {
			for (auto const& [from, to] : noncharacters) {
				if (ch >= from && ch <= to) {
					return true;
				}
			}
			return false;
		}
	}  // namespace

	node read_string(iterator& it, iterator const& end, read_mode mode) {
		// sanity check for value_reader::read and
		// object_reader::read_object_key
		assert(it != end && (*it == '"' || *it == '\''));

		if (mode == read_mode::strict && *it == '\'') return {};

		auto tmplt = *it;
		++it;

		std::optional<uint32_t> lead = std::nullopt;

		string result{};
		bool in_string = true;
		bool in_escape = false;
		while (it != end && in_string) {
			if (in_escape) {
				if (*it != 'u' && lead) {
					encode(UNI_REPLACEMENT_CHAR, result);
					lead = std::nullopt;
				}

				switch (*it) {
					case '\r':
						if (mode == read_mode::strict) return {};
						++it;
						if (it != end && *it == '\n') break;
						--it;
						break;
					case '\n':
						if (mode == read_mode::strict) return {};
						++it;
						if (it != end && *it == '\r') break;
						--it;
						break;
					case 'b':
						result.push_back('\b');
						break;
					case 'f':
						result.push_back('\f');
						break;
					case 'n':
						result.push_back('\n');
						break;
					case 'r':
						result.push_back('\r');
						break;
					case 't':
						result.push_back('\t');
						break;
					case 'v':
						if (mode == read_mode::strict) return {};
						result.push_back('\v');
						break;
					case 'x': {
						if (mode == read_mode::strict) return {};
						auto const val = hex_escape(it, end);
						if (val > 255)  // NOLINT(readability-magic-numbers)
							return {};
						result.push_back(static_cast<string::value_type>(
						    static_cast<uchar>(val)));
						--it;
						break;
					}
					case 'u': {
						auto const val = unicode_escape(it, end);
						if (val == std::numeric_limits<uint32_t>::max())
							return {};
						if (val >= UNI_SUR_HIGH_START &&
						    val <= UNI_SUR_HIGH_END) {
							if (lead) {
								encode(UNI_REPLACEMENT_CHAR, result);
							}
							lead = val;
						} else if (val >= UNI_SUR_LOW_START &&
						           val <= UNI_SUR_LOW_END) {
							if (!lead) {
								encode(UNI_REPLACEMENT_CHAR, result);
							} else {
								auto const val2 =
								    (*lead - UNI_SUR_HIGH_START) * 0x400u +
								    (val - UNI_SUR_LOW_START) + 0x10000u;
								if (is_noncharacter(*lead) ||
								    is_noncharacter(val) ||
								    is_noncharacter(val2)) {
									encode(UNI_REPLACEMENT_CHAR, result);
									encode(UNI_REPLACEMENT_CHAR, result);
								} else {
									encode(val2, result);
								}
								lead = std::nullopt;
							}
						} else {
							if (lead) {
								encode(UNI_REPLACEMENT_CHAR, result);
								lead = std::nullopt;
							}
							encode(val, result);
						}
						--it;
						break;
					}
					case '"':
					case '\\':
					case '/':
						result.push_back(*it);
						break;
					default:
						if (mode == read_mode::strict) return {};
						result.push_back(*it);
						break;
				}
				++it;
				in_escape = false;
				continue;
			}

			if (*it == tmplt) {
				++it;
				in_string = false;
				continue;
			}

			switch (*it) {
				case '\\':
					in_escape = true;
					break;
				default:
					if (mode == read_mode::strict &&
					    static_cast<unsigned char>(*it) < CHAR_SPACE)
						return {};

					if (lead) {
						encode(UNI_REPLACEMENT_CHAR, result);
						lead = std::nullopt;
					}

					result.push_back(*it);
			}
			++it;
		}

		if (lead) {
			encode(UNI_REPLACEMENT_CHAR, result);
		}

		if (mode == read_mode::strict && in_string) return {};
		return node{std::move(result)};
	}
}  // namespace json
