// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stack>
#include <json/json.hpp>

using namespace std::literals;

namespace json {
	namespace {
		std::string as_str(std::string_view view) {
			return {view.data(), view.size()};
		}

		std::vector<std::string> split_s(char sep, std::string_view data) {
			std::vector<std::string> result{};

			auto pos = data.find(sep);
			decltype(pos) prev = 0;

			while (pos != std::string_view::npos) {
				auto const view = data.substr(prev, pos - prev);
				prev = pos + 1;
				pos = data.find(sep, prev);
				result.push_back(as_str(view));
			}

			result.push_back(as_str(data.substr(prev)));

			return result;
		}
	}  // namespace

	template <typename T, typename Q>
	struct copy_const {
		using type = Q;
	};

	template <typename T, typename Q>
	struct copy_const<T const, Q> {
		using type = Q const;
	};

	template <typename Map>
	typename copy_const<Map, node>::type* from_json_impl(Map& value,
	                                                     std::string_view key) {
		using ret_type = typename copy_const<Map, node>::type;
		auto path = split_s('.', key);

		auto it = value.find(path.front());
		if (it == value.end()) return nullptr;

		ret_type* ctx = &it->second;

		size_t count = path.size();
		for (size_t index = 1; index < count; ++index) {
			auto* dict = json::cast<map>(*ctx);
			if (!dict) return nullptr;

			auto const& property = path[index];
			it = dict->find(property);
			if (it == dict->end()) return nullptr;

			ctx = &it->second;
		}

		return ctx;
	}

	node* from_json(map& value, std::string_view path) {
		return from_json_impl(value, path);
	}

	node const* from_json(map const& value, std::string_view path) {
		return from_json_impl(value, path);
	}

	namespace {
		using uchar = unsigned char;
		using iterator = std::string_view::iterator;

		void skip_ws(iterator&, iterator const&, read_mode mode);
		node read_string(iterator&, iterator const&, read_mode mode);
		node read_number(iterator&, iterator const&, read_mode mode);
		node read_keyword(iterator&, iterator const&, read_mode mode);

		void encode(uint32_t ch, std::string& target);

		void skip_ws(iterator& it, iterator const& end, read_mode mode) {
			if (mode == read_mode::strict) {
				while (it != end) {
					switch (*it) {
						case ' ':
						case '\r':
						case '\n':
						case '\t':
							break;
						default:
							return;
					}
					++it;
				}
				return;
			}
			while (it != end && std::isspace(static_cast<uchar>(*it)))
				++it;
		}

		struct reader_state;
		enum class reader { push, replace };
		struct reader_result {
			std::variant<std::monostate,
			             std::unique_ptr<reader_state>,
			             json::node>
			    result{};
			reader result_mode{reader::push};

			reader_result() = default;
			reader_result(reader result_mode) : result_mode{result_mode} {}
			reader_result(std::unique_ptr<reader_state> next_reader,
			              reader result_mode = reader::push)
			    : result{std::move(next_reader)}, result_mode{result_mode} {}
			reader_result(json::node&& result,
			              reader result_mode = reader::replace)
			    : result{std::move(result)}, result_mode{result_mode} {}
		};

		struct reader_state {
			virtual ~reader_state() = default;
			virtual reader_result read(iterator& it,
			                           iterator const& end,
			                           read_mode mode,
			                           node& val) = 0;

		protected:
			template <typename Reader>
			static reader_result push() {
				return {std::make_unique<Reader>()};
			}
			template <typename Reader>
			static reader_result replace() {
				return {std::make_unique<Reader>(), reader::replace};
			}
			static inline reader_result read_value();

			/*
			<before_loop>
			while (<expression>) {
			    <in_loop>
			    // recursion
			    auto val = read_value(...);
			    <has_value>
			}
			<post_loop>
			return node{std::move(result)};

			becomes

			while(true) {
			    switch(state_) {
			        case before_loop:
			            <before_loop>
			            break;

			        case in_loop:
			            <in_loop>
			            // coroutine suspend?
			            stage = has_value;
			            return read_value();

			        case has_value:
			            // coroutine resume?
			            <has_value>
			            break;
			    }
			    if (!<expression>) break;
			    stage = in_loop;
			}
			<past_loop>
			return node{std::move(result)};
			*/
			enum while_stage { before_loop, in_loop, has_value };

			while_stage stage{before_loop};
		};

		class value_reader final : public reader_state {
		public:
			reader_result read(iterator& it,
			                   iterator const& end,
			                   read_mode mode,
			                   node& val) final;
		};

		inline reader_result reader_state::read_value() {
			return push<value_reader>();
		}

