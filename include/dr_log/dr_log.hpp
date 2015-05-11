#define BOOST_LOG_USE_NATIVE_SYSLOG
#define BOOST_LOG_DYN_LINK

#include <string>
#include <boost/log/common.hpp>
#include <boost/log/sources/logger.hpp>

namespace dr {

/// Available log levels.
enum class LogLevel {
	debug = 0,
	info,
	success,
	warning,
	error,
	critical,
};

// Initialize the logging library.
void setupLogging(std::string const & base_dir, std::string const & name);

}
