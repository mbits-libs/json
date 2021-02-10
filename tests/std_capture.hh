// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#pragma once

#include <stdio.h>
#include <atomic>
#include <string>
#include <thread>

class std_capture {
public:
	std_capture() = default;

	void lock() noexcept;
	void unlock() noexcept;
	std::string const& str() const& noexcept { return buffer_; }
	std::string str() && noexcept { return std::move(buffer_); }

private:
	void capture();
	void append();
	std::string buffer_{};
	std::atomic<bool> capturing_{false};
	int oldfd_{-1};
	int pipe_[2];
	std::thread reader_{};
};