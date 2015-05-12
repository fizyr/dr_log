#define BOOST_LOG_USE_NATIVE_SYSLOG
#define BOOST_LOG_DYN_LINK

#include <string>
#include <boost/log/common.hpp>
#include <boost/log/sources/severity_logger.hpp>

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

using Logger = boost::log::sources::severity_logger<LogLevel>;

// Initialize the logging library.
void setupLogging(std::string const & log_file, std::string const & name);

}
