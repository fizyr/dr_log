#pragma once
#include "chrono.hpp"
#include "../timed_action.hpp"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template<typename Archive, typename T>
void load(Archive & archive, dr::TimedAction<T> const & action, const unsigned int) {
	using boost::serialization::make_nvp;

	typename dr::TimedAction<T>::Duration duration;

	archive >> make_nvp("start",     action.start());
	archive >> make_nvp("duration",  duration);
	archive >> make_nvp("value",     action.value());
	action.end() = action.start() + duration;
}

template<typename Archive, typename T>
void save(Archive & archive, dr::TimedAction<T> const & action, const unsigned int) {
	using boost::serialization::make_nvp;

	archive << make_nvp("start",     action.start());
	archive << make_nvp("duration",  action.duration());
	archive << make_nvp("value",     action.value());
}

template<typename Archive, typename T>
void serialize(Archive & archive, dr::TimedAction<T> const & action, const unsigned int version) {
	boost::serialization::split_free(archive, action, version);
}

}}