		class array_reader final : public reader_state {
		public:
			reader_result read(iterator& it,
			                   iterator const& end,
			                   read_mode mode,
			                   node& val) final;

		private:
			array result{};
		};

		class object_reader final : public reader_state {
		public:
			reader_result read(iterator& it,
			                   iterator const& end,
			                   read_mode mode,
			                   node& val) final;

		private:
			std::optional<std::string> read_object_key(iterator& it,
			                                           iterator const& end,
			                                           read_mode mode);
			map result{};
			std::string current_key_{};
		};

		reader_result value_reader::read(iterator& it,
		                                 iterator const& end,
		                                 read_mode mode,
		                                 node&) {
			skip_ws(it, end, mode);
			if (it == end) return {};

			if (*it == '{') return replace<object_reader>();
			if (*it == '[') return replace<array_reader>();
			if (*it == '"' || *it == '\'') return read_string(it, end, mode);
			if (std::isdigit(static_cast<uchar>(*it)) || *it == '-' ||
			    *it == '+' || *it == '.')
				return read_number(it, end, mode);
			return read_keyword(it, end, mode);
		}

		reader_result array_reader::read(iterator& it,
		                                 iterator const& end,
		                                 read_mode mode,
		                                 node& val) {
			while (true) {
				switch (stage) {
					case before_loop:
						// sanity check for value_reader::read
						assert(it != end && *it == '[');
						++it;

						skip_ws(it, end, mode);
						break;

					case in_loop:
						skip_ws(it, end, mode);
						stage = has_value;
						return read_value();

					case has_value:
						if (!val.index()) return {};
						result.push_back(std::move(val));
						skip_ws(it, end, mode);
						if (it == end) return {};
						if (*it != ',') {
							if (*it == ']') break;
							return {};
						}
						++it;
						skip_ws(it, end, mode);
						break;
				}
				if (it == end || *it == ']') break;
				stage = in_loop;
			}

			if (it == end || *it != ']') return {};
			++it;
			return node{std::move(result)};
		}

		reader_result object_reader::read(iterator& it,
		                                  iterator const& end,
		                                  read_mode mode,
		                                  node& val) {
			while (true) {
				switch (stage) {
					case before_loop:
						// sanity check for value_reader::read
						assert(it != end && *it == '{');
						++it;

						skip_ws(it, end, mode);
						break;

					case in_loop:
						// this comment forbids the formatter from placing open
						// bracket at case line
						{
							auto key = read_object_key(it, end, mode);
							if (!key) return {};
							current_key_ = std::move(*key);
						}
						skip_ws(it, end, mode);
						if (it == end || *it != ':') return {};
						++it;
						skip_ws(it, end, mode);
						stage = has_value;
						return read_value();

					case has_value:
						if (!val.index()) return {};
						{
							auto innerIt = result.find(current_key_);
							if (innerIt != result.end() &&
							    innerIt->first == current_key_) {
								innerIt->second = std::move(val);
							} else {
								result.insert(innerIt, {std::move(current_key_),
								                        std::move(val)});
							}
						}
						skip_ws(it, end, mode);
						if (it == end) return {};
						if (*it != ',') {
							if (*it == '}') break;
							return {};
						}
						++it;
						skip_ws(it, end, mode);
						break;
				}
				if (it == end || *it == '}') break;
				stage = in_loop;
			}

			if (it == end || *it != '}') return {};
			++it;
			return node{std::move(result)};
		}

		std::optional<std::string> object_reader::read_object_key(
		    iterator& it,
		    iterator const& end,
		    read_mode mode) {
			// there were skip_ws calls at the end of before_loop and has_value,
			// both of which are guarded against it == end. For now, just sanity
			// check for object_reader::read
			assert(it != end);

			if (*it == '"' || *it == '\'') {
				auto val = read_string(it, end, mode);
				if (std::holds_alternative<std::string>(val))
					return std::get<std::string>(val);
				return std::nullopt;
			}

			if (mode == read_mode::strict) return std::nullopt;

			if (std::isdigit(static_cast<uchar>(*it)) || *it == '-' ||
			    *it == '+') {
				auto val = read_number(it, end, mode);
				if (std::holds_alternative<long long>(val))
					return std::to_string(std::get<long long>(val));
				if (std::holds_alternative<double>(val))
					return std::to_string(std::get<double>(val));
				return std::nullopt;
			}

			auto start = iterator{it};
			while (it != end && *it != ':' &&
			       !std::isspace(static_cast<uchar>(*it)))
				++it;
			return std::string{start, it};
		}

