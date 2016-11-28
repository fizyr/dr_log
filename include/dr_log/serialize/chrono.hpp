#pragma once
#include <boost/serialization/split_free.hpp>
#include <chrono>

namespace boost {
namespace serialization {

template<typename Archive, typename Rep, typename Period>
void load(Archive & archive, std::chrono::duration<Rep, Period> & duration, const unsigned int) {
	Rep rep;
	archive >> rep;
	duration = std::chrono::duration<Rep, Period>(rep);
}

template<typename Archive, typename Rep, typename Period>
void save(Archive & archive, std::chrono::duration<Rep, Period> const & duration, const unsigned int) {
	archive << duration.count();
}

template<typename Archive, typename Rep, typename Period>
void serialize(Archive & archive, std::chrono::duration<Rep, Period> const & duration, const unsigned int version) {
	boost::serialization::split_free(archive, duration, version);
}

template<typename Archive, typename Clock, typename Duration>
void load(Archive & archive, std::chrono::time_point<Clock, Duration> & time_point, const unsigned int) {
	Duration time_since_epoch;
	archive >> time_since_epoch;
	time_point = std::chrono::time_point<Clock, Duration>(time_since_epoch);
}

template<typename Archive, typename Clock, typename Duration>
void save(Archive & archive, std::chrono::time_point<Clock, Duration> const & time_point, const unsigned int) {
	archive << time_point.time_since_epoch();
}

template<typename Archive, typename Clock, typename Duration>
void serialize(Archive & archive, std::chrono::time_point<Clock, Duration> const & time_point, const unsigned int version) {
	boost::serialization::split_free(archive, time_point, version);
}

}}
