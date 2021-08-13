// Copyright (c) 2021 midnightBITS
// This code is licensed under MIT license (see LICENSE for details)

#include "std_capture.hh"
#include <cstdint>
#ifdef _MSC_VER
#include <io.h>
#define popen _popen
#define pclose _pclose
#define stat _stat
#define dup _dup
#define dup2 _dup2
#define fileno _fileno
#define close _close
#define pipe _pipe
#define read _read
#define eof _eof
using ssize_t = intptr_t;
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <cerrno>
#include <chrono>
#include <cstdio>

using namespace std::literals;

namespace {
	enum PIPE { READ, WRITE };

	template <typename Pred>
	int EINTR_(Pred pred) noexcept {
		int ret = -1;
		while (true) {
			ret = pred();
			if (ret < 0 && (errno == EINTR || errno == EBUSY)) {
				std::this_thread::sleep_for(10ms);
				continue;
			}
			break;
		};
		return ret;
	}

	int dup_(int fd) noexcept {
		return EINTR_([=] { return dup(fd); });
	}

	int dup2_(int fdA, int fdB) noexcept {
		return EINTR_([=] { return dup2(fdA, fdB); });
	}

	int pipe_(int* pipes) noexcept {
#ifdef _MSC_VER
		return EINTR_([=] { return pipe(pipes, 65536, O_BINARY); });
#else
		return EINTR_([=] { return pipe(pipes); });
#endif
	}

	int close_(int fd) noexcept {
		return EINTR_([=] { return close(fd); });
	}
}  // namespace

void std_capture::lock() noexcept {
	if (capturing_) return;

	::pipe_(pipe_);
	oldfd_ = dup_(fileno(stdout));
	dup2_(pipe_[WRITE], fileno(stdout));
#ifndef _MSC_VER
	close_(pipe_[WRITE]);
#endif
	// auto const flags = fcntl(pipe_[READ], F_GETFL);
	// fcntl(pipe_[READ], F_SETFL, flags | O_NONBLOCK);

	capturing_ = true;
	reader_ = std::thread{[](std_capture* self) { self->capture(); }, this};
}

void std_capture::unlock() noexcept {
	if (!capturing_) return;
	capturing_ = false;

	fflush(stdout);
	dup2_(oldfd_, fileno(stdout));
	close_(oldfd_);
#ifdef _MSC_VER
	close_(pipe_[WRITE]);
#endif

	reader_.join();

	close_(pipe_[READ]);
}

void std_capture::capture() {
	buffer_.clear();
	do {
		append();
	} while (capturing_);
}

void std_capture::append() {
	char buffer[100];
	ssize_t ret = 0;
	while (true) {
		ret = read(pipe_[READ], buffer, sizeof(buffer));
		auto const err = errno;
		if (ret < 0) {
			if (err == EINTR || err == EBUSY || err == EAGAIN) {
				std::this_thread::sleep_for(10ms);
				continue;
			}
			return;
		}
		break;
	}

	buffer_.append(buffer, static_cast<size_t>(ret));
}