		unsigned hex_digit(char c) {
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
					return static_cast<unsigned>(c - 'A' + 10);
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					return static_cast<unsigned>(c - 'a' + 10);
			}
			return 16;
		}

		unsigned hex_digit(iterator& it, iterator const& end) {
			if (it == end) return 16;
			auto const val = hex_digit(*it);
			if (val < 16) ++it;
			return val;
		}

		unsigned hex_escape(iterator& it, iterator const& end) {
			// sanity check for read_string
			assert(*it == 'x');

			++it;
			auto const result = hex_digit(it, end);
			if (result == 16) return 256;

			auto const lower = hex_digit(it, end);
			if (lower == 16) return 256;
			return result * 16 + lower;
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
					if (dig == 16) return max;
					auto const overflow_guard = val;
					val *= 16;
					val += dig;
					if (overflow_guard > val) return max;
				}
				if (it != end) ++it;
				return val;
			}

			uint32_t val = 0;
			for (int i = 0; i < 4; ++i) {
				auto const dig = hex_digit(it, end);
				if (dig == 16) return max;
				val *= 16;
				val += dig;
			}
			return val;
		}

		node read_string(iterator& it, iterator const& end, read_mode mode) {
			// sanity check for value_reader::read and
			// object_reader::read_object_key
			assert(it != end && (*it == '"' || *it == '\''));

			if (mode == read_mode::strict && *it == '\'') return {};

			auto tmplt = *it;
			++it;

			std::string result{};
			bool in_string = true;
			bool in_escape = false;
			while (it != end && in_string) {
				if (in_escape) {
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
							if (val > 255) return {};
							result.push_back(
							    static_cast<char>(static_cast<uchar>(val)));
							--it;
							break;
						}
						case 'u': {
							auto const val = unicode_escape(it, end);
							if (val == std::numeric_limits<uint32_t>::max())
								return {};
							encode(val, result);
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
						    static_cast<unsigned char>(*it) < 0x20)
							return {};
						result.push_back(*it);
				}
				++it;
			}

			if (mode == read_mode::strict && in_string) return {};
			return node{std::move(result)};
		}

		node read_int(iterator& it,
		              iterator const& end,
		              unsigned base,
		              int neg) {
			if (it == end) return {};
			static constexpr auto max = std::numeric_limits<long long>::max();
			auto const max10th = max / base;
			long long result{};
			bool read = false;
			while (it != end) {
				auto const digit = hex_digit(*it);
				if (digit >= base) {
					if (read) break;
					return {};
				}
				if (result > max10th) return {};
				result *= base;
				result += digit;
				read = true;
				++it;
			}
			return result * neg;
		}

		std::optional<std::pair<unsigned long long, unsigned>> read_digits(
		    iterator& it,
		    iterator const& end,
		    bool strict_mode = true) {
			std::pair<unsigned long long, unsigned> result{};
			auto& [value, power] = result;

			bool read = false;
			while (it != end) {
				auto const digit = hex_digit(*it);
				if (digit > 9) {
					if (!read && strict_mode) return std::nullopt;
					break;
				}
				read = true;
				++it;
				++power;
				auto const overflow_guard = value;
				value *= 10;
				value += digit;
				if (overflow_guard > value) return std::nullopt;
			}

			return result;
		}

		node read_number(iterator& it, iterator const& end, read_mode mode) {
			// sanity check for value_reader::read and
			// object_reader::read_object_key
			assert(it != end);

			int neg = 1;

			if (*it == '+') {
				if (mode == read_mode::strict) return {};
				++it;
			} else if (*it == '-') {
				neg = -1;
				++it;
			}

			if (it == end) return {};
			if (*it == '0') {
				++it;
				if (it == end) return 0ll;
				if (mode == read_mode::strict) {
					switch (*it) {
						default:
							return 0ll;
						case '.':
						case 'e':
						case 'E':
							--it;
							break;
					}
				} else {
					switch (*it) {
						case 'b':
							++it;
							return read_int(it, end, 2, neg);
						case 'o':
							++it;
							return read_int(it, end, 8, neg);
						case 'x':
						case 'X':
							++it;
							return read_int(it, end, 16, neg);
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							return read_int(it, end, 8, neg);
						default:
							return 0ll;
						case '.':
						case 'e':
						case 'E':
							--it;
							break;
					}
				}
			}

			auto const whole = read_digits(it, end, mode == read_mode::strict);
			if (!whole) return {};

			if (it == end || (*it != '.' && *it != 'e' && *it != 'E'))
				return static_cast<long long>(whole->first) * neg;

			std::pair<unsigned long long, unsigned> fraction{};
			if (*it == '.') {
				++it;
				auto const fraction_ = read_digits(it, end);
				if (!fraction_) return {};

				if (it == end || (*it != 'e' && *it != 'E')) {
					auto const div = std::pow(10.0, fraction_->second);
					return (double(whole->first) +
					        double(fraction_->first) / div) *
					       neg;
				}

				fraction = *fraction_;
			}

			++it;

			int exp_neg = 1;
			if (*it == '+')
				++it;
			else if (*it == '-') {
				exp_neg = -1;
				++it;
			}

			auto const exponent = read_digits(it, end);
			if (!exponent) return {};

			auto const int_exp =
			    static_cast<long long>(exponent->first) * exp_neg;
			auto const full_exp =
			    int_exp + static_cast<long long>(whole->second - 1);

			if (full_exp < std::numeric_limits<double>::min_exponent10 ||
			    full_exp > std::numeric_limits<double>::max_exponent10)
				return {};

			auto const pow10 = std::pow(10.0, int_exp);
			auto const div = std::pow(10.0, fraction.second);
			return (double(whole->first) + double(fraction.first) / div) *
			       pow10 * neg;
		}

		node read_keyword(iterator& it, iterator const& end, read_mode mode) {
			if (it == end) {
				return {};
			}
			auto start = it;
			while (it != end && std::isalpha(static_cast<uchar>(*it)))
				++it;
			std::string_view view{&*start, static_cast<size_t>(it - start)};
			if (view == "null") return nullptr;
			if (view == "true") return true;
			if (view == "false") return false;
			if (mode == read_mode::strict) return {};
			if (view == "undefined") return nullptr;
			return {};
		}

		static constexpr const uchar firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0,
		                                                 0xF0, 0xF8, 0xFC};

		enum : uint32_t {
			UNI_SUR_HIGH_START = 0xD800,
			UNI_SUR_HIGH_END = 0xDBFF,
			UNI_SUR_LOW_START = 0xDC00,
			UNI_SUR_LOW_END = 0xDFFF,
			UNI_REPLACEMENT_CHAR = 0x0000FFFD,
			UNI_MAX_BMP = 0x0000FFFF,
			UNI_MAX_UTF16 = 0x0010FFFF,
			UNI_MAX_LEGAL_UTF32 = 0x0010FFFF
		};

		static constexpr const uint32_t byteMask = 0xBF;
		static constexpr const uint32_t byteMark = 0x80;

		void encode(uint32_t ch, std::string& target) {
			unsigned short bytesToWrite = 0;

			/* Figure out how many bytes the result will require */
			if (ch < 0x80u)
				bytesToWrite = 1;
			else if (ch < 0x800u)
				bytesToWrite = 2;
			else if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				bytesToWrite = 3;
				ch = UNI_REPLACEMENT_CHAR;
			} else if (ch < 0x10000u)
				bytesToWrite = 3;
			else if (ch <= UNI_MAX_LEGAL_UTF32)
				bytesToWrite = 4;
			else {
				bytesToWrite = 3;
				ch = UNI_REPLACEMENT_CHAR;
			}

			uchar mid[4];
			uchar* midp = mid + sizeof(mid);
			switch (bytesToWrite) { /* note: everything falls through. */
				case 4:
					*--midp = static_cast<uchar>((ch | byteMark) & byteMask);
					ch >>= 6;
					[[fallthrough]];
				case 3:
					*--midp = static_cast<uchar>((ch | byteMark) & byteMask);
					ch >>= 6;
					[[fallthrough]];
				case 2:
					*--midp = static_cast<uchar>((ch | byteMark) & byteMask);
					ch >>= 6;
					[[fallthrough]];
				case 1:
					*--midp =
					    static_cast<uchar>(ch | firstByteMark[bytesToWrite]);
			}
			for (int i = 0; i < bytesToWrite; ++i)
				target.push_back(static_cast<char>(*midp++));
		}

		struct writer {
			output& printer;
			unsigned indent_size{};

			void write(node const& value) { std::visit(*this, value.base()); }

			template <typename T>
			void operator()(T const&) const {
				printer.write("undefined"sv);  // monspace, lambda and object
			}

			void operator()(std::nullptr_t) const { printer.write("null"sv); }

			void operator()(bool value) const {
				printer.write(value ? "true"sv : "false"sv);
			}

			void operator()(long long value) const {
				char buffer[200];
				auto length = snprintf(buffer, sizeof(buffer), "%lld", value);
				printer.write({buffer, static_cast<size_t>(length)});
			}

			void operator()(double value) const {
				char buffer[200];
				auto length = snprintf(buffer, sizeof(buffer), "%g", value);
				printer.write({buffer, static_cast<size_t>(length)});
			}

			void operator()(std::string const& value) const {
				write_string(value);
			}

			void operator()(map const& values) const {
				if (values.empty()) {
					printer.write("{}"sv);
					return;
				}
				printer.write('{');
				if (values.size() == 1) {
					auto const& [key, value] = *values.begin();
					write_string(key);
					printer.write(": "sv);
					writer{printer, indent_size}.write(value);
					printer.write('}');
					return;
				}
				bool first = true;
				for (auto const& [key, value] : values) {
					if (first)
						first = false;
					else
						printer.write(',');

					indent();
					printer.write("    "sv);
					write_string(key);
					printer.write(": "sv);
					writer{printer, indent_size + 1}.write(value);
				}
				indent();
				printer.write('}');
			}

			void operator()(array const& values) const {
				if (values.empty()) {
					printer.write("[]"sv);
					return;
				}
				printer.write('[');
				if (values.size() == 1) {
					writer{printer, indent_size}.write(values.front());
					printer.write(']');
					return;
				}

				bool first = true;
				for (auto const& value : values) {
					if (first)
						first = false;
					else
						printer.write(',');

					indent();
					printer.write("    "sv);
					writer{printer, indent_size + 1}.write(value);
				}
				indent();
				printer.write(']');
			}

			void indent() const {
				printer.write('\n');
				for (unsigned counter = 0; counter < indent_size; ++counter)
					printer.write("    "sv);
			}

			void write_string(std::string const& s) const {
				printer.write('"');
				for (auto c : s) {
					switch (c) {
						default:
							if (std::iscntrl(static_cast<unsigned char>(c)))
								write_control(c);
							else
								printer.write(c);
							break;
						case '\"':
							printer.write("\\\""sv);
							break;
						case '\\':
							printer.write("\\\\"sv);
							break;
						case '\b':
							printer.write("\\b"sv);
							break;
						case '\f':
							printer.write("\\f"sv);
							break;
						case '\n':
							printer.write("\\n"sv);
							break;
						case '\r':
							printer.write("\\r"sv);
							break;
						case '\t':
							printer.write("\\t"sv);
							break;
					}
				}
				printer.write('"');
			}

			void write_control(char c) const {
				char buff[10];
				auto const len =
				    sprintf(buff, "%04x", static_cast<unsigned char>(c));
				printer.write("\\u"sv);
				printer.write({buff, static_cast<size_t>(len)});
			}
		};

		class file_output final : public output {
		public:
			file_output(FILE* file) : file_{file} {};

			void write(std::string_view str) override {
				std::fwrite(str.data(), 1, str.size(), file_);
			}
			void write(char c) override { std::fputc(c, file_); }

		private:
			FILE* file_{};
		};

		class string_output final : public output {
		public:
			string_output(std::string& str) : str_{str} {};

			void write(std::string_view str) override { str_.append(str); }
			void write(char c) override { str_.push_back(c); }

		private:
			std::string& str_;
		};
	}  // namespace

	node read_json(std::string_view text,
	               std::string_view skip,
	               read_mode mode) {
		if (!skip.empty() && text.substr(0, skip.size()) == skip)
			text = text.substr(skip.size());

		auto it = text.begin();
		auto end = text.end();

		std::stack<std::unique_ptr<reader_state>> stack;
		stack.push(std::make_unique<value_reader>());

		node value;
		while (!stack.empty()) {
			auto reader = std::move(stack.top());
			stack.pop();
			auto [result, result_mode] = reader->read(it, end, mode, value);
			value = {};

			if (std::holds_alternative<std::monostate>(result)) return {};

			if (std::holds_alternative<std::unique_ptr<reader_state>>(result)) {
				auto& next = std::get<std::unique_ptr<reader_state>>(result);
				if (result_mode != json::reader::replace)
					stack.push(std::move(reader));

				stack.push(std::move(next));
				continue;
			}

			value = std::move(std::get<node>(result));
		}

		if (mode == read_mode::strict) {
			skip_ws(it, text.end(), mode);
			if (it != text.end()) return {};
		}

		return value;
	}

	output::~output() = default;

	void write_json(output& printer, node const& value) {
		writer{printer}.write(value);
	}

	void write_json(FILE* file, node const& value) {
		file_output printer{file};
		write_json(printer, value);
	}

	void write_json(std::string& output, node const& value) {
		output.clear();
		string_output printer{output};
		write_json(printer, value);
	}
}  // namespace mstch
