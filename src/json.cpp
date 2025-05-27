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
		string as_str(string_view view) { return {view.data(), view.size()}; }

		std::vector<string> split_s(string_view::value_type sep,
		                            string_view data) {
			std::vector<string> result{};

			auto pos = data.find(sep);
			decltype(pos) prev = 0;

			while (pos != string_view::npos) {
				auto const view = data.substr(prev, pos - prev);
				prev = pos + 1;
				pos = data.find(sep, prev);
				result.push_back(as_str(view));
			}

			result.push_back(as_str(data.substr(prev)));

			return result;
		}

		std::u8string as_u8(std::string_view view) {
			return {reinterpret_cast<string::value_type const*>(view.data()),
			        view.length()};
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
	                                                     string_view key) {
		using ret_type = typename copy_const<Map, node>::type;
		auto path = split_s('.', key);

		auto it = value.find(path.front());
		if (it == value.end()) return nullptr;

		ret_type* ctx = &it->second;

		size_t count = path.size();
		for (size_t index = 1; index < count; ++index) {
			auto* dict = cast<map>(*ctx);
			if (!dict) return nullptr;

			auto const& property = path[index];
			it = dict->find(property);
			if (it == dict->end()) return nullptr;

			ctx = &it->second;
		}

		return ctx;
	}

	node* from_json(map& value, string_view path) {
		return from_json_impl(value, path);
	}

	node const* from_json(map const& value, string_view path) {
		return from_json_impl(value, path);
	}

	node read_string(string_view::iterator&,
	                 string_view::iterator const&,
	                 read_mode mode);

	namespace {
		using uchar = unsigned char;
		using iterator = string_view::iterator;

		void skip_ws(iterator&, iterator const&, read_mode mode);
		node read_number(iterator&, iterator const&, read_mode mode);
		node read_keyword(iterator&, iterator const&, read_mode mode);

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

		struct reader_result;

		struct
		    reader_state {  // NOLINT(cppcoreguidelines-special-member-functions)
			virtual ~reader_state() = default;
			virtual reader_result read(iterator& it,
			                           iterator const& end,
			                           read_mode mode,
			                           node& val) = 0;

		protected:
			template <typename Reader>
			static inline reader_result push();
			template <typename Reader>
			static inline reader_result replace();
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

			while_stage
			    stage  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
			    {before_loop};
		};

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

		template <typename Reader>
		reader_result reader_state::push() {
			return {std::make_unique<Reader>()};
		}
		template <typename Reader>
		reader_result reader_state::replace() {
			return {std::make_unique<Reader>(), reader::replace};
		}

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
			std::optional<string> read_object_key(iterator& it,
			                                      iterator const& end,
			                                      read_mode mode);
			map result{};
			string current_key_{};
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
						val = {};
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
							auto innerIt = result.lower_bound(current_key_);
							if (innerIt != result.end() &&
							    innerIt->first == current_key_) {
								innerIt->second = std::move(val);
								val = {};
							} else {
								result.insert(innerIt, {std::move(current_key_),
								                        std::move(val)});
								val = {};
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

		std::optional<string> object_reader::read_object_key(
		    iterator& it,
		    iterator const& end,
		    read_mode mode) {
			// there were skip_ws calls at the end of before_loop and has_value,
			// both of which are guarded against it == end. For now, just sanity
			// check for object_reader::read
			assert(it != end);

			if (*it == '"' || *it == '\'') {
				auto val = read_string(it, end, mode);
				if (std::holds_alternative<string>(val))
					return std::get<string>(val);
				return std::nullopt;
			}

			if (mode == read_mode::strict) return std::nullopt;

			if (std::isdigit(static_cast<uchar>(*it)) || *it == '-' ||
			    *it == '+') {
				auto val = read_number(it, end, mode);
				if (std::holds_alternative<long long>(val))
					return as_u8(std::to_string(std::get<long long>(val)));
				if (std::holds_alternative<double>(val))
					return as_u8(std::to_string(std::get<double>(val)));
				return std::nullopt;
			}

			auto start = iterator{it};
			while (it != end && *it != ':' &&
			       !std::isspace(static_cast<uchar>(*it)))
				++it;
			return string{start, it};
		}

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
				if (digit > 9) {  // NOLINT(readability-magic-numbers)
					if (!read && strict_mode) return std::nullopt;
					break;
				}
				read = true;
				++it;
				++power;
				auto const overflow_guard = value;
				value *= 10;  // NOLINT(readability-magic-numbers)
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
				if (it == end) return 0LL;
				if (mode == read_mode::strict) {
					switch (*it) {
						default:
							return 0LL;
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
							return read_int(it, end, 2, neg);  // NOLINT
						case 'o':
							++it;
							return read_int(it, end, 8, neg);  // NOLINT
						case 'x':
						case 'X':
							++it;
							return read_int(it, end, 16, neg);  // NOLINT
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
							return read_int(it, end, 8, neg);  // NOLINT
						default:
							return 0LL;
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
			string_view view{&*start, static_cast<size_t>(it - start)};
			if (view == u8"null"sv) return nullptr;
			if (view == u8"true"sv) return true;
			if (view == u8"false"sv) return false;
			if (mode == read_mode::strict) return {};
			if (view == u8"undefined"sv) return nullptr;
			return {};
		}

		struct size_judge {
			size_t allowed_space;

			template <typename T>
			bool operator()(T const&) noexcept {
				return false;
			}

			bool consume(size_t length) noexcept {
				if (length > allowed_space) {
					allowed_space = 0;
					return false;
				}
				allowed_space -= length;
				return true;
			}

			bool operator()(node const& v) noexcept {
				try {
					return std::visit(*this, v.base());
				} catch (...) {
					return false;
				}
			}

			bool operator()(std::nullptr_t) noexcept { return consume(4); }

			bool operator()(bool value) noexcept {
				return consume(value ? 4 : 5);  // NOLINT
			}

			bool operator()(long long value) noexcept {
				char buffer[200];  // NOLINT
				auto length = snprintf(buffer, sizeof(buffer), "%lld", value);
				return consume(static_cast<size_t>(length));
			}

			bool operator()(double value) noexcept {
				char buffer[200];  // NOLINT
				auto length = snprintf(buffer, sizeof(buffer), "%g", value);
				return consume(static_cast<size_t>(length));
			}

			bool operator()(string const& value) noexcept {
				// quick and dirty...
				return consume(value.length() + 2);
			}

			bool operator()(map const& values) noexcept {
				if (!consume(2)) return false;
				bool first = true;
				for (auto const& key : values.keys()) {
					if (first) {
						first = false;
					} else {
						if (!consume(2)) return false;
					}

					if (!(*this)(key)) return false;
					if (!consume(2)) return false;
					if (!(*this)(values.find(key)->second)) return false;
				}
				return true;
			}

			bool operator()(array const& values) noexcept {
				if (!consume(2)) return false;
				bool first = true;
				for (auto const& value : values) {
					if (first) {
						first = false;
					} else {
						if (!consume(2)) return false;
					}

					if (!(*this)(value)) return false;
				}
				return true;
			}
		};

		struct writer {
			output& printer;
			write_config const& config;
			unsigned indent_size{};
			bool force_one_line{false};

			void write(node const& value) { std::visit(*this, value.base()); }

			template <typename T>
			void operator()(T const&) const {
				printer.write(u8"undefined"sv);  // monspace, lambda and object
			}

			void operator()(std::nullptr_t) const { printer.write(u8"null"sv); }

			void operator()(bool value) const {
				printer.write(value ? u8"true"sv : u8"false"sv);
			}

			void operator()(long long value) const {
				char buffer[200];  // NOLINT
				auto length = snprintf(buffer, sizeof(buffer), "%lld", value);
				printer.write(
				    {reinterpret_cast<string::value_type const*>(buffer),
				     static_cast<size_t>(length)});
			}

			void operator()(double value) const {
				char buffer[200];  // NOLINT
				auto length = snprintf(buffer, sizeof(buffer), "%g", value);
				printer.write(
				    {reinterpret_cast<string::value_type const*>(buffer),
				     static_cast<size_t>(length)});
			}

			void operator()(string const& value) const { write_string(value); }

			void operator()(map const& values) const {
				if (force_one_line || size_judge{config.horiz_space}(values)) {
					printer.write('{');
					bool first = true;
					for (auto const& key : values.keys()) {
						if (first)
							first = false;
						else
							printer.write(config.separators.alt_item);

						write_string(key);
						printer.write(config.separators.key);
						writer{printer, config, indent_size, true}.write(
						    values.find(key)->second);
					}
					printer.write('}');
					return;
				}

				if (values.empty() && config.inline_single_item) {
					printer.write(u8"{}"sv);
					return;
				}
				printer.write('{');
				if (values.size() == 1 && config.inline_single_item) {
					auto const& [key, value] = values.front();
					write_string(key);
					printer.write(config.separators.key);
					writer{printer, config, indent_size}.write(value);
					printer.write('}');
					return;
				}
				bool first = true;
				for (auto const& key : values.keys()) {
					if (first)
						first = false;
					else
						printer.write(config.separators.item);

					indent();
					deeper();
					write_string(key);
					printer.write(config.separators.key);
					writer{printer, config, indent_size + 1}.write(
					    values.find(key)->second);
				}
				indent();
				printer.write('}');
			}

			void operator()(array const& values) const {
				if (force_one_line || size_judge{config.horiz_space}(values)) {
					printer.write('[');
					bool first = true;
					for (auto const& value : values) {
						if (first)
							first = false;
						else
							printer.write(config.separators.alt_item);
						writer{printer, config, indent_size, true}.write(value);
					}
					printer.write(']');
					return;
				}

				if (values.empty() && config.inline_single_item) {
					printer.write(u8"[]"sv);
					return;
				}

				printer.write('[');
				if (values.size() == 1 && config.inline_single_item) {
					writer{printer, config, indent_size}.write(values.front());
					printer.write(']');
					return;
				}

				if (config.inline_single_item) {
					bool valid = true;
					for (auto const& item : values) {
						if (std::holds_alternative<string>(item) ||
						    std::holds_alternative<map>(item) ||
						    std::holds_alternative<array>(item)) {
							valid = false;
							break;
						}
					}

					if (valid) {
						bool first = true;
						for (auto const& value : values) {
							if (first)
								first = false;
							else
								printer.write(config.separators.alt_item);
							writer{printer, config, indent_size}.write(value);
						}
						printer.write(']');
						return;
					}
				}

				bool first = true;
				for (auto const& value : values) {
					if (first)
						first = false;
					else
						printer.write(config.separators.item);

					indent();
					deeper();
					writer{printer, config, indent_size + 1}.write(value);
				}
				indent();
				printer.write(']');
			}

			void indent() const {
				if (std::holds_alternative<std::monostate>(config.indent))
					return;
				printer.write('\n');

				auto const value = std::get<std::u8string_view>(config.indent);
				if (value.empty()) return;
				for (unsigned counter = 0; counter < indent_size; ++counter)
					printer.write(value);
			}

			void deeper() const {
				if (std::holds_alternative<std::monostate>(config.indent))
					return;

				auto const value = std::get<std::u8string_view>(config.indent);
				if (value.empty()) return;
				printer.write(value);
			}

			void write_string(string const& s) const {
				printer.write('"');
				for (auto c : s) {
					switch (c) {
						default:
							if (static_cast<unsigned char>(c) <= 0x1F)
								write_control(c);
							else
								printer.write(c);
							break;
						case '\"':
							printer.write(u8"\\\""sv);
							break;
						case '\\':
							printer.write(u8"\\\\"sv);
							break;
						case '\b':
							printer.write(u8"\\b"sv);
							break;
						case '\f':
							printer.write(u8"\\f"sv);
							break;
						case '\n':
							printer.write(u8"\\n"sv);
							break;
						case '\r':
							printer.write(u8"\\r"sv);
							break;
						case '\t':
							printer.write(u8"\\t"sv);
							break;
					}
				}
				printer.write('"');
			}

			void write_control(string::value_type c) const {
				char buff[10];  // NOLINT
#ifdef _MSC_VER
#pragma warning(push)
// 'sprintf': This function or variable may be unsafe. Consider using sprintf_s
// instead.

// Well, no
#pragma warning(disable : 4996)
#endif
				auto const len =
				    sprintf(buff, "%04x", static_cast<unsigned char>(c));
#ifdef _MSC_VER
#pragma warning(pop)
#endif
				printer.write(u8"\\u"sv);
				printer.write(
				    {reinterpret_cast<string::value_type const*>(buff),
				     static_cast<size_t>(len)});
			}
		};

		class file_output final : public output {
		public:
			file_output(FILE* file) : file_{file} {};

			void write(string_view str) override {
				if (!str.empty()) std::fwrite(str.data(), 1, str.size(), file_);
			}
			void write(byte_type c) override { std::fputc(c, file_); }

		private:
			FILE* file_{};
		};

		class string_output final : public output {
		public:
			string_output(string& str) : str_{str} {};

			void write(string_view str) override { str_.append(str); }
			void write(byte_type c) override { str_.push_back(c); }

		private:
			string& str_;
		};
	}  // namespace

	node read_json(string_view text,
	               string_view skip,
	               read_mode mode,
	               size_t* read) {
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
		} else if (mode == read_mode::serialized && read) {
			skip_ws(it, text.end(), mode);
			auto const difference = std::distance(text.begin(), it);
			*read = static_cast<size_t>(difference);
		}

		return value;
	}

	output::~output() = default;

	void write_json(output& printer,
	                node const& value,
	                write_config const& config) {
		write_config cfg{config};
		std::u8string indent{};
		{
			static constexpr auto missing_key = u8": "sv;
			static constexpr auto missing_empty_item = u8", "sv;
			static constexpr auto missing_non_empty_item = u8","sv;
			if (cfg.separators.key.empty()) cfg.separators.key = missing_key;
			if (cfg.separators.item.empty()) {
				if (std::holds_alternative<std::monostate>(cfg.indent))
					cfg.separators.item = missing_empty_item;
				else
					cfg.separators.item = missing_non_empty_item;
			}
			if (std::holds_alternative<int>(cfg.indent)) {
				auto const indent_value = std::get<int>(cfg.indent);
				if (indent_value > 0)
					indent =
					    std::u8string(static_cast<unsigned>(indent_value), ' ');
				cfg.indent = indent;
			}
		}
		writer{.printer = printer, .config = cfg}.write(value);
	}

	void write_json(FILE* file, node const& value, write_config const& config) {
		file_output printer{file};
		write_json(printer, value, config);
	}

	void write_json(string& output,
	                node const& value,
	                write_config const& config) {
		output.clear();
		string_output printer{output};
		write_json(printer, value, config);
	}
}  // namespace json
