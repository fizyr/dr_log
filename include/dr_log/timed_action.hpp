#pragma once

#include <boost/optional.hpp>

#include <chrono>
#include <utility>

template<typename T>
class TimedAction {
	using Clock     = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Duration  = Clock::duration;

	TimePoint start_;
	TimePoint end_;
	boost::optional<T> value_;
	std::string reason_;

public:
	void start() {
		start_ = Clock::now();
	}

	template<typename Y>
	void success(Y && value) {
		value_ = std::forward<Y>(value);
	}

	void failure(std::string reason) {
		reason_ = std::move(reason);
	}

	boost::optional<T> const & value() const {
		return value_;
	}

	bool succeeded() const {
		return !!value_;
	}

	std::string const & failureReason() const {
		return reason_;
	}

	explicit operator bool() const {
		return succeeded();
	}

	Duration duration() const {
		return end_ - start_;
	}

	template<typename Y>
	Duration timeSince(TimedAction<Y> const & other) const {
		return end_ - other.start_;
	}
};